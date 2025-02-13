#!/bin/bash

export HISTCONTROL=ignoreboth:erasedups # no duplicate entries
export PROMPT_COMMAND="history -a" # update histfile after every command
export HISTSIZE=10000
export HISTIGNORE="&:[ ]*:exit"

export LANG=en_US.UTF-8
export LC_CTYPE=en_US.UTF-8

# set prompt to >
export PS1="$ "

# PATH
export PATH=$HOME/bin:$PATH

# use vi editing mode in shell
set -o vi

# bindkey
# % bind -l for a list of bindkey
# % bind -p for a list of current bind keys
bind '"\C-p": history-search-backward'
bind '"\C-n": history-search-forward'


# Use 256 color, this will make vim work right inside tmux
export TERM=xterm-256color
export COLORTERM=truecolor

# fzf
export FZF_DEFAULT_OPTS="
    --layout=reverse
    --height=40%
    --info=hidden
    --prompt='  '
    --multi
    --no-bold
    --color fg:-1,bg:-1,gutter:-1,hl:underline:33,fg+:-1,bg+:0,hl+:underline:33
    --color info:136,prompt:136,pointer:230,marker:230,spinner:136
    --bind '?:toggle-preview'
    --preview-window=:hidden
"

#  alias
alias ll='ls -lah '
alias l='ls -1pG'
alias ls='ls -1pG'
alias e='vim'
alias t='tmux a'
