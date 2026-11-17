complete -c wc -l files0-from -d 'read input from the files specified by NUL-terminated names in file F; If F is - then read names from standard input' -r -F
complete -c wc -l total -d 'when to print a line with total counts; WHEN can be: auto, always, only, never' -r -f -a "auto\t''
always\t''
only\t''
never\t''"
complete -c wc -s c -l bytes -d 'print the byte counts'
complete -c wc -s m -l chars -d 'print the character counts'
complete -c wc -s l -l lines -d 'print the newline counts'
complete -c wc -s L -l max-line-length -d 'print the length of the longest line'
complete -c wc -s w -l words -d 'print the word counts'
complete -c wc -l debug
complete -c wc -s h -l help -d 'Print help'
complete -c wc -s V -l version -d 'Print version'
