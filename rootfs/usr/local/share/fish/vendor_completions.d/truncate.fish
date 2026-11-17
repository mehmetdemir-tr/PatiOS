complete -c truncate -s r -l reference -d 'base the size of each file on the size of RFILE' -r -F
complete -c truncate -s s -l size -d 'set or adjust the size of each file according to SIZE, which is in bytes unless --io-blocks is specified' -r
complete -c truncate -s o -l io-blocks -d 'treat SIZE as the number of I/O blocks of the file rather than bytes'
complete -c truncate -s c -l no-create -d 'do not create files that do not exist'
complete -c truncate -s h -l help -d 'Print help'
complete -c truncate -s V -l version -d 'Print version'
