complete -c mkdir -s m -l mode -d 'set file mode (not implemented on windows)' -r
complete -c mkdir -l context -d 'like -Z, or if CTX is specified then set the SELinux or SMACK security context to CTX' -r
complete -c mkdir -s p -l parents -d 'make parent directories as needed'
complete -c mkdir -s v -l verbose -d 'print a message for each created directory'
complete -c mkdir -s Z -d 'set SELinux security context of each created directory to the default type'
complete -c mkdir -s h -l help -d 'Print help'
complete -c mkdir -s V -l version -d 'Print version'
