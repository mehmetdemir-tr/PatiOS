complete -c env -s C -l chdir -d 'change working directory to DIR' -r -f -a "(__fish_complete_directories)"
complete -c env -s u -l unset -d 'remove variable from the environment' -r
complete -c env -s S -l split-string -d 'process and split S into separate arguments; used to pass multiple arguments on shebang lines' -r
complete -c env -s a -l argv0 -d 'Override the zeroth argument passed to the command being executed. Without this option a default value of `command` is used.' -r
complete -c env -l ignore-signal -d 'set handling of SIG signal(s) to do nothing' -r
complete -c env -l default-signal -d 'reset handling of SIG signal(s) to the default action' -r
complete -c env -l block-signal -d 'block delivery of SIG signal(s) while running COMMAND' -r
complete -c env -s i -l ignore-environment -d 'start with an empty environment'
complete -c env -s 0 -l null -d 'end each output line with a 0 byte rather than a newline (only valid when printing the environment)'
complete -c env -s v -l debug -d 'print verbose information for each processing step'
complete -c env -l list-signal-handling -d 'list signal handling changes requested by preceding options'
complete -c env -s h -l help -d 'Print help'
complete -c env -s V -l version -d 'Print version'
