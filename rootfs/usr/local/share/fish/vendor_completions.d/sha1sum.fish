complete -c sha1sum -s b -l binary
complete -c sha1sum -s c -l check -d 'read checksums from the FILEs and check them'
complete -c sha1sum -s w -l warn -d 'warn about improperly formatted checksum lines'
complete -c sha1sum -l status -d 'don\'t output anything, status code shows success'
complete -c sha1sum -l quiet -d 'don\'t print OK for each successfully verified file'
complete -c sha1sum -l ignore-missing -d 'don\'t fail or report status for missing files'
complete -c sha1sum -l strict -d 'exit non-zero for improperly formatted checksum lines'
complete -c sha1sum -l tag -d 'create a BSD style checksum'
complete -c sha1sum -s t -l text
complete -c sha1sum -s z -l zero -d 'end each output line with NUL, not newline, and disable file name escaping'
complete -c sha1sum -s h -l help -d 'Print help'
complete -c sha1sum -s V -l version -d 'Print version'
