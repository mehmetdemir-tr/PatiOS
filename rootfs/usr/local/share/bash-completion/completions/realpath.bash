_realpath() {
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
                cmd="realpath"
                ;;
            *)
                ;;
        esac
    done

    case "${cmd}" in
        realpath)
            opts="-q -s -z -L -P -E -e -m -h -V --quiet --no-symlinks --strip --zero --logical --physical --canonicalize --canonicalize-existing --canonicalize-missing --relative-to --relative-base --help --version"
            if [[ ${cur} == -* || ${COMP_CWORD} -eq 1 ]] ; then
                COMPREPLY=( $(compgen -W "${opts}" -- "${cur}") )
                return 0
            fi
            case "${prev}" in
                --relative-to)
                    COMPREPLY=($(compgen -f "${cur}"))
                    return 0
                    ;;
                --relative-base)
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
    complete -F _realpath -o nosort -o bashdefault -o default realpath
else
    complete -F _realpath -o bashdefault -o default realpath
fi
