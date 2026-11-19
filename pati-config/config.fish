# PatiOS shell defaults — installed as /etc/fish/config.fish
# ANSI escapes are written directly so colors do not depend on fish's
# terminal-capability detection.

set -gx TERM xterm-256color
set -gx HOME /root
set -gx PAGER cat

# --- ls / grep colors -------------------------------------------------------
set -gx LS_COLORS 'rs=0:di=01;34:ln=01;36:mh=00:pi=40;33:so=01;35:do=01;35:bd=40;33;01:cd=40;33;01:or=40;31;01:su=37;41:sg=30;43:ca=30;41:tw=30;42:ow=34;42:st=37;44:ex=01;32:*.tar=01;31:*.tgz=01;31:*.gz=01;31:*.xz=01;31:*.zip=01;31:*.sh=01;32:*.c=01;36:*.h=01;36:*.md=00;33'
alias ls 'ls --color=auto'
alias ll 'ls -lh --color=auto'
alias grep 'grep --color=auto'

# --- prompt -----------------------------------------------------------------
function fish_prompt
    set -l st $status
    if test $st -ne 0
        printf '\033[1;31m[%d]\033[0m ' $st
    end
    printf '\033[1;36mpati\033[0m \033[1;34m%s\033[0m > ' (prompt_pwd)
end

function fish_greeting
end
