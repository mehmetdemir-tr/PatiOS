complete -c uniq -l all-repeated -d 'print all duplicate lines. Delimiting is done with blank lines. [default: none]' -r -f -a "none\t''
prepend\t''
separate\t''"
complete -c uniq -l group -d 'show all items, separating groups with an empty line. [default: separate]' -r -f -a "separate\t''
prepend\t''
append\t''
both\t''"
complete -c uniq -s w -l check-chars -d 'compare no more than N characters in lines' -r
complete -c uniq -s s -l skip-chars -d 'avoid comparing the first N characters' -r
complete -c uniq -s f -l skip-fields -d 'avoid comparing the first N fields' -r
complete -c uniq -s D -d 'print all duplicate lines'
complete -c uniq -s c -l count -d 'prefix lines by the number of occurrences'
complete -c uniq -s i -l ignore-case -d 'ignore differences in case when comparing'
complete -c uniq -s d -l repeated -d 'only print duplicate lines'
complete -c uniq -s u -l unique -d 'only print unique lines'
complete -c uniq -s z -l zero-terminated -d 'end lines with 0 byte, not newline'
complete -c uniq -s h -l help -d 'Print help'
complete -c uniq -s V -l version -d 'Print version'
