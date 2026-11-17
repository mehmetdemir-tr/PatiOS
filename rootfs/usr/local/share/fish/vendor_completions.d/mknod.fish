complete -c mknod -s m -l mode -d 'set file permission bits to MODE, not a=rw - umask' -r
complete -c mknod -l context -d 'like -Z, or if CTX is specified then set the SELinux or SMACK security context to CTX' -r
complete -c mknod -s Z -d 'set SELinux security context of the created file to the default type'
complete -c mknod -s h -l help -d 'Print help'
complete -c mknod -s V -l version -d 'Print version'
