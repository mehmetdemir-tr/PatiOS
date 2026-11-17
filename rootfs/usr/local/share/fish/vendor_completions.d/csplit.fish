complete -c csplit -s b -l suffix-format -d 'use sprintf FORMAT instead of %02d' -r
complete -c csplit -s f -l prefix -d 'use PREFIX instead of \'xx\'' -r
complete -c csplit -s n -l digits -d 'use specified number of digits instead of 2' -r
complete -c csplit -s k -l keep-files -d 'do not remove output files on errors'
complete -c csplit -l suppress-matched -d 'suppress the lines matching PATTERN'
complete -c csplit -s q -s s -l quiet -l silent -d 'do not print counts of output file sizes'
complete -c csplit -s z -l elide-empty-files -d 'remove empty output files'
complete -c csplit -s h -l help -d 'Print help'
complete -c csplit -s V -l version -d 'Print version'
