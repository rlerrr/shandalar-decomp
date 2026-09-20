#include "avi_player.h"
#include <stdlib.h>
#include <string.h>
extern "C"
{
#include "magsnd.h"
}

// GLOBAL: MAGVID 0x1000C088
static int g_avi_sound_id = 0x100;

// GLOBAL: MAGVID 0x1000C068
Sound g_avi_sound = {400, 0, 0, 0, 0, 0, 0, 0x14};

struct VideoProfileEntry
{
  char label[0x80];
  DWORD start_time;
  DWORD end_time;
  int values[4];

  void begin(char *text);
  void end(int value1, int value2, int value3, int value4);
};

struct VideoProfileLog
{
  unsigned int index;
  VideoProfileEntry entries[256];

  void operator++(int);
  void begin(char *text);
  void end(int value1, int value2, int value3, int value4);
};

// GLOBAL: MAGVID 0x1001F238
VideoProfileLog g_video_profile;

// GLOBAL: MAGVID 0x10015A08
AviPlayer *g_audio_callback_player;

// FUNCTION: MAGVID 0x10003A90
void CALLBACK audio_callback(HWAVEOUT wave_out, UINT message, DWORD instance,
                             DWORD param1, DWORD param2)
{
  switch (message)
  {
    case WOM_OPEN:
      g_audio_callback_player = (AviPlayer *)instance;
      break;
    case WOM_CLOSE:
      g_audio_callback_player = NULL;
      break;
    case WOM_DONE:
      if (g_audio_callback_player != NULL)
      {
        g_audio_callback_player->refill_audio_buffer();
      }
      break;
  }
}

// FUNCTION: MAGVID 0x10002D00
int AviPlayer::is_open(void)
{
  return avi_file != NULL;
}

// FUNCTION: MAGVID 0x1000329E
int AviPlayer::start_playback(HDC device_context)
{
  TIMECAPS timer_caps;

  if (video_stream == NULL)
  {
    return -1;
  }
  if (playback_active != 0)
  {
    return -4;
  }
  playback_active = 1;
  timeGetDevCaps(&timer_caps, sizeof(TIMECAPS));
  timer_resolution = (timer_caps.wPeriodMin > 1) ?
      timer_caps.wPeriodMin : 1;
  timeBeginPeriod(timer_resolution);
  playback_ended = 0;
  start_video(device_context, -1);
  return 0;
}

// FUNCTION: MAGVID 0x10002D30
void AviPlayer::advance_frame(void)
{
  current_frame++;
}

// FUNCTION: MAGVID 0x10002D50
AviPlayer::AviPlayer(void)
{
  int i;

  draw = NULL;
  background_dib = NULL;
  avi_file = NULL;
  video_stream = NULL;
  audio_stream = NULL;
  notification_window = NULL;
  playback_active = 0;
  playback_ended = 0;
  frame_period = 0.0f;
  stream_frame_period = 0.0f;
  timer_resolution = 0;
  reserved_2c = 0;
  playback_dc = NULL;
  playback_start_tick = 0;
  playback_start_time = 0;
  playback_reference_frame = 0;
  dropped_frame_count = 0;
  current_frame = 0;
  decoded_frame_index = 0;
  previous_key_frame = 0;
  next_key_frame = 0;
  first_frame = 0;
  last_frame = 0;
  video_buffer = NULL;
  reserved_60 = 0;
  wave_out = NULL;
  audio_start_time = 0;
  first_audio_sample = 0;
  last_audio_sample = 0;
  audio_sample_size = 0;
  reserved_88[0] = 0;
  reserved_88[1] = 0;
  audio_block_size = 0;
  audio_sample_divisor = 0;
  audio_sample = 0;
  for (i = 0; i < 32; i++)
  {
    audio_buffers[i] = NULL;
  }
  audio_read_index = 0;
  audio_write_index = 0;
  audio_queued_buffers = 0;
  AVIFileInit();
}

// FUNCTION: MAGVID 0x10002F36
AviPlayer::~AviPlayer(void)
{
  close_streams();
  close_file();
  AVIFileExit();
}

