complete -c sha256sum -s b -l binary
complete -c sha256sum -s c -l check -d 'read checksums from the FILEs and check them'
complete -c sha256sum -s w -l warn -d 'warn about improperly formatted checksum lines'
complete -c sha256sum -l status -d 'don\'t output anything, status code shows success'
complete -c sha256sum -l quiet -d 'don\'t print OK for each successfully verified file'
complete -c sha256sum -l ignore-missing -d 'don\'t fail or report status for missing files'
complete -c sha256sum -l strict -d 'exit non-zero for improperly formatted checksum lines'
complete -c sha256sum -l tag -d 'create a BSD style checksum'
complete -c sha256sum -s t -l text
complete -c sha256sum -s z -l zero -d 'end each output line with NUL, not newline, and disable file name escaping'
complete -c sha256sum -s h -l help -d 'Print help'
complete -c sha256sum -s V -l version -d 'Print version'
