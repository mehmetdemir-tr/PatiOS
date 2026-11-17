complete -c numfmt -s d -l delimiter -d 'use X instead of whitespace for field delimiter' -r
complete -c numfmt -l field -d 'replace the numbers in these input fields; see FIELDS below' -r
complete -c numfmt -l format -d 'use printf style floating-point FORMAT; see FORMAT below for details' -r
complete -c numfmt -l from -d 'auto-scale input numbers to UNITs; see UNIT below' -r
complete -c numfmt -l from-unit -d 'specify the input unit size' -r
complete -c numfmt -l to -d 'auto-scale output numbers to UNITs; see UNIT below' -r
complete -c numfmt -l to-unit -d 'the output unit size' -r
complete -c numfmt -l padding -d 'pad the output to N characters; positive N will right-align; negative N will left-align; padding is ignored if the output is wider than N; the default is to automatically pad if a whitespace is found' -r
complete -c numfmt -l header -d 'print (without converting) the first N header lines; N defaults to 1 if not specified' -r
complete -c numfmt -l round -d 'use METHOD for rounding when scaling' -r -f -a "up\t''
down\t''
from-zero\t''
towards-zero\t''
nearest\t''"
complete -c numfmt -l suffix -d 'print SUFFIX after each formatted number, and accept inputs optionally ending with SUFFIX' -r
complete -c numfmt -l unit-separator -d 'use STRING to separate the number from any unit when printing; by default, no separator is used' -r
complete -c numfmt -l invalid -d 'set the failure mode for invalid input' -r -f -a "abort\t''
fail\t''
warn\t''
ignore\t''"
complete -c numfmt -l debug -d 'print warnings about invalid input'
complete -c numfmt -l grouping -d 'use locale-defined grouping of digits, for example 1,000,000 (which means it has no effect in the C/POSIX locale)'
complete -c numfmt -s z -l zero-terminated -d 'line delimiter is NUL, not newline'
complete -c numfmt -s h -l help -d 'Print help'
complete -c numfmt -s V -l version -d 'Print version'