// FUNCTION: MAGVID 0x10002F61
int AviPlayer::open_file(const char *path)
{
  int result;

  if (avi_file != NULL)
  {
    close_file();
  }
  audio_stream = NULL;
  video_stream = audio_stream;
  result = AVIFileOpenA(&avi_file, path, OF_SHARE_DENY_WRITE, NULL);
  if (result != 0)
  {
    close_file();
    return result;
  }
  return 0;
}

// FUNCTION: MAGVID 0x10003012
int AviPlayer::open_streams(HWND window, int sound_id)
{
  AVISTREAMINFO info;
  int result;

  if ((video_stream != NULL) || (audio_stream != NULL))
  {
    close_streams();
  }
  playback_ended = 0;
  playback_active = playback_ended;
  notification_window = window;
  draw = NULL;
  wave_out = NULL;
  result = AVIFileGetStream(avi_file, &video_stream, streamtypeVIDEO, 0);
  if (result == AVIERR_NODATA)
  {
    video_stream = NULL;
    return -1;
  }
  result = AVIFileGetStream(avi_file, &audio_stream, streamtypeAUDIO, 0);
  if (result == AVIERR_NODATA)
  {
    audio_stream = NULL;
  }
  else
  {
    sound_load((const char *)audio_stream, sound_id + 0x100,
               &g_avi_sound);
  }
  AVIStreamInfo(video_stream, &info, sizeof(info));
  stream_frame_period = (float)info.dwScale / (float)info.dwRate;
  frame_period = stream_frame_period;
  result = initialize_video_stream();
  return result;
}

// FUNCTION: MAGVID 0x10007450
int AviPlayer::initialize_video_stream(void)
{
  AVISTREAMINFO info;
  BITMAPINFOHEADER format;
  LONG format_size;
  AviDraw *new_draw;
  int result;

  if (video_stream == NULL)
  {
    return -20;
  }
  if (draw != NULL)
  {
    release_video();
  }
  playback_start_tick = -1;
  AVIStreamInfo(video_stream, &info, sizeof(info));
  current_frame = info.dwStart;
  first_frame = current_frame;
  last_frame = info.dwStart + info.dwLength - 1;
  decoded_frame_index = current_frame - 1;
  format_size = 0;
  AVIStreamReadFormat(video_stream, 0, NULL, &format_size);
  if (format_size != sizeof(BITMAPINFOHEADER))
  {
    return -1;
  }
  result = AVIStreamReadFormat(video_stream, 0, &format, &format_size);
  new_draw = new AviDraw;
  draw = new_draw;
  result = draw->open_codec(info.fccHandler, &format);
  if (result != 0)
  {
    release_video();
    return -21;
  }
  reserved_60 = info.dwSuggestedBufferSize;
  video_buffer = malloc(reserved_60);
  if (video_buffer == NULL)
  {
    return 3;
  }
  dropped_frame_count = 0;
  return 0;
}

// FUNCTION: MAGVID 0x10002FD7
int AviPlayer::close_file(void)
{
  if (avi_file != NULL)
  {
    AVIFileRelease(avi_file);
    avi_file = NULL;
  }
  return 0;
}

// FUNCTION: MAGVID 0x100033CA
int AviPlayer::seek_frame(int frame)
{
  struct
  {
    LONG audio_position;
    LONG video_time;
  } local;

  if (playback_active != 0)
  {
    return -1;
  }
  current_frame = frame;
  if (current_frame < first_frame)
  {
    current_frame = first_frame;
  }
  else if (last_frame < current_frame)
  {
    current_frame = last_frame;
  }
  if (first_frame == current_frame)
  {
    decoded_frame_index = -1;
    previous_key_frame = -1;
    next_key_frame = -1;
  }
  if ((audio_stream != NULL) && (audio_sample_divisor != 0))
  {
    local.video_time = AVIStreamSampleToTime(video_stream, frame);
    local.audio_position = AVIStreamTimeToSample(audio_stream,
                                                 local.video_time);
    audio_sample = local.audio_position / audio_sample_divisor;
  }
  return current_frame;
}

