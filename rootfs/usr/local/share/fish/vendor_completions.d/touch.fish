complete -c touch -s t -d 'use [[CC]YY]MMDDhhmm[.ss] instead of the current time' -r
complete -c touch -s d -l date -d 'parse argument and use it instead of current time' -r
complete -c touch -s r -l reference -d 'use this file\'s times instead of the current time' -r -F
complete -c touch -l time -d 'change only the specified time: "access", "atime", or "use" are equivalent to -a; "modify" or "mtime" are equivalent to -m' -r -f -a "atime\t''
mtime\t''"
complete -c touch -l help -d 'Print help information.'
complete -c touch -s a -d 'change only the access time'
complete -c touch -s f -d '(ignored)'
complete -c touch -s m -d 'change only the modification time'
complete -c touch -s c -l no-create -d 'do not create any files'
complete -c touch -s h -l no-dereference -d 'affect each symbolic link instead of any referenced file (only for systems that can change the timestamps of a symlink)'
complete -c touch -s V -l version -d 'Print version'
