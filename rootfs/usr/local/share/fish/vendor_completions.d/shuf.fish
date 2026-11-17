complete -c shuf -s i -l input-range -d 'treat each number LO through HI as an input line' -r
complete -c shuf -s n -l head-count -d 'output at most COUNT lines' -r
complete -c shuf -s o -l output -d 'write result to FILE instead of standard output' -r -F
complete -c shuf -l random-seed -d 'seed with STRING for reproducible output' -r -f
complete -c shuf -l random-source -d 'get random bytes from FILE' -r -F
complete -c shuf -s e -l echo -d 'treat each ARG as an input line'
complete -c shuf -s r -l repeat -d 'output lines can be repeated'
complete -c shuf -s z -l zero-terminated -d 'line delimiter is NUL, not newline'
complete -c shuf -s h -l help -d 'Print help'
complete -c shuf -s V -l version -d 'Print version'