// FUNCTION: MAGVID 0x100034D2
int AviPlayer::step_frames(int count)
{
  struct
  {
    LONG audio_position;
    LONG video_time;
  } local;

  if (playback_active != 0)
  {
    return -1;
  }
  current_frame += count;
  if (current_frame < first_frame)
  {
    current_frame = first_frame;
  }
  else if (last_frame < current_frame)
  {
    current_frame = last_frame;
  }
  if ((audio_stream != NULL) && (audio_sample_divisor != 0))
  {
    local.video_time = AVIStreamSampleToTime(video_stream, current_frame);
    local.audio_position = AVIStreamTimeToSample(audio_stream,
                                                 local.video_time);
    audio_sample = local.audio_position / audio_sample_divisor;
  }
  return current_frame;
}

// FUNCTION: MAGVID 0x10003BF8
int AviPlayer::release_audio(void)
{
  audio_start_time = -1;
  sound_unload(g_avi_sound_id);
  return 0;
}

// FUNCTION: MAGVID 0x10003D1D
int AviPlayer::unload_audio(void)
{
  sound_unload(g_avi_sound_id);
  return 0;
}

// FUNCTION: MAGVID 0x10003D43
int AviPlayer::stop_audio(void)
{
  sound_stop(g_avi_sound_id);
  return 0;
}

// FUNCTION: MAGVID 0x10003D69
int AviPlayer::play_audio(void)
{
  sound_play(g_avi_sound_id, NULL);
  return 0;
}

// FUNCTION: MAGVID 0x10003D91
int AviPlayer::get_audio_time(void)
{
  unsigned int audio_time;

  if (sound_get_time(g_avi_sound_id, &audio_time) != 0)
  {
    return -14;
  }
  return -1;
}

// FUNCTION: MAGVID 0x10003DD0
LONG AviPlayer::video_time_to_sample(LONG time)
{
  LONG sample;

  sample = AVIStreamTimeToSample(video_stream, time);
  return sample;
}

// FUNCTION: MAGVID 0x10003DFE
int AviPlayer::read_audio_buffers(int count)
{
  struct
  {
    int *read_index;
    int result;
    LONG bytes_read;
    int buffers_read;
  } local;

  for (local.buffers_read = 0; local.buffers_read < count;
       local.buffers_read++)
  {
    if (last_audio_sample < audio_sample)
    {
      break;
    }
    local.result = AVIStreamRead(audio_stream,
                           (audio_sample + local.buffers_read) * audio_sample_divisor,
                           audio_sample_divisor,
                           audio_buffers[audio_read_index]->lpData,
                           audio_block_size * audio_sample_divisor,
                           &local.bytes_read, NULL);
    if ((local.result != 0) ||
        (audio_block_size * audio_sample_divisor != local.bytes_read))
    {
      return local.buffers_read;
    }
    audio_read_index++;
    local.read_index = &audio_read_index;
    *local.read_index %= 32;
  }
  audio_sample += local.buffers_read;
  return local.buffers_read;
}

// FUNCTION: MAGVID 0x10003F1A
int AviPlayer::submit_audio_buffers(void)
{
  struct
  {
    int *write_index;
    MMRESULT result;
    int submitted;
  } local;

  for (local.submitted = 0; local.submitted < 32; local.submitted++)
  {
    if (audio_queued_buffers >= 32)
    {
      break;
    }
    if (audio_read_index == audio_write_index)
    {
      break;
    }
    local.result = waveOutWrite(wave_out, audio_buffers[audio_write_index],
                                sizeof(WAVEHDR));
    audio_queued_buffers++;
    audio_write_index++;
    local.write_index = &audio_write_index;
    *local.write_index %= 32;
  }
  return local.submitted;
}

// FUNCTION: MAGVID 0x10003FDC
int AviPlayer::refill_audio_buffer(void)
{
  if (audio_start_time < 0)
  {
    return -1;
  }
  if (playback_active == 0)
  {
    return -1;
  }
  read_audio_buffers(1);
  audio_queued_buffers--;
  submit_audio_buffers();
  return 0;
}

