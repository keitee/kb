# _init_completion is a function from /usr/share/bash-completion/bash_completion
#
# Initialize completion and deal with various general things: do file
# and variable completion where appropriate, and adjust prev, words,
# and cword as if no redirections exist so that completions do not
# need to deal with them.  Before calling this function, make sure
# cur, prev, words, and cword are local, ditto split if you use -s.

function _mycomplete_()
{
    # local cur prev words cword
    # _init_completion || return

    # use COMP_WORDS instead for embedded since "bash_completion" script shows
    # error and do not work well on
    # GNU bash, version 3.2.57(1)-release (arm-rdk-linux-gnueabi)

    local cur=${COMP_WORDS[COMP_CWORD]}

    # see if the user selected a command already
    local SERVICES=(
        "sky-asproxy"
        "sky-asplayer"
        "mediarite.service"
        "/usr/sbin/asproxy"
        "/usr/sbin/asrdkplayer")

    # see if the user selected a command already
    local COMMANDS=(
        "status"
        "start"
        "stop"
        "/opt/asproxy"
        "/opt/asrdkplayer")

    # case 3
    # myfoo install <TAB>
    # $words is "myfoo install" and "install" matches so $command is "install"
    local command i
    for (( i=0; i < ${#COMP_WORDS[@]}-1; i++ )); do
        if [[ ${COMMANDS[@]} =~ ${COMP_WORDS[i]} ]]; then
            command=${COMP_WORDS[i]}
            break
        fi
    done

    # specific command arguments
    #
    # case 3
    # apt install <TAB> or
    # apt install slack<TAB>
    #
    # to show up possible options from apt cache
    #
    # $ apt install slack
    # slack          slack-desktop
    # 
    # $ apt-cache --no-generate pkgnames slack
    # slack-desktop
    # slack

    if [[ -n $command ]]; then
        case $command in
            status|stop|start|/opt/*)
              COMPREPLY=( $( compgen -W '${SERVICES[@]}' -- "$cur" ) )
                return 0
                ;;
        esac
    fi

    # no command yet, show what commands we have
    #
    # case 1
    # myfoo <TAB>
    # $comamnd and $cur are null and shows all available options
    #
    # case 2
    # myfoo inst<TAB>
    # $command is null but $cur is "inst" and shows only match which is
    # "install"
    if [ "$command" = "" ]; then
        COMPREPLY=( $( compgen -W '${COMMANDS[@]}' -- "$cur" ) )
    fi

    return 0
}

# systemctl status sky-asproxy
# systemctl stop sky-asproxy
# systemctl start sky-asproxy
#
# systemctl status sky-asplayer
# systemctl start sky-asplayer
# systemctl stop sky-asplayer
#
# systemctl stop mediarite.service
# systemctl start mediarite.service
# systemctl status mediarite.service

# mount-copybind /opt/asrdkplayer /usr/sbin/asrdkplayer
# mount-copybind /opt/asproxy /usr/sbin/asproxy

complete -F _mycomplete_ systemctl
complete -F _mycomplete_ mount-copybind

alias a="alias"

# proxy
alias pxst="systemctl status sky-asproxy"
alias pxstop="systemctl stop sky-asproxy"
alias pxstart="systemctl start sky-asproxy"
alias pxmm="mount-copybind /opt/asproxy /usr/sbin/asproxy"
alias pxum="umount /usr/sbin/asproxy"
alias pxlog="journalctl -fu sky-asproxy"

# player
alias plst="systemctl status sky-asplayer"
alias plstop="systemctl stop sky-asplayer"
alias plstart="systemctl start sky-asplayer"
alias plmm="mount-copybind /opt/asrdkplayer /usr/sbin/asrdkplayer"
alias plum="umount /usr/sbin/asrdkplayer"
alias pllog="journalctl -fu sky-asplayer"

# system
alias syst="systemctl status sky-assystem"
alias systop="systemctl stop sky-assystem"
alias systart="systemctl start sky-assystem"
alias symm="mount-copybind /opt/ASSystemService /usr/bin/ASSystemService"
alias syum="umount /usr/bin/ASSystemService"
alias sylog="journalctl -fu sky-assystem"

# mr
alias mrst="systemctl status mediarite.service"
alias mrstop="systemctl stop mediarite.service"
alias mrstart="systemctl start mediarite.service"
alias mrlog="journalctl -fu mediarite.service"

# epg
alias est="systemctl status sky-epg.service"
alias estop="systemctl stop sky-epg.service"
alias estart="systemctl start sky-epg.service"
alias elog="journalctl -fu sky-epg.service"

#
alias sl="systemctl list-units 'sky*'"


