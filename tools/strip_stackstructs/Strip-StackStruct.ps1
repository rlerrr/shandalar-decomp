param(
    [Parameter(Mandatory = $true)]
    [string] $Root,

    [switch] $Apply
)

$sPrefixPattern = '(?<![A-Za-z0-9_%])s\.'

function Remove-WrapperStructs {
    param([string] $Text)

    $pattern = [regex]'(?m)^[ \t]*struct[ \t]*(?:\r?\n[ \t]*)?\{'
    $out = New-Object System.Text.StringBuilder
    $pos = 0

    foreach ($m in $pattern.Matches($Text)) {
        if ($m.Index -lt $pos) {
            continue
        }

        $bodyStart = $m.Index + $m.Length
        $i = $bodyStart
        $depth = 1

        while ($i -lt $Text.Length -and $depth -gt 0) {
            $ch = $Text[$i]

            if ($ch -eq '{') {
                $depth++
            }
            elseif ($ch -eq '}') {
                $depth--
            }

            $i++
        }

        if ($depth -ne 0) {
            continue
        }

        $bodyEnd = $i - 1

        $tail = $Text.Substring($i)
        $tailMatch = [regex]::Match($tail, '^[ \t]*s[ \t]*;[ \t]*(?:\r?\n)?')

        if (-not $tailMatch.Success) {
            continue
        }

        [void]$out.Append($Text.Substring($pos, $m.Index - $pos))

        $body = $Text.Substring($bodyStart, $bodyEnd - $bodyStart)

        # Trim one leading newline after `{`
        $body = [regex]::Replace($body, '^\r?\n', '')

        [void]$out.Append($body)

        $pos = $i + $tailMatch.Length
    }

    [void]$out.Append($Text.Substring($pos))
    return $out.ToString()
}

$files = Get-ChildItem -Path $Root -Recurse -File -Include *.c,*.h

foreach ($file in $files) {
    $original = Get-Content -LiteralPath $file.FullName -Raw

    $updated = Remove-WrapperStructs $original
    $updated = [regex]::Replace($updated, $sPrefixPattern, '')

    if ($updated -ne $original) {
        Write-Host "Changed: $($file.FullName)"

        if ($Apply) {
            Set-Content -LiteralPath $file.FullName -Value $updated -NoNewline
        }
    }
}