// FUNCTION: MAGVID 0x100039E1
int AviPlayer::set_background_dib(MagDib *dib)
{
  if ((video_stream == NULL) || (draw == NULL))
  {
    return -1;
  }
  if (playback_active != 0)
  {
    return -4;
  }
  background_dib = dib;
  if (draw->set_overlay_target(background_dib) != 0)
  {
    return 0;
  }
  else
  {
    return -5;
  }
}

// FUNCTION: MAGVID 0x10003837
int AviPlayer::set_codec_palette(LOGPALETTE *palette)
{
  if ((video_stream == NULL) || (draw == NULL))
  {
    return -1;
  }
  if (draw->set_palette(palette) != 0)
  {
    return 0;
  }
  return -5;
}

// FUNCTION: MAGVID 0x10003899
int AviPlayer::load_background(char *path)
{
  BITMAPINFOHEADER format;
  MagDib *dib;

  if ((video_stream == NULL) || (draw == NULL))
  {
    return -1;
  }
  if (playback_active != 0)
  {
    return -4;
  }
  if (path != NULL)
  {
    draw->get_decoded_format(&format);
    dib = new MagDib;
    background_dib = dib;
    if (dib->load_bitmap(GetActiveWindow(), path, format.biBitCount) == 0)
    {
      return -5;
    }
  }
  if (draw->set_overlay_target(background_dib) == 0)
  {
    return -5;
  }
  return 0;
}

// FUNCTION: MAGVID 0x10003338
int AviPlayer::stop_playback(void)
{
  playback_active = 0;
  playback_ended = 0;
  unload_audio();
  stop_video();
  timeEndPeriod(timer_resolution);
  return 0;
}

// FUNCTION: MAGVID 0x100077F1
int AviPlayer::stop_video(void)
{
  if (playback_dc == NULL)
  {
    return -22;
  }
  draw->stop_codec_playback();
  draw->end_draw();
  playback_start_tick = -1;
  playback_dc = NULL;
  return 0;
}

// FUNCTION: MAGVID 0x10007677
int AviPlayer::release_video(void)
{
  if (video_buffer != NULL)
  {
    free(video_buffer);
    video_buffer = NULL;
  }
  if (draw != NULL)
  {
    delete draw;
    draw = NULL;
  }
  return 0;
}

// FUNCTION: MAGVID 0x10003226
int AviPlayer::close_streams(void)
{
  stop_playback();
  release_video();
  release_audio();
  if (video_stream != NULL)
  {
    AVIStreamRelease(video_stream);
  }
  if (audio_stream != NULL)
  {
    AVIStreamRelease(audio_stream);
  }
  audio_stream = NULL;
  video_stream = audio_stream;
  return 0;
}

// FUNCTION: MAGVID 0x100073F0
void AviPlayer::clear_thread_state(void)
{
  dropped_frame_count = 0;
}

// FUNCTION: MAGVID 0x100077D0
void AviPlayer::adjust_playback_clock(int milliseconds)
{
  playback_start_tick += milliseconds;
}

// FUNCTION: MAGVID 0x100073B0
int AviPlayer::is_playback_finished(void)
{
  return (last_frame == current_frame) ? 1 : 0;
}

// FUNCTION: MAGVID 0x100076F2
int AviPlayer::start_video(HDC device_context, int frame)
{
  if (draw == NULL)
  {
    return -21;
  }
  if (device_context == NULL)
  {
    return -22;
  }
  playback_dc = device_context;
  if (frame >= 0)
  {
    current_frame = frame;
  }
  if (draw->begin_draw(device_context, stream_frame_period) != 0)
  {
    stop_video();
    return -21;
  }
  if (playback_active != 0)
  {
    draw->start_codec_playback(stream_frame_period);
  }
  playback_start_tick = timeGetTime();
  playback_start_time = AVIStreamSampleToTime(video_stream, current_frame);
  playback_reference_frame = current_frame;
  return 0;
}

