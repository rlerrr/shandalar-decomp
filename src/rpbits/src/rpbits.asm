.386
.model flat, c
option casemap:none

; ============================================================
; Engine-provided externals (stream I/O)
; ============================================================

EXTERN global_pcxw_image_width:DWORD
EXTERN global_pcxw_image_height:DWORD
EXTERN rpbits_stream_ptr:DWORD        ; g_rpbits_stream_ptr
EXTERN rpbits_stream_end:DWORD        ; PTR_DAT_1002252c
EXTERN rpbits_stream_refill:DWORD     ; g_rpbits_stream_refill
EXTERN RpBits_ApplyPalette:PROC

; ============================================================
; Decoder-private data (library local)
; ============================================================

.data

decoder_stack_ptr       dd 0          ; g_rpbits_decoder_stack_ptr
remaining_count         dd 0          ; g_rpbits_remaining_count
code_mask               dd 0          ; g_rpbits_code_mask
saved_edx               dd 0          ; g_rpbits_saved_edx
bit_buffer              dd 0          ; g_rpbits_bit_buffer

repeat_count            db 0          ; g_rpbits_repeat_count
last_literal            db 0          ; g_rpbits_last_literal
code_bits               db 0          ; g_rpbits_code_bits
max_code_bits           db 0          ; g_rpbits_max_code_bits
bit_count               db 0          ; g_rpbits_bit_count
packed_mode             db 0          ; g_rpbits_packed_mode

last_node_index         dd 0          ; g_rpbits_last_node_index
last_symbol             db 0          ; g_rpbits_last_symbol

align 4
decoder_table           db 2048*3 dup (0)  ; g_rpbits_decoder_table

; Dword written by RpBits_ReadTables header path (address 0x10026550 in original).
g_rpbits_stream_word            dd 0

; Dedicated decoder stack space. InitStream loads decoder_stack_ptr with the
; address of decoder_stack_sentinel, and DecodeRun swaps ESP with decoder_stack_ptr.
decoder_stack_space     db 0400h dup (0)
decoder_stack_sentinel  LABEL BYTE

; ============================================================
; Public entry points
; ============================================================

.code

PUBLIC RpBits_ReadTables
PUBLIC RpBits_InitStream
PUBLIC RpBits_ResetDecoder
PUBLIC RpBits_DecodeSymbol
PUBLIC RpBits_DecodeRun
PUBLIC RpBits_DecodeImage

; ============================================================
; RpBits_ReadTables
; ============================================================

RpBits_ReadTables PROC
    push ebp
    mov  ebp, esp
    push ebx
    push esi
    push edi

    xor eax, eax

ReadLoopTop:
    mov esi, dword ptr [rpbits_stream_ptr]
    cmp esi, dword ptr [rpbits_stream_end]
    jc  short ReadWord1

    push ebx
    push ecx
    push edx
    call dword ptr [rpbits_stream_refill]
    pop  edx
    pop  ecx
    pop  ebx
    mov esi, dword ptr [rpbits_stream_ptr]

ReadWord1:
    mov eax, 0
    lodsw
    mov dword ptr [rpbits_stream_ptr], esi

    cmp al, 58h
    jz  near ptr FoundHeader

    lea edi, [decoder_table]
    cmp ax, 304Dh
    jz  short MaybeRedirect
    cmp ax, 314Dh
    jz  short MaybeRedirect
    jmp short HaveDest

MaybeRedirect:
    cmp dword ptr [ebp+8], 1
    jl  short Add2ToDest
    jz  short HaveDest
    mov edi, dword ptr [ebp+8]
    jmp short HaveDest

Add2ToDest:
    add edi, 2

HaveDest:
    push edi
    stosw

    mov esi, dword ptr [rpbits_stream_ptr]
    cmp esi, dword ptr [rpbits_stream_end]
    jc  short ReadWord2

    push ebx
    push ecx
    push edx
    call dword ptr [rpbits_stream_refill]
    pop  edx
    pop  ecx
    pop  ebx
    mov esi, dword ptr [rpbits_stream_ptr]

ReadWord2:
    mov eax, 0
    lodsw
    mov dword ptr [rpbits_stream_ptr], esi
    stosw

    mov ecx, eax
    shr ecx, 1
    jecxz short AfterWordLoop

WordLoop:
    mov esi, dword ptr [rpbits_stream_ptr]
    cmp esi, dword ptr [rpbits_stream_end]
    jc  short ReadWord3

    push ebx
    push ecx
    push edx
    call dword ptr [rpbits_stream_refill]
    pop  edx
    pop  ecx
    pop  ebx
    mov esi, dword ptr [rpbits_stream_ptr]

