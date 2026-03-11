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

See very strange function @ DECKDLL `0x10010a72`.  MSVC rarely uses `esi` but does here via inling assignment

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