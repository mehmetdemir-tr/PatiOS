_date() {
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
                cmd="date"
                ;;
            *)
                ;;
        esac
    done

    case "${cmd}" in
        date)
            opts="-d -f -I -R -r -s -u -h -V --date --file --iso-8601 --resolution --rfc-email --rfc-3339 --debug --reference --set --utc --universal --help --version"
            if [[ ${cur} == -* || ${COMP_CWORD} -eq 1 ]] ; then
                COMPREPLY=( $(compgen -W "${opts}" -- "${cur}") )
                return 0
            fi
            case "${prev}" in
                --date)
                    COMPREPLY=($(compgen -f "${cur}"))
                    return 0
                    ;;
                -d)
                    COMPREPLY=($(compgen -f "${cur}"))
                    return 0
                    ;;
                --file)
                    local oldifs
                    if [ -n "${IFS+x}" ]; then
                        oldifs="$IFS"
                    fi
                    IFS=$'\n'
                    COMPREPLY=($(compgen -f "${cur}"))
                    if [ -n "${oldifs+x}" ]; then
                        IFS="$oldifs"
                    fi
                    if [[ "${BASH_VERSINFO[0]}" -ge 4 ]]; then
                        compopt -o filenames
                    fi
                    return 0
                    ;;
                -f)
                    local oldifs
                    if [ -n "${IFS+x}" ]; then
                        oldifs="$IFS"
                    fi
                    IFS=$'\n'
                    COMPREPLY=($(compgen -f "${cur}"))
                    if [ -n "${oldifs+x}" ]; then
                        IFS="$oldifs"
                    fi
                    if [[ "${BASH_VERSINFO[0]}" -ge 4 ]]; then
                        compopt -o filenames
                    fi
                    return 0
                    ;;
                --iso-8601)
                    COMPREPLY=($(compgen -W "date hours minutes seconds ns" -- "${cur}"))
                    return 0
                    ;;
                -I)
                    COMPREPLY=($(compgen -W "date hours minutes seconds ns" -- "${cur}"))
                    return 0
                    ;;
                --rfc-3339)
                    COMPREPLY=($(compgen -W "date seconds ns" -- "${cur}"))
                    return 0
                    ;;
                --reference)
                    COMPREPLY=($(compgen -f "${cur}"))
                    return 0
                    ;;
                -r)
                    COMPREPLY=($(compgen -f "${cur}"))
                    return 0
                    ;;
                --set)
                    COMPREPLY=($(compgen -f "${cur}"))
                    return 0
                    ;;
                -s)
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
    complete -F _date -o nosort -o bashdefault -o default date
else
    complete -F _date -o bashdefault -o default date
fi
