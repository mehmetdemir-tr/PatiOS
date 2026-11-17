complete -c ptx -s F -l flag-truncation -d 'use STRING for flagging line truncations' -r
complete -c ptx -s M -l macro-name -d 'macro name to use instead of \'xx\'' -r
complete -c ptx -l format -r -f -a "roff\t''
tex\t''"
complete -c ptx -s S -l sentence-regexp -d 'for end of lines or end of sentences' -r
complete -c ptx -s W -l word-regexp -d 'use REGEXP to match each keyword' -r
complete -c ptx -s b -l break-file -d 'word break characters in this FILE' -r -F
complete -c ptx -s g -l gap-size -d 'gap size in columns between output fields' -r
complete -c ptx -s i -l ignore-file -d 'read ignore word list from FILE' -r -F
complete -c ptx -s o -l only-file -d 'read only word list from this FILE' -r -F
complete -c ptx -s w -l width -d 'output width in columns, reference excluded' -r
complete -c ptx -s A -l auto-reference -d 'output automatically generated references'
complete -c ptx -s G -l traditional -d 'behave more like System V \'ptx\''
complete -c ptx -s O -d 'generate output as roff directives'
complete -c ptx -s T -d 'generate output as TeX directives'
complete -c ptx -s R -l right-side-refs -d 'put references at right, not counted in -w'
complete -c ptx -s f -l ignore-case -d 'fold lower case to upper case for sorting'
complete -c ptx -s r -l references -d 'first field of each line is a reference'
complete -c ptx -s t -l typeset-mode -d 'change the default width from 72 to 100'
complete -c ptx -s h -l help -d 'Print help'
complete -c ptx -s V -l version -d 'Print version'