// FUNCTION: MAGVID 0x10007877
int AviPlayer::render_next_frame(void)
{
  struct
  {
    int playback_time;
    LONG samples_read;
    DWORD result;
    LONG bytes_read;
  } local;

  if (draw == NULL)
  {
    return -21;
  }
  update_snd();
  if (playback_start_tick < 0)
  {
    return -23;
  }
  if (playback_active != 0)
  {
    local.playback_time = get_audio_time();
    if (local.playback_time < 0)
    {
      local.playback_time = (timeGetTime() - playback_start_tick) +
                            playback_start_time;
    }
    current_frame = video_time_to_frame(local.playback_time);
    if (last_frame < current_frame)
    {
      current_frame = last_frame;
    }
    if (decoded_frame_index == current_frame)
    {
      update_snd();
      return 0;
    }
  }
  else
  {
    if (last_frame < current_frame)
    {
      current_frame = last_frame;
    }
    if (decoded_frame_index == current_frame)
    {
      invalidate_frame_cache();
    }
  }
  if (prepare_frame() == 0)
  {
    return 0;
  }
  local.result = AVIStreamRead(video_stream, current_frame, 1, video_buffer,
                               reserved_60, &local.bytes_read,
                               &local.samples_read);
  if (local.result != 0)
  {
    return -24;
  }
  g_video_profile.begin("vidsdraw, vcmDraw(m_pvBuf), rval");
  local.result = draw->draw_frame(video_buffer, 0);
  if ((int)local.result > 1)
  {
    g_video_profile.end(current_frame, local.result,
                        playback_start_tick, 0);
    g_video_profile++;
    return -21;
  }
  g_video_profile.end(current_frame, local.result, 0, 0);
  g_video_profile++;
  decoded_frame_index = current_frame;
  if (playback_active != 0)
  {
    dropped_frame_count +=
        (current_frame - playback_reference_frame) - 1;
    playback_reference_frame = current_frame;
  }
  update_snd();
  return 0;
}

// FUNCTION: MAGVID 0x10007AA1
LONG AviPlayer::video_time_to_frame(LONG time)
{
  LONG result;

  result = AVIStreamTimeToSample(video_stream, time);
  return result;
}

// FUNCTION: MAGVID 0x10007ACF
LONG AviPlayer::video_frame_to_time(LONG frame)
{
  LONG result;

  result = AVIStreamSampleToTime(video_stream, frame);
  return result;
}

// FUNCTION: MAGVID 0x10007AFD
int AviPlayer::is_key_frame(int frame)
{
  if (video_stream == NULL)
  {
    return 0;
  }
  if (frame < 0)
  {
    frame = current_frame;
  }
  return AVIStreamFindSample(video_stream, frame, 0x14) == frame;
}

// FUNCTION: MAGVID 0x10007B63
int AviPlayer::prepare_frame(void)
{
  int distance_to_next;
  int distance_to_previous;

  if (current_frame < decoded_frame_index)
  {
    decoded_frame_index = -1;
  }
  if (current_frame - decoded_frame_index == 1)
  {
    return 1;
  }
  else
  {
    if (AVIStreamFindSample(video_stream, current_frame, 0x14) ==
        current_frame)
    {
      previous_key_frame = current_frame;
      next_key_frame =
          AVIStreamFindSample(video_stream, current_frame + 1, 0x11);
    }
    else if (current_frame - decoded_frame_index == 2)
    {
      catch_up_frames();
    }
    else
    {
      if ((next_key_frame < current_frame) ||
          (current_frame < previous_key_frame))
      {
        previous_key_frame =
            AVIStreamFindSample(video_stream, current_frame - 1, 0x14);
        next_key_frame =
            AVIStreamFindSample(video_stream, current_frame + 1, 0x11);
      }
      if (current_frame - previous_key_frame >= 0)
      {
        distance_to_previous = current_frame - previous_key_frame;
      }
      else
      {
        distance_to_previous = -(current_frame - previous_key_frame);
      }
      if (current_frame - next_key_frame >= 0)
      {
        distance_to_next = current_frame - next_key_frame;
      }
      else
      {
        distance_to_next = -(current_frame - next_key_frame);
      }
      if (distance_to_previous <= distance_to_next)
      {
        catch_up_frames();
      }
      else
      {
        if (playback_active != 0)
        {
          return 0;
        }
        catch_up_frames();
      }
    }
  }
  return 1;
}