ReadWord3:
    mov eax, 0
    lodsw
    mov dword ptr [rpbits_stream_ptr], esi
    stosw
    loop WordLoop

AfterWordLoop:
    pop edi
    lea eax, [decoder_table]
    cmp eax, edi
    jnz short ContinueLoop

    push eax
    call RpBits_ApplyPalette
    add esp, 4

ContinueLoop:
    jmp near ptr ReadLoopTop

FoundHeader:
    and ah, 1
    mov byte ptr [packed_mode], ah

    mov esi, dword ptr [rpbits_stream_ptr]
    cmp esi, dword ptr [rpbits_stream_end]
    jc  short ReadHdr1

    push ebx
    push ecx
    push edx
    call dword ptr [rpbits_stream_refill]
    pop  edx
    pop  ecx
    pop  ebx
    mov esi, dword ptr [rpbits_stream_ptr]

ReadHdr1:
    mov eax, 0
    lodsw
    mov dword ptr [rpbits_stream_ptr], esi
    mov dword ptr [g_rpbits_stream_word], eax

    mov esi, dword ptr [rpbits_stream_ptr]
    cmp esi, dword ptr [rpbits_stream_end]
    jc  short ReadHdr2

    push ebx
    push ecx
    push edx
    call dword ptr [rpbits_stream_refill]
    pop  edx
    pop  ecx
    pop  ebx
    mov esi, dword ptr [rpbits_stream_ptr]

ReadHdr2:
    mov eax, 0
    lodsw
    mov dword ptr [rpbits_stream_ptr], esi
    mov dword ptr [global_pcxw_image_width], eax

    mov esi, dword ptr [rpbits_stream_ptr]
    cmp esi, dword ptr [rpbits_stream_end]
    jc  short ReadHdr3

    push ebx
    push ecx
    push edx
    call dword ptr [rpbits_stream_refill]
    pop  edx
    pop  ecx
    pop  ebx
    mov esi, dword ptr [rpbits_stream_ptr]

ReadHdr3:
    mov eax, 0
    lodsw
    mov dword ptr [rpbits_stream_ptr], esi
    mov dword ptr [global_pcxw_image_height], eax

    call RpBits_InitStream

    pop edi
    pop esi
    pop ebx
    leave
    ret
RpBits_ReadTables ENDP

; ============================================================
; RpBits_InitStream
; ============================================================

RpBits_InitStream PROC
    mov eax, dword ptr [global_pcxw_image_width]
    or  eax, dword ptr [global_pcxw_image_height]
    jne short DoInit
    ret

DoInit:
    mov byte ptr [repeat_count], 0
    mov byte ptr [last_literal], 0

    lea eax, [decoder_stack_sentinel]
    mov dword ptr [decoder_stack_ptr], eax

    mov esi, dword ptr [rpbits_stream_ptr]
    cmp esi, dword ptr [rpbits_stream_end]
    jc  short ReadInitBits

    push ebx
    push ecx
    push edx
    call dword ptr [rpbits_stream_refill]
    pop  edx
    pop  ecx
    pop  ebx
    mov esi, dword ptr [rpbits_stream_ptr]

ReadInitBits:
    mov eax, 0
    lodsw
    mov dword ptr [rpbits_stream_ptr], esi

    cmp al, 0Bh
    jbe short BitsOK
    mov al, 0Bh

BitsOK:
    mov byte ptr [max_code_bits], al

    and eax, 0FFFFh
    mov dword ptr [bit_buffer], eax
    mov byte ptr [bit_count], 8
RpBits_InitStream ENDP

; ============================================================
; RpBits_ResetDecoder
; ============================================================

RpBits_ResetDecoder PROC
    mov byte ptr [code_bits], 9
    mov dword ptr [code_mask], 01FFh
    mov edx, 100h
    mov dword ptr [saved_edx], edx

    mov eax, 0FFFFFFFFh
    xor ebx, ebx
    mov ecx, 800h

ResetWords:
    mov word ptr [decoder_table + ebx], ax
    add ebx, 3
    loop ResetWords

    mov al, 0
    xor ebx, ebx
    mov ecx, 100h

ResetBytes:
    mov byte ptr [decoder_table + ebx + 2], al
    inc al
    add ebx, 3
    loop ResetBytes

    ret
RpBits_ResetDecoder ENDP

; ============================================================
; RpBits_DecodeSymbol
; ============================================================

RpBits_DecodeSymbol PROC
    pop ebp
    cmp esp, OFFSET decoder_stack_sentinel
    jz  short DecodeReal

ReturnTrampoline:
    pop eax
    jmp ebp

