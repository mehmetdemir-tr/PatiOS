complete -c install -l backup -d 'make a backup of each existing destination file' -r
complete -c install -s g -l group -d 'set group ownership, instead of process\'s current group' -r
complete -c install -s m -l mode -d 'set permission mode (as in chmod), instead of rwxr-xr-x' -r
complete -c install -s o -l owner -d 'set ownership (super-user only)' -r -f -a "(__fish_complete_users)"
complete -c install -l strip-program -d 'program used to strip binaries' -r -f -a "(__fish_complete_command)"
complete -c install -s S -l suffix -d 'override the usual backup suffix' -r
complete -c install -s t -l target-directory -d 'copy all SOURCE arguments into DIRECTORY' -r -f -a "(__fish_complete_directories)"
complete -c install -l context -d 'set security context of files and directories' -r
complete -c install -s b -d 'like --backup but does not accept an argument'
complete -c install -s c -d 'ignored'
complete -c install -s C -l compare -d 'compare each pair of source and destination files, and in some cases, do not modify the destination at all'
complete -c install -s d -l directory -d 'treat all arguments as directory names. create all components of the specified directories'
complete -c install -s D -d 'create all leading components of DEST except the last, then copy SOURCE to DEST'
complete -c install -s p -l preserve-timestamps -d 'apply access/modification times of SOURCE files to corresponding destination files'
complete -c install -s s -l strip -d 'strip symbol tables'
complete -c install -s T -l no-target-directory -d 'treat DEST as a normal file'
complete -c install -s v -l verbose -d 'explain what is being done'
complete -c install -s P -l preserve-context -d 'preserve security context'
complete -c install -s Z -d 'set SELinux security context of destination file and each created directory to default type'
complete -c install -s U -l unprivileged -d 'do not require elevated privileges to change the owner, the group, or the file flags of the destination'
complete -c install -s h -l help -d 'Print help'
complete -c install -s V -l version -d 'Print version'
