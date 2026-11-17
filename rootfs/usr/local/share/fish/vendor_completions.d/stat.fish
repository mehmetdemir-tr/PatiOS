complete -c stat -s c -l format -d 'use the specified FORMAT instead of the default; output a newline after each use of FORMAT' -r
complete -c stat -l printf -d 'like --format, but interpret backslash escapes, and do not output a mandatory trailing newline; if you want a newline, include \\n in FORMAT' -r
complete -c stat -s L -l dereference -d 'follow links'
complete -c stat -s f -l file-system -d 'display file system status instead of file status'
complete -c stat -s t -l terse -d 'print the information in terse form'
complete -c stat -s h -l help -d 'Print help'
complete -c stat -s V -l version -d 'Print version'
