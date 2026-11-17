_numfmt() {
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
                cmd="numfmt"
                ;;
            *)
                ;;
        esac
    done

    case "${cmd}" in
        numfmt)
            opts="-d -z -h -V --debug --grouping --delimiter --field --format --from --from-unit --to --to-unit --padding --header --round --suffix --unit-separator --invalid --zero-terminated --help --version"
            if [[ ${cur} == -* || ${COMP_CWORD} -eq 1 ]] ; then
                COMPREPLY=( $(compgen -W "${opts}" -- "${cur}") )
                return 0
            fi
            case "${prev}" in
                --delimiter)
                    COMPREPLY=($(compgen -f "${cur}"))
                    return 0
                    ;;
                -d)
                    COMPREPLY=($(compgen -f "${cur}"))
                    return 0
                    ;;
                --field)
                    COMPREPLY=($(compgen -f "${cur}"))
                    return 0
                    ;;
                --format)
                    COMPREPLY=($(compgen -f "${cur}"))
                    return 0
                    ;;
                --from)
                    COMPREPLY=($(compgen -f "${cur}"))
                    return 0
                    ;;
                --from-unit)
                    COMPREPLY=($(compgen -f "${cur}"))
                    return 0
                    ;;
                --to)
                    COMPREPLY=($(compgen -f "${cur}"))
                    return 0
                    ;;
                --to-unit)
                    COMPREPLY=($(compgen -f "${cur}"))
                    return 0
                    ;;
                --padding)
                    COMPREPLY=($(compgen -f "${cur}"))
                    return 0
                    ;;
                --header)
                    COMPREPLY=($(compgen -f "${cur}"))
                    return 0
                    ;;
                --round)
                    COMPREPLY=($(compgen -W "up down from-zero towards-zero nearest" -- "${cur}"))
                    return 0
                    ;;
                --suffix)
                    COMPREPLY=($(compgen -f "${cur}"))
                    return 0
                    ;;
                --unit-separator)
                    COMPREPLY=($(compgen -f "${cur}"))
                    return 0
                    ;;
                --invalid)
                    COMPREPLY=($(compgen -W "abort fail warn ignore" -- "${cur}"))
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
    complete -F _numfmt -o nosort -o bashdefault -o default numfmt
else
    complete -F _numfmt -o bashdefault -o default numfmt
fi
