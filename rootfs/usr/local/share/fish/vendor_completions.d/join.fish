complete -c join -s a -d 'also print unpairable lines from file FILENUM, where FILENUM is 1 or 2, corresponding to FILE1 or FILE2' -r
complete -c join -s v -d 'like -a FILENUM, but suppress joined output lines' -r
complete -c join -s e -d 'replace missing input fields with EMPTY' -r
complete -c join -s j -d 'equivalent to \'-1 FIELD -2 FIELD\'' -r
complete -c join -s o -d 'obey FORMAT while constructing output line' -r
complete -c join -s t -d 'use CHAR as input and output field separator' -r
complete -c join -s 1 -d 'join on this FIELD of file 1' -r
complete -c join -s 2 -d 'join on this FIELD of file 2' -r
complete -c join -s i -l ignore-case -d 'ignore differences in case when comparing fields'
complete -c join -s n -l numeric-sort -d 'compare join fields by numerical value, as \'sort -n\' does'
complete -c join -l check-order -d 'check that the input is correctly sorted, even if all input lines are pairable'
complete -c join -l nocheck-order -d 'do not check that the input is correctly sorted'
complete -c join -l header -d 'treat the first line in each file as field headers, print them without trying to pair them'
complete -c join -s z -l zero-terminated -d 'line delimiter is NUL, not newline'
complete -c join -s h -l help -d 'Print help'
complete -c join -s V -l version -d 'Print version'
