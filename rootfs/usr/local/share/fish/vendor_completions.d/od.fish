complete -c od -s A -l address-radix -d 'Select the base in which file offsets are printed.' -r
complete -c od -s j -l skip-bytes -d 'Skip BYTES input bytes before formatting and writing.' -r
complete -c od -s N -l read-bytes -d 'limit dump to BYTES input bytes' -r
complete -c od -l endian -d 'byte order to use for multi-byte formats' -r -f -a "big\t''
little\t''"
complete -c od -s S -l strings -d 'output strings of at least BYTES graphic chars. 3 is assumed when BYTES is not specified.' -r
complete -c od -s t -l format -d 'select output format or formats' -r
complete -c od -s w -l width -d 'output BYTES bytes per output line. 32 is implied when BYTES is not specified.' -r
complete -c od -l help -d 'Print help information.'
complete -c od -s a -d 'named characters, ignoring high-order bit'
complete -c od -s b -d 'octal bytes'
complete -c od -s c -d 'ASCII characters or backslash escapes'
complete -c od -s d -d 'unsigned decimal 2-byte units'
complete -c od -s D -d 'unsigned decimal 4-byte units'
complete -c od -s o -d 'octal 2-byte units'
complete -c od -s I -d 'decimal 8-byte units'
complete -c od -s L -d 'decimal 8-byte units'
complete -c od -s i -d 'decimal 4-byte units'
complete -c od -s l -d 'decimal 8-byte units'
complete -c od -s x -d 'hexadecimal 2-byte units'
complete -c od -s h -d 'hexadecimal 2-byte units'
complete -c od -s O -d 'octal 4-byte units'
complete -c od -s s -d 'decimal 2-byte units'
complete -c od -s X -d 'hexadecimal 4-byte units'
complete -c od -s H -d 'hexadecimal 4-byte units'
complete -c od -s e -d 'floating point double precision (64-bit) units'
complete -c od -s f -d 'floating point single precision (32-bit) units'
complete -c od -s F -d 'floating point double precision (64-bit) units'
complete -c od -s v -l output-duplicates -d 'do not use * to mark line suppression'
complete -c od -l traditional -d 'compatibility mode with one input, offset and label.'
complete -c od -s V -l version -d 'Print version'
