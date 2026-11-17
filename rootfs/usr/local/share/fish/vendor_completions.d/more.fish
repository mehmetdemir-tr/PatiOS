complete -c more -s n -l lines -d 'The number of lines per screen full' -r
complete -c more -l number -d 'Same as --lines option argument' -r
complete -c more -s F -l from-line -d 'Start displaying each file at line number' -r
complete -c more -s P -l pattern -d 'The string to be searched in each file before starting to display it' -r
complete -c more -s d -l silent -d 'Display help instead of ringing bell when an illegal key is pressed'
complete -c more -s f -l logical -d 'Count logical lines, rather than screen lines'
complete -c more -s e -l exit-on-eof -d 'Exit on End-Of-File'
complete -c more -s l -l no-pause -d 'Do not pause after any line containing a ^L (form feed)'
complete -c more -s p -l print-over -d 'Do not scroll, clear screen and display text'
complete -c more -s c -l clean-print -d 'Do not scroll, display text and clean line ends'
complete -c more -s s -l squeeze -d 'Squeeze multiple blank lines into one'
complete -c more -s u -l plain -d 'Suppress underlining'
complete -c more -s h -l help -d 'Print help'
complete -c more -s V -l version -d 'Print version'