DecodeReal:
    mov ebx, dword ptr [bit_buffer]
    mov cl, 10h
    mov ch, byte ptr [bit_count]
    sub cl, ch
    shr ebx, cl
    mov cl, ch

NeedBits:
    cmp cl, byte ptr [code_bits]
    jge short HaveBits

    cmp esi, dword ptr [rpbits_stream_end]
    jc  short ReadWord

    push ebx
    push ecx
    push edx
    call dword ptr [rpbits_stream_refill]
    pop edx
    pop ecx
    pop ebx
    mov esi, dword ptr [rpbits_stream_ptr]

ReadWord:
    mov eax, 0
    lodsw
    mov dword ptr [bit_buffer], eax
    shl eax, cl
    or  ebx, eax
    add cl, 10h
    jmp short NeedBits

HaveBits:
    sub cl, byte ptr [code_bits]
    mov byte ptr [bit_count], cl

    mov eax, ebx
    and eax, dword ptr [code_mask]

    mov ecx, eax
    cmp eax, edx
    jl short WalkTree

    mov ecx, edx
    mov eax, dword ptr [last_node_index]
    mov bl, byte ptr [last_symbol]
    push ebx

WalkTree:
    mov ebx, eax
    add ebx, eax
    add ebx, eax
    mov ax, word ptr [decoder_table + ebx]
    inc ax
    jz  short Leaf
    dec ax
    mov bl, byte ptr [decoder_table + ebx + 2]
    push ebx
    jmp short WalkTree

Leaf:
    mov al, byte ptr [decoder_table + ebx + 2]
    mov byte ptr [last_symbol], al
    push eax

    mov ebx, edx
    add ebx, edx
    add ebx, edx
    mov byte ptr [decoder_table + ebx + 2], al
    mov eax, dword ptr [last_node_index]
    mov word ptr [decoder_table + ebx], ax

    inc edx
    cmp edx, dword ptr [code_mask]
    jle short MaskOK

    inc byte ptr [code_bits]
    stc
    rcl dword ptr [code_mask], 1

MaskOK:
    mov al, byte ptr [code_bits]
    cmp al, byte ptr [max_code_bits]
    jle short StoreState
    call RpBits_ResetDecoder

StoreState:
    mov dword ptr [last_node_index], ecx
    jmp ReturnTrampoline
RpBits_DecodeSymbol ENDP

; ============================================================
; RpBits_DecodeRun
; ECX = count, EDI = destination
; ============================================================

RpBits_DecodeRun PROC
    cmp byte ptr [packed_mode], 0
    jz  short StoreCount
    inc ecx
    shr ecx, 1

StoreCount:
    mov dword ptr [remaining_count], ecx
    mov edx, dword ptr [saved_edx]
    xchg dword ptr [decoder_stack_ptr], esp

DecodeLoop:
    cmp byte ptr [repeat_count], 0
    jnz short UseLast

    call RpBits_DecodeSymbol
    cmp al, 90h
    jz  short Escape

    mov byte ptr [last_literal], al
    jmp short Emit

Escape:
    call RpBits_DecodeSymbol
    or  al, al
    jnz short SetRun
    mov al, 90h
    mov byte ptr [last_literal], al
    jmp short Emit

SetRun:
    dec al
    mov byte ptr [repeat_count], al

UseLast:
    mov al, byte ptr [last_literal]
    dec byte ptr [repeat_count]

Emit:
    cmp byte ptr [packed_mode], 0
    jz  short EmitByte

    mov ah, al
    and al, 0Fh
    shr ah, 1
    shr ah, 1
    shr ah, 1
    shr ah, 1
    stosw
    dec dword ptr [remaining_count]
    jnz short DecodeLoop
    jmp short Done

EmitByte:
    stosb
    dec dword ptr [remaining_count]
    jnz short DecodeLoop

Done:
    mov dword ptr [saved_edx], edx
    xchg dword ptr [decoder_stack_ptr], esp
    ret
RpBits_DecodeRun ENDP

; ============================================================
; RpBits_DecodeImage
; C-callable wrapper
; ============================================================

RpBits_DecodeImage PROC
    push ebp
    mov  ebp, esp
    push ebx
    push esi
    push edi

    mov esi, dword ptr [rpbits_stream_ptr]
    mov edi, dword ptr [ebp+8]
    mov ecx, dword ptr [ebp+12]

    push ebp
    call RpBits_DecodeRun
    pop  ebp

    mov dword ptr [rpbits_stream_ptr], esi

    pop edi
    pop esi
    pop ebx
    leave
    ret
RpBits_DecodeImage ENDP

END
