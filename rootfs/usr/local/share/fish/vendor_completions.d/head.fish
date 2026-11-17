complete -c head -s c -l bytes -d 'print the first NUM bytes of each file; with a leading \'-\', print all but the last NUM bytes of each file' -r
complete -c head -s n -l lines -d 'print the first NUM lines instead of the first 10; with a leading \'-\', print all but the last NUM lines of each file' -r
complete -c head -s q -l quiet -l silent -d 'never print headers giving file names'
complete -c head -s v -l verbose -d 'always print headers giving file names'
complete -c head -l presume-input-pipe
complete -c head -s z -l zero-terminated -d 'line delimiter is NUL, not newline'
complete -c head -s h -l help -d 'Print help'
complete -c head -s V -l version -d 'Print version'
