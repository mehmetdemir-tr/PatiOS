complete -c nl -s b -l body-numbering -d 'use STYLE for numbering body lines' -r
complete -c nl -s d -l section-delimiter -d 'use CC for separating logical pages' -r
complete -c nl -s f -l footer-numbering -d 'use STYLE for numbering footer lines' -r
complete -c nl -s h -l header-numbering -d 'use STYLE for numbering header lines' -r
complete -c nl -s i -l line-increment -d 'line number increment at each line' -r
complete -c nl -s l -l join-blank-lines -d 'group of NUMBER empty lines counted as one' -r
complete -c nl -s n -l number-format -d 'insert line numbers according to FORMAT' -r -f -a "ln\t''
rn\t''
rz\t''"
complete -c nl -s s -l number-separator -d 'add STRING after (possible) line number' -r
complete -c nl -s v -l starting-line-number -d 'first line number on each logical page' -r
complete -c nl -s w -l number-width -d 'use NUMBER columns for line numbers' -r
complete -c nl -l help -d 'Print help information.'
complete -c nl -s p -l no-renumber -d 'do not reset line numbers at logical pages'
complete -c nl -s V -l version -d 'Print version'
