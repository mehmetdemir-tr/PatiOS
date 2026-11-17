complete -c chgrp -l reference -d 'use RFILE\'s group rather than specifying GROUP values' -r -F
complete -c chgrp -l from -d 'change the group only if its current group matches GROUP' -r
complete -c chgrp -l help -d 'Print help information.'
complete -c chgrp -s c -l changes -d 'like verbose but report only when a change is made'
complete -c chgrp -s f -l silent
complete -c chgrp -l quiet -d 'suppress most error messages'
complete -c chgrp -s v -l verbose -d 'output a diagnostic for every file processed'
complete -c chgrp -l preserve-root -d 'fail to operate recursively on \'/\''
complete -c chgrp -l no-preserve-root -d 'do not treat \'/\' specially (the default)'
complete -c chgrp -s R -l recursive -d 'operate on files and directories recursively'
complete -c chgrp -s H -d 'if a command line argument is a symbolic link to a directory, traverse it'
complete -c chgrp -s L -d 'traverse every symbolic link to a directory encountered'
complete -c chgrp -s P -d 'do not traverse any symbolic links (default)'
complete -c chgrp -l dereference -d 'affect the referent of each symbolic link (this is the default), rather than the symbolic link itself'
complete -c chgrp -s h -l no-dereference -d 'affect symbolic links instead of any referenced file (useful only on systems that can change the ownership of a symlink)'
complete -c chgrp -s V -l version -d 'Print version'
