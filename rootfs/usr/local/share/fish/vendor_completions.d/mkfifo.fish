complete -c mkfifo -s m -l mode -d 'file permissions for the fifo' -r
complete -c mkfifo -l context -d 'like -Z, or if CTX is specified then set the SELinux or SMACK security context to CTX' -r
complete -c mkfifo -s Z -d 'set the SELinux security context to default type'
complete -c mkfifo -s h -l help -d 'Print help'
complete -c mkfifo -s V -l version -d 'Print version'
