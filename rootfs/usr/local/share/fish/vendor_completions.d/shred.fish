complete -c shred -s n -l iterations -d 'overwrite N times instead of the default (3)' -r
complete -c shred -s s -l size -d 'shred this many bytes (suffixes like K, M, G accepted)' -r
complete -c shred -l remove -d 'like -u but give control on HOW to delete;  See below' -r -f -a "unlink\t''
wipe\t''
wipesync\t''"
complete -c shred -l random-source -d 'take random bytes from FILE' -r -F
complete -c shred -s f -l force -d 'change permissions to allow writing if necessary'
complete -c shred -s u -d 'deallocate and remove file after overwriting'
complete -c shred -s v -l verbose -d 'show progress'
complete -c shred -s x -l exact -d 'do not round file sizes up to the next full block; this is the default for non-regular files'
complete -c shred -s z -l zero -d 'add a final overwrite with zeros to hide shredding'
complete -c shred -s h -l help -d 'Print help'
complete -c shred -s V -l version -d 'Print version'
