_fmt() {
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
                cmd="fmt"
                ;;
            *)
                ;;
        esac
    done

    case "${cmd}" in
        fmt)
            opts="-c -t -m -s -u -p -P -x -X -w -g -q -T -h -V --crown-margin --tagged-paragraph --preserve-headers --split-only --uniform-spacing --prefix --skip-prefix --exact-prefix --exact-skip-prefix --width --goal --quick --tab-width --help --version"
            if [[ ${cur} == -* || ${COMP_CWORD} -eq 1 ]] ; then
                COMPREPLY=( $(compgen -W "${opts}" -- "${cur}") )
                return 0
            fi
            case "${prev}" in
                --prefix)
                    COMPREPLY=($(compgen -f "${cur}"))
                    return 0
                    ;;
                -p)
                    COMPREPLY=($(compgen -f "${cur}"))
                    return 0
                    ;;
                --skip-prefix)
                    COMPREPLY=($(compgen -f "${cur}"))
                    return 0
                    ;;
                -P)
                    COMPREPLY=($(compgen -f "${cur}"))
                    return 0
                    ;;
                --width)
                    COMPREPLY=($(compgen -f "${cur}"))
                    return 0
                    ;;
                -w)
                    COMPREPLY=($(compgen -f "${cur}"))
                    return 0
                    ;;
                --goal)
                    COMPREPLY=($(compgen -f "${cur}"))
                    return 0
                    ;;
                -g)
                    COMPREPLY=($(compgen -f "${cur}"))
                    return 0
                    ;;
                --tab-width)
                    COMPREPLY=($(compgen -f "${cur}"))
                    return 0
                    ;;
                -T)
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
    complete -F _fmt -o nosort -o bashdefault -o default fmt
else
    complete -F _fmt -o bashdefault -o default fmt
fi