// FUNCTION: MAGVID 0x10007D28
void AviPlayer::catch_up_frames(void)
{
  struct
  {
    int stream_result;
    int draw_result;
    int frame;
  } local;

  if (decoded_frame_index < previous_key_frame)
  {
    decoded_frame_index = previous_key_frame - 1;
  }
  local.frame = decoded_frame_index + 1;
  while (local.frame < current_frame)
  {
    local.stream_result = AVIStreamRead(video_stream, local.frame, 1,
                                        video_buffer, reserved_60, NULL, NULL);
    if (local.stream_result != 0)
    {
      break;
    }
    if (playback_active == 0)
    {
      g_video_profile.begin("vidsCatchup(), !m_bPlaying, DrawIn");
      local.draw_result = draw->catch_up_frame(video_buffer, 0);
      g_video_profile.end(current_frame, local.draw_result, 0, 0);
      g_video_profile++;
    }
    else
    {
      g_video_profile.begin(
          "vidsCatchup(), !m_bPlaying, DrawIn, HURRYUP");
      local.draw_result = draw->catch_up_frame(video_buffer, 0x80000000);
      g_video_profile.end(current_frame, local.draw_result, 0, 0);
      g_video_profile++;
    }
    decoded_frame_index = local.frame;
    local.frame++;
  }
}

// FUNCTION: MAGVID 0x10007EB0
void VideoProfileLog::operator++(int)
{
  index = index + 1;
  index &= 0xff;
}

// FUNCTION: MAGVID 0x10007EE0
void VideoProfileLog::begin(char *text)
{
  entries[index].begin(text);
}

// FUNCTION: MAGVID 0x10007F20
void VideoProfileLog::end(int value1, int value2, int value3, int value4)
{
  entries[index].end(value1, value2, value3, value4);
}

// FUNCTION: MAGVID 0x10007F70
void VideoProfileEntry::begin(char *text)
{
  strcpy(label, text);
  start_time = timeGetTime();
}

// FUNCTION: MAGVID 0x10007FB0
void VideoProfileEntry::end(int value1, int value2, int value3, int value4)
{
  end_time = timeGetTime();
  values[0] = value1;
  values[1] = value2;
  values[2] = value3;
  values[3] = value4;
}

// FUNCTION: MAGVID 0x10003B1A
int AviPlayer::read_audio_stream_info(int sound_id)
{
  AVISTREAMINFO info;

  audio_start_time = -1;
  if (audio_stream == NULL)
  {
    return -13;
  }
  AVIStreamInfo(audio_stream, &info, sizeof(info));
  audio_sample_divisor = info.dwSuggestedBufferSize * info.dwSampleSize;
  audio_block_size = info.dwSampleSize;
  first_audio_sample = info.dwStart;
  last_audio_sample = info.dwStart + info.dwLength / audio_sample_divisor;
  audio_sample_size = info.dwInitialFrames;
  if (audio_sample_size >= 32)
  {
    return -11;
  }
  return 0;
}

// FUNCTION: MAGVID 0x10003C28
int AviPlayer::start_audio(int position, int reserved, int position_is_sample)
{
  int result;

  if (playback_ended != 0)
  {
    sound_play(g_avi_sound_id, NULL);
    return 0;
  }
  result = read_audio_stream_info((int)notification_window);
  if (result != 0)
  {
    return result;
  }
  if (position_is_sample != 0)
  {
    audio_sample = position;
  }
  else
  {
    audio_sample = position / audio_sample_divisor;
  }
  audio_start_time = AVIStreamSampleToTime(
      audio_stream, audio_sample_divisor * audio_sample);
  if (sound_load((const char *)audio_stream, g_avi_sound_id,
                 &g_avi_sound) != 0)
  {
    return -13;
  }
  sound_play(g_avi_sound_id, NULL);
  return 0;
}

// FUNCTION: MAGVID 0x10003A70
void AviPlayer::invalidate_frame_cache(void)
{
  decoded_frame_index = -1;
}
