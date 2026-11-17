complete -c tail -s c -l bytes -d 'Print the last NUM bytes; use -c +NUM to print from byte NUM onward (counting from 1)' -r
complete -c tail -s f -l follow -d 'Print the file as it grows' -r -f -a "descriptor\t''
name\t''"
complete -c tail -s n -l lines -d 'Print the last NUM lines; use -n +NUM to print from line NUM onward (counting from 1)' -r
complete -c tail -l pid -d 'With -f, terminate after process ID, PID dies' -r
complete -c tail -s s -l sleep-interval -d 'Number of seconds to sleep between polling the file when running with -f' -r
complete -c tail -l max-unchanged-stats -d 'Reopen a FILE which has not changed size after N (default 5) iterations to see if it has been unlinked or renamed (this is the usual case of rotated log files); This option is meaningful only when polling (i.e., with --use-polling) and when --follow=name' -r
complete -c tail -s q -l quiet -l silent -d 'Never output headers giving file names'
complete -c tail -s v -l verbose -d 'Always output headers giving file names'
complete -c tail -s z -l zero-terminated -d 'Line delimiter is NUL, not newline'
complete -c tail -l use-polling -d 'Disable \'inotify\' support and use polling instead'
complete -c tail -l retry -d 'Keep trying to open a file if it is inaccessible'
complete -c tail -s F -d 'Same as --follow=name --retry'
complete -c tail -l debug -d 'indicate which --follow implementation is used'
complete -c tail -l presume-input-pipe
complete -c tail -s h -l help -d 'Print help'
complete -c tail -s V -l version -d 'Print version'
