complete -c rm -l interactive -d 'prompt according to WHEN: never, once (-I), or always (-i). Without WHEN, prompts always' -r -f -a "always\t''
once\t''
never\t''"
complete -c rm -l preserve-root -d 'do not remove \'/\' (default); with \'all\', reject any command line argument on a separate device from its parent' -r -f -a "all\t''"
complete -c rm -s f -l force -d 'ignore nonexistent files and arguments, never prompt'
complete -c rm -s i -d 'prompt before every removal'
complete -c rm -s I -d 'prompt once before removing more than three files, or when removing recursively. Less intrusive than -i, while still giving some protection against most mistakes'
complete -c rm -l one-file-system -d 'when removing a hierarchy recursively, skip any directory that is on a file system different from that of the corresponding command line argument (NOT IMPLEMENTED)'
complete -c rm -l no-preserve-root -d 'do not treat \'/\' specially'
complete -c rm -s r -s R -l recursive -d 'remove directories and their contents recursively'
complete -c rm -s d -l dir -d 'remove empty directories'
complete -c rm -s v -l verbose -d 'explain what is being done'
complete -c rm -s g -l progress -d 'display a progress bar. Note: this feature is not supported by GNU coreutils.'
complete -c rm -l presume-input-tty
complete -c rm -s h -l help -d 'Print help'
complete -c rm -s V -l version -d 'Print version'
