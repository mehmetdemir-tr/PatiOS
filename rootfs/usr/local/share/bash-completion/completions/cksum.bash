_cksum() {
    local i cur prev opts cmd
    COMPREPLY=()
    if [[ "${BASH_VERSINFO[0]}" -ge 4 ]]; then
        cur="$2"
    else
        cur="${COMP_WORDS[COMP_CWORD]}"
    fi
    prev="$3"
    cmd=""
    opts=""

    for i in "${COMP_WORDS[@]:0:COMP_CWORD}"
    do
        case "${cmd},${i}" in
            ",$1")
                cmd="cksum"
                ;;
            *)
                ;;
        esac
    done

    case "${cmd}" in
        cksum)
            opts="-a -l -c -w -t -b -z -h -V --algorithm --untagged --tag --length --raw --check --warn --status --quiet --ignore-missing --strict --base64 --text --binary --zero --debug --help --version"
            if [[ ${cur} == -* || ${COMP_CWORD} -eq 1 ]] ; then
                COMPREPLY=( $(compgen -W "${opts}" -- "${cur}") )
                return 0
            fi
            case "${prev}" in
                --algorithm)
                    COMPREPLY=($(compgen -W "sysv bsd crc crc32b md5 sha1 sha2 sha3 blake2b sm3 sha224 sha256 sha384 sha512 blake3 shake128 shake256" -- "${cur}"))
                    return 0
                    ;;
                -a)
                    COMPREPLY=($(compgen -W "sysv bsd crc crc32b md5 sha1 sha2 sha3 blake2b sm3 sha224 sha256 sha384 sha512 blake3 shake128 shake256" -- "${cur}"))
                    return 0
                    ;;
                --length)
                    COMPREPLY=($(compgen -f "${cur}"))
                    return 0
                    ;;
                -l)
                    COMPREPLY=($(compgen -f "${cur}"))
                    return 0
                    ;;
                *)
                    COMPREPLY=()
                    ;;
            esac
            COMPREPLY=( $(compgen -W "${opts}" -- "${cur}") )
            return 0
            ;;
    esac
}

if [[ "${BASH_VERSINFO[0]}" -eq 4 && "${BASH_VERSINFO[1]}" -ge 4 || "${BASH_VERSINFO[0]}" -gt 4 ]]; then
    complete -F _cksum -o nosort -o bashdefault -o default cksum
else
    complete -F _cksum -o bashdefault -o default cksum
fi
