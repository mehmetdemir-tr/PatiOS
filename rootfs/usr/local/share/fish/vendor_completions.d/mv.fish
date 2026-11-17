complete -c mv -l backup -d 'make a backup of each existing destination file' -r
complete -c mv -s S -l suffix -d 'override the usual backup suffix' -r
complete -c mv -l update -d 'move only when the SOURCE file is newer than the destination file or when the destination file is missing' -r -f -a "none\t''
all\t''
older\t''
none-fail\t''"
complete -c mv -s t -l target-directory -d 'move all SOURCE arguments into DIRECTORY' -r -f -a "(__fish_complete_directories)"
complete -c mv -l context -d 'like -Z, or if CTX is specified then set the SELinux security context to CTX' -r
complete -c mv -s f -l force -d 'do not prompt before overwriting'
complete -c mv -s i -l interactive -d 'prompt before overwrite'
complete -c mv -s n -l no-clobber -d 'do not overwrite an existing file'
complete -c mv -l strip-trailing-slashes -d 'remove any trailing slashes from each SOURCE argument'
complete -c mv -s b -d 'like --backup but does not accept an argument'
complete -c mv -s u -d 'like --update but does not accept an argument'
complete -c mv -s T -l no-target-directory -d 'treat DEST as a normal file'
complete -c mv -s v -l verbose -d 'explain what is being done'
complete -c mv -s g -l progress -d 'Display a progress bar. Note: this feature is not supported by GNU coreutils.'
complete -c mv -s Z -d 'set SELinux security context of destination file to default type'
complete -c mv -l debug -d 'explain how a file is copied. Implies -v'
complete -c mv -l exchange -d 'exchange source and destination (atomically swap them)'
complete -c mv -s h -l help -d 'Print help'
complete -c mv -s V -l version -d 'Print version'
