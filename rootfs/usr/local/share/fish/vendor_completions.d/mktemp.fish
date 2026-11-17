complete -c mktemp -l suffix -d 'append SUFFIX to TEMPLATE; SUFFIX must not contain a path separator. This option is implied if TEMPLATE does not end with X.' -r
complete -c mktemp -s p -d 'short form of --tmpdir' -r -f -a "(__fish_complete_directories)"
complete -c mktemp -l tmpdir -d 'interpret TEMPLATE relative to DIR; if DIR is not specified, use $TMPDIR ($TMP on windows) if set, else /tmp. With this option, TEMPLATE must not be an absolute name; unlike with -t, TEMPLATE may contain slashes, but mktemp creates only the final component' -r -f -a "(__fish_complete_directories)"
complete -c mktemp -s d -l directory -d 'Make a directory instead of a file'
complete -c mktemp -s u -l dry-run -d 'do not create anything; merely print a name (unsafe)'
complete -c mktemp -s q -l quiet -d 'Fail silently if an error occurs.'
complete -c mktemp -s t -d 'Generate a template (using the supplied prefix and TMPDIR (TMP on windows) if set) to create a filename template [deprecated]'
complete -c mktemp -s h -l help -d 'Print help'
complete -c mktemp -s V -l version -d 'Print version'
