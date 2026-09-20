#ifndef MAGVID_AVI_PLAYER_H
#define MAGVID_AVI_PLAYER_H

#include <windows.h>
#include <vfw.h>

#include "avi_draw.h"

struct MagVid;

// Playback/decompression state allocated by LoadAVI.  The original object is
// 0x128 bytes.  Fields will replace the reserved ranges as their users are
// reconstructed; current_frame is already corroborated by seek/draw code.
class AviPlayer
{
public:
  AviPlayer(void);
  // SYNTHETIC: MAGVID 0x10002C40
  // AviPlayer::`scalar deleting destructor'
  ~AviPlayer(void);

  int open_file(const char *path);
  int open_streams(HWND window, int sound_id);
  int initialize_video_stream(void);
  int close_file(void);
  int is_open(void);
  void advance_frame(void);
  int seek_frame(int frame);
  int step_frames(int count);
  int release_audio(void);
  int unload_audio(void);
  int stop_audio(void);
  int play_audio(void);
  int get_audio_time(void);
  LONG video_time_to_sample(LONG time);
  int read_audio_buffers(int count);
  int submit_audio_buffers(void);
  int refill_audio_buffer(void);
  int read_audio_stream_info(int sound_id);
  int start_audio(int position, int reserved, int position_is_sample);
  void invalidate_frame_cache(void);
  int set_background_dib(MagDib *dib);
  int set_codec_palette(LOGPALETTE *palette);
  int load_background(char *path);
  int start_playback(HDC device_context);
  int start_video(HDC device_context, int frame);
  int stop_playback(void);
  int stop_video(void);
  int release_video(void);
  int close_streams(void);
  void clear_thread_state(void);
  void adjust_playback_clock(int milliseconds);
  int is_playback_finished(void);
  int render_next_frame(void);
  LONG video_time_to_frame(LONG time);
  LONG video_frame_to_time(LONG frame);
  int is_key_frame(int frame);
  int prepare_frame(void);
  void catch_up_frames(void);

public:
  AviDraw *draw;                // 0x00
  MagDib *background_dib;       // 0x04

private:
  PAVIFILE avi_file;            // 0x08
  PAVISTREAM video_stream;      // 0x0c
  PAVISTREAM audio_stream;      // 0x10
  HWND notification_window;     // 0x14
  int playback_active;          // 0x18
  int playback_ended;           // 0x1c
  float frame_period;           // 0x20
  float stream_frame_period;    // 0x24
  UINT timer_resolution;        // 0x28
  int reserved_2c;              // 0x2c
public:
  HDC playback_dc;              // 0x30

private:
  int playback_start_tick;      // 0x34
  int playback_start_time;      // 0x38
  int playback_reference_frame; // 0x3c
  int dropped_frame_count;      // 0x40
  int current_frame;            // 0x44
  int decoded_frame_index;      // 0x48
  int previous_key_frame;       // 0x4c
  int next_key_frame;           // 0x50
  int first_frame;              // 0x54
  int last_frame;               // 0x58
  void *video_buffer;           // 0x5c
  int reserved_60;              // 0x60
  int reserved_64[4];           // 0x64; intentionally not initialized here
  HWAVEOUT wave_out;            // 0x74
  int audio_start_time;         // 0x78
  int first_audio_sample;       // 0x7c
  int last_audio_sample;        // 0x80
  int audio_sample_size;        // 0x84
  int reserved_88[2];           // 0x88
  int audio_block_size;         // 0x90
  int audio_sample_divisor;     // 0x94
  int audio_sample;             // 0x98
  WAVEHDR *audio_buffers[32];   // 0x9c
  int audio_read_index;         // 0x11c
  int audio_write_index;        // 0x120
  int audio_queued_buffers;     // 0x124

  friend void __cdecl video_playback_thread(MagVid *state);
};

#endif
