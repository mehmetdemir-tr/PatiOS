complete -c chroot -l groups -d 'Comma-separated list of groups to switch to' -r
complete -c chroot -l userspec -d 'Colon-separated user and group to switch to.' -r
complete -c chroot -l skip-chdir -d 'Use this option to not change the working directory to / after changing the root directory to newroot, i.e., inside the chroot.'
complete -c chroot -s h -l help -d 'Print help'
complete -c chroot -s V -l version -d 'Print version'
