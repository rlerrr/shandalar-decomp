.386
.model flat, c
option casemap:none

; ============================================================
; Engine-provided externals (stream I/O)
; ============================================================

EXTERN global_pcxw_image_width:DWORD
EXTERN global_pcxw_image_height:DWORD
EXTERN rpbits_stream_ptr:DWORD        ; DAT_100f3394
EXTERN rpbits_stream_end:DWORD        ; PTR_DAT_1002252c
EXTERN rpbits_stream_refill:DWORD    ; DAT_100f35a0

; ============================================================
; Decoder-private data (library local)
; ============================================================

.data

decoder_stack_ptr      dd 0        ; DAT_10026554
decoder_stack_sentinel dd 0        ; DAT_10026973

remaining_count        dd 0        ; DAT_10026558
saved_edx              dd 0        ; DAT_10026564
bit_buffer              dd 0        ; DAT_10026568
code_mask               dd 0        ; DAT_10026560
last_node_index         dd 0        ; DAT_1002656e

repeat_count            db 0        ; DAT_1002655c
last_literal            db 0        ; DAT_1002655d
code_bits               db 0        ; DAT_1002655e
max_code_bits           db 0        ; DAT_1002655f
bit_count               db 0        ; DAT_1002656c
packed_mode             db 0        ; DAT_1002656d
last_symbol             db 0        ; DAT_10026572

align 4
decoder_table           db 2048*3 dup (0)

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
; RpBits_InitStream
; Initializes decoder state after header read
; ============================================================

RpBits_InitStream PROC
    ; if (header_width | header_height) == 0 → return
    mov eax, dword ptr [global_pcxw_image_width]      ; reuse eax temporarily
    or  eax, dword ptr [global_pcxw_image_height]
    jne  short DoInit
    ret
DoInit:

    ; reset RLE state
    mov byte ptr [repeat_count], 0
    mov byte ptr [last_literal], 0

    ; set decoder stack sentinel to current ESP
    lea eax, [decoder_stack_sentinel]
    mov dword ptr [decoder_stack_ptr], eax

    ; --- read initial code bit width ---
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

    ; clamp to max 0x0B
    cmp al, 0Bh
    jbe short BitsOK
    mov al, 0Bh

BitsOK:
    mov byte ptr [max_code_bits], al

    ; initialize bit buffer
    and eax, 0FFFFh
    mov dword ptr [bit_buffer], eax
    mov byte ptr [bit_count], 8
    ret
RpBits_InitStream ENDP

; ============================================================
; RpBits_ReadTables
; Loads decoder tables from stream
; ============================================================

RpBits_ReadTables PROC
    push ebp
    mov  ebp, esp
    push ebx
    push esi
    push edi

    xor eax, eax

ReadLoop:
    ; --- read word ---
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

    ; check for 'XM' marker
    cmp ax, 'MX'
    je  short FoundHeader

    ; store word
    mov word ptr [edi], ax

    ; --- read symbol word ---
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

    ; store symbol byte
    mov byte ptr [edi + 2], al

    add edi, 3

    ; stop when table filled
    cmp edi, offset decoder_table + (2048*3)
    jb  short ReadLoop

    call RpBits_InitStream
    jmp short Done

FoundHeader:
    ; header flag: packed (4bpp) or not
    and ah, 1
    mov byte ptr [packed_mode], ah

    call RpBits_InitStream

Done:
    pop edi
    pop esi
    pop ebx
    leave
    ret
RpBits_ReadTables ENDP

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
; FASTCALL-ish, returns symbol in AL via trampoline
; ============================================================

RpBits_DecodeSymbol PROC
    pop ebp
    cmp esp, dword ptr [decoder_stack_sentinel]
    jz  short DecodeReal

ReturnTrampoline:
    pop eax
    jmp ebp

DecodeReal:
    mov ebx, dword ptr [bit_buffer]
    mov cl, byte ptr [bit_count]

NeedBits:
    cmp cl, byte ptr [code_bits]
    jge short HaveBits

    mov esi, dword ptr [rpbits_stream_ptr]
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
    xor eax, eax
    lodsw
    mov dword ptr [rpbits_stream_ptr], esi
    shl eax, cl
    or  ebx, eax
    add cl, 16
    jmp short NeedBits

HaveBits:
    sub cl, byte ptr [code_bits]
    mov byte ptr [bit_count], cl

    mov eax, ebx
    and eax, dword ptr [code_mask]

WalkTree:
    mov ebx, eax
    lea ebx, [ebx + ebx*2]
    mov ax, word ptr [decoder_table + ebx]
    inc ax
    jz  short Leaf
    dec ax
    mov al, byte ptr [decoder_table + ebx + 2]
    push ebx
    jmp short WalkTree

Leaf:
    mov al, byte ptr [decoder_table + ebx + 2]
    mov byte ptr [last_symbol], al
    push eax

    ; adaptive update
    mov eax, dword ptr [last_node_index]
    lea ebx, [eax + eax*2]
    mov byte ptr [decoder_table + ebx + 2], al
    mov word ptr [decoder_table + ebx], ax

    inc eax
    cmp eax, dword ptr [code_mask]
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
    mov dword ptr [last_node_index], eax
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
    test al, al
    jnz short SetRun
    mov al, 90h
    mov byte ptr [last_literal], al
    jmp short Emit

SetRun:
    dec al
    mov byte ptr [repeat_count], al
    mov al, byte ptr [last_literal]
    jmp short Emit

UseLast:
    mov al, byte ptr [last_literal]
    dec byte ptr [repeat_count]

Emit:
    cmp byte ptr [packed_mode], 0
    jz  short EmitByte

    mov ah, al
    and al, 0Fh
    shr ah, 4
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
