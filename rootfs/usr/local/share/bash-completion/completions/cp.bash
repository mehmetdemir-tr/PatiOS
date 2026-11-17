_cp() {
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
                cmd="cp"
                ;;
            *)
                ;;
        esac
    done

    case "${cmd}" in
        cp)
            opts="-t -T -i -l -n -r -R -v -s -f -b -S -u -p -P -L -H -a -d -x -Z -g -h -V --target-directory --no-target-directory --interactive --link --no-clobber --recursive --strip-trailing-slashes --debug --verbose --symbolic-link --force --remove-destination --backup --suffix --update --reflink --attributes-only --preserve --preserve-default-attributes --no-preserve --parents --no-dereference --dereference --archive --one-file-system --sparse --context --progress --copy-contents --help --version"
            if [[ ${cur} == -* || ${COMP_CWORD} -eq 1 ]] ; then
                COMPREPLY=( $(compgen -W "${opts}" -- "${cur}") )
                return 0
            fi
            case "${prev}" in
                --target-directory)
                    COMPREPLY=()
                    if [[ "${BASH_VERSINFO[0]}" -ge 4 ]]; then
                        compopt -o plusdirs
                    fi
                    return 0
                    ;;
                -t)
                    COMPREPLY=()
                    if [[ "${BASH_VERSINFO[0]}" -ge 4 ]]; then
                        compopt -o plusdirs
                    fi
                    return 0
                    ;;
                --backup)
                    COMPREPLY=($(compgen -f "${cur}"))
                    return 0
                    ;;
                --suffix)
                    COMPREPLY=($(compgen -f "${cur}"))
                    return 0
                    ;;
                -S)
                    COMPREPLY=($(compgen -f "${cur}"))
                    return 0
                    ;;
                --update)
                    COMPREPLY=($(compgen -W "none all older none-fail" -- "${cur}"))
                    return 0
                    ;;
                --reflink)
                    COMPREPLY=($(compgen -W "auto always never" -- "${cur}"))
                    return 0
                    ;;
                --preserve)
                    COMPREPLY=($(compgen -W "mode ownership timestamps context links xattr all" -- "${cur}"))
                    return 0
                    ;;
                --no-preserve)
                    COMPREPLY=($(compgen -W "mode ownership timestamps context links xattr all" -- "${cur}"))
                    return 0
                    ;;
                --sparse)
                    COMPREPLY=($(compgen -W "never auto always" -- "${cur}"))
                    return 0
                    ;;
                --context)
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
    complete -F _cp -o nosort -o bashdefault -o default cp
else
    complete -F _cp -o bashdefault -o default cp
fi
