# function _mycomplete_()
# {
#   echo "{"
#   echo "1 = $1"
#   echo "2 = $2"
#   echo "3 = $3"
#   local cmd="${1##*/}"
#   local word=${COMP_WORDS[COMP_CWORD]}
#   local line=${COMP_LINE}
#   echo "cmd=$cmd"
#   echo "word=$word"
#   echo "line=$line"
#   echo "}"
# }

# _init_completion is a function from /usr/share/bash-completion/bash_completion
#
# Initialize completion and deal with various general things: do file
# and variable completion where appropriate, and adjust prev, words,
# and cword as if no redirections exist so that completions do not
# need to deal with them.  Before calling this function, make sure
# cur, prev, words, and cword are local, ditto split if you use -s.

# NOTE: taken from "apt" completion

function _mycomplete_()
{
    local cur prev words cword
    _init_completion || return

    # see if the user selected a command already
    local COMMANDS=(
        "list"
        "search"
        "show" "showsrc"
        "install" "remove" "purge" "autoremove"
        "update"
        "upgrade" "full-upgrade" "dist-upgrade"
        "edit-sources"
        "help"
        "source" "build-dep"
        "clean" "autoclean"
        "download" "changelog"
        "moo"
        "depends" "rdepends"
        "policy")

    # case 3
    # myfoo install <TAB>
    # $words is "myfoo install" and "install" matches so $command is "install"
    local command i
    for (( i=0; i < ${#words[@]}-1; i++ )); do
        if [[ ${COMMANDS[@]} =~ ${words[i]} ]]; then
            command=${words[i]}
            break
        fi
    done

    # supported options per command
    # case 4
    # apt install -<TAB>

    if [[ "$cur" == -* ]]; then
        case $command in
            install|remove|purge|upgrade|dist-upgrade|full-upgrade|autoremove)
                COMPREPLY=( $( compgen -W '--show-progress
                  --fix-broken --purge --verbose-versions --auto-remove
                  -s --simulate --dry-run
                  --download
                  --fix-missing
                  --fix-policy
                  --ignore-hold
                  --force-yes
                  --trivial-only
                  --reinstall --solver
                  -t --target-release'"$GENERIC_APT_GET_OPTIONS" -- "$cur" ) )
                return 0
                ;;
        esac
    fi

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
            install)
                COMPREPLY=( $( apt-cache --no-generate pkgnames "$cur" \
                    2> /dev/null ) )
                if [[ "$cur" == ./* || "$cur" == /* ]]; then
                    _filedir "deb"
                fi
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
