# shift sh

# ={=========================================================================
# sh-shift

# *sh-shift*
# shift
# 
#     shift [n]
# 
#     Shift the positional parameters `to-the-left` by n. The positional
#     parameters from n+1 ... $# are renamed to $1 ... $#-n. Parameters
#     represented by the numbers $# to $#-n+1 are unset. n must be a non-negative
#     number less than or equal to $#. If n is zero or greater than $#, the
#     positional parameters are not changed. If n is not supplied, it is assumed
#     to be 1. The return status is zero unless n is greater than $# or less than
#     zero, non-zero otherwise.


# <ex> can use 'loop' until there is no more args to process:
# 
# while [ "$1" != "" ]; do
#    case $1 in
#       -h | -help | --help)
#          echo "help.."
#          exit
#          ;;
#       release)
#          echo "release..."
#          exit
#          ;;
#       *)
#          echo "else..."
#          exit
#          ;;
#    esac
#    shift
# done


# keitee@kit-hdebi:~/git/kb/code-sh$ sh shift.sh 1 2 3 4 5
# $1 = 1 and $# = 5
# $1 = 2 and $# = 4
# $1 = 3 and $# = 3
# $1 = 4 and $# = 2
# $1 = 5 and $# = 1
# $1 =  and $# = 0
# shift.sh: 19: shift: can't shift that many


echo "\$1 = $1 and \$# = $#"
shift
echo "\$1 = $1 and \$# = $#"
shift
echo "\$1 = $1 and \$# = $#"
shift
echo "\$1 = $1 and \$# = $#"
shift
echo "\$1 = $1 and \$# = $#"
shift
echo "\$1 = $1 and \$# = $#"
shift
