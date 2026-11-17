complete -c cut -s b -l bytes -d 'extract bytes listed in LIST' -r
complete -c cut -s c -l characters -d 'extract characters listed in LIST' -r
complete -c cut -s d -l delimiter -d 'use DELIM as field separator (default: Tab)' -r
complete -c cut -s w -l whitespace-delimited -d 'use any run of blanks (Space/Tab, plus Unicode spaces in a UTF-8 locale) as delimiter; ignore leading and trailing blanks with \'trimmed\'' -r -f -a "trimmed\t''"
complete -c cut -s f -l fields -d 'extract fields listed in LIST' -r
complete -c cut -s F -d 'like -f, but merge adjacent delimiters; the delimiter defaults to whitespace and the output delimiter to a space' -r
complete -c cut -s O -l output-delimiter -d 'replace input delimiter with NEW_DELIM in output' -r
complete -c cut -l complement -d 'invert selection: print all columns except the specified ones'
complete -c cut -s s -l only-delimited -d 'suppress lines that do not contain the delimiter'
complete -c cut -s z -l zero-terminated -d 'use NULL (\\0) instead of newline as line terminator'
complete -c cut -s n -l no-partial -d 'with -b, do not output partial multi-byte characters'
complete -c cut -s h -l help -d 'Print help'
complete -c cut -s V -l version -d 'Print version'
