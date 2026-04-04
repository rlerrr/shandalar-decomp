## Interesting codegen examples

`x % 1024`

Generates:

```asm
cdq
xor eax, edx
sub eax, edx
and eax, 0x3ff
xor eax, edx
sub eax, edx 
```

See very strange function @ DECKDLL `0x10010a72` and `0x10020636`.  MSVC rarely uses `esi` but does here via inling assignment

```asm
call find_artist_name_idx (FUNCTION)
add esp, 8
cdq
mov dword ptr [ebp - 0xc], eax
mov dword ptr [ebp - 8], edx
mov ebx, global_filter_expansions (DATA)
add ebx, 0x38
mov esi, dword ptr [ebx] 
mov cl, byte ptr [ebp - 0xc]
mov eax, 1 
xor edx, edx 
call __allshl (FUNCTION) 
and esi, eax 
test dword ptr [ebx + 4], edx
```

Interesting match @ dlgproc_FilterCreatureList DECKDLL 0x10020c7a

`global_filters.creature_list[a] |= 1U << (selected[b] % 32)`

Generates:

```asm
mov ebx, 1
mov eax, dword ptr [ebp - 0x498]
mov eax, dword ptr [ebp + eax*4 - 0x494]
cdq
xor eax, edx
sub eax, edx
and eax, 0x1f
xor eax, edx
sub eax, edx
mov cl, al
shl ebx, cl
mov eax, dword ptr [ebp - 0x49c]
or dword ptr [eax*4 + global_filters+12 (OFFSET)], ebx
```

`((unsigned int)x >= 1) ? 8 : 0`

Generates:

```asm
cmp dword ptr [x (DATA)], 1
mov eax, 0
adc eax, -1
and eax, 8
```