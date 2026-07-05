# Strip Stack Struct Script

This PowerShell script removes anonymous wrapper structs of the form:

```c
struct
{
    ...
} s;
```

and also strips all `s.` prefixes from usages, e.g. `s.foo` becomes `foo`.

It is designed for cases where a temporary struct is used to stabilize stack layout, but later needs to be removed cleanly.

## Usage

### Dry run

```powershell
.\Strip-StackStruct.ps1 -Root C:\src
```

### Apply changes

```powershell
.\Strip-StackStruct.ps1 -Root C:\src -Apply
```

## Notes

- Recursively processes all `.c` and `.h` files under the given root.
- Uses .NET regex balancing groups to safely match nested `{}` inside structs.
- Only removes `s.` when it is not part of a larger identifier.
- No backups are created; intended for use with version control.

## Caveats

- The regex counts `{}` even inside strings/comments, which could theoretically cause mismatches in unusual cases.
- Assumes the struct is declared exactly as `struct { ... } s;`, anonymous and named `s`.