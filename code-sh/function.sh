# function.sh

# ={=========================================================================
echo ""
echo "={====================================================================="
echo "sh-command"

current=`pwd`
echo "ls"
ls
echo "cd"
cd
echo "pwd"
pwd
echo "ls"
ls
echo "cd $current"
cd $current
echo "ls"
ls

current=`pwd`; echo "ls"; ls; echo "cd"; cd; echo "pwd"; pwd
echo "ls"; ls; echo "cd $current"; cd $current; echo "ls"; ls


# ={=========================================================================
echo ""
echo "={====================================================================="
echo "sh-variable"

v1=
v2="string is set"
v3="string is set"

# when use single [

if [ -n "$v1" ]; then
  echo "v1 is non-zero"
else
  echo "v1 is zero"
fi

if [ -n $v1 ]; then
  echo "v1 is non-zero"
else
  echo "v1 is zero"
fi

if [ -z "$v1" ]; then
  echo "v1 is zero"
else
  echo "v1 is non-zero"
fi

if [ -z $v1 ]; then
  echo "v1 is zero"
else
  echo "v1 is non-zero"
fi



if [ -n "$v4" ]; then
  echo "v4 is non-zero"
else
  echo "v4 is zero"
fi

if [ -n $v4 ]; then
  echo "v4 is non-zero"
else
  echo "v4 is zero"
fi

if [ -z "$v4" ]; then
  echo "v4 is zero"
else
  echo "v4 is non-zero"
fi

if [ -z $v4 ]; then
  echo "v4 is zero"
else
  echo "v4 is non-zero"
fi

# when use double [

if [[ -n "$v1" ]]; then
  echo "v1 is non-zero"
else
  echo "v1 is zero"
fi

if [[ -n $v1 ]]; then
  echo "v1 is non-zero"
else
  echo "v1 is zero"
fi

if [[ -z "$v1" ]]; then
  echo "v1 is zero"
else
  echo "v1 is non-zero"
fi

if [[ -z $v1 ]]; then
  echo "v1 is zero"
else
  echo "v1 is non-zero"
fi



if [[ -n "$v4" ]]; then
  echo "v4 is non-zero"
else
  echo "v4 is zero"
fi

if [[ -n $v4 ]]; then
  echo "v4 is non-zero"
else
  echo "v4 is zero"
fi

if [[ -z "$v4" ]]; then
  echo "v4 is zero"
else
  echo "v4 is non-zero"
fi

if [[ -z $v4 ]]; then
  echo "v4 is zero"
else
  echo "v4 is non-zero"
fi

# -z, true if the length of string is zero
# -n, true if the length of string is non-zero
[[ -z $v1 || -n $v2 ]] || echo "echo 1 is executed"

[[ -n $v1 || -z $v2 ]] || echo "echo 2 is executed"


# ={=========================================================================
echo ""
echo "={====================================================================="
echo "sh-variable-expansion"


# '${PARAMETER/PATTERN/STRING}'
# 
#     If pattern begins with ‘/’, all matches of pattern are replaced with
#     string. Normally only the first match is replaced. 

function sh_variable_expansion_1
{
  local words="07500558088 is my number"

  # [^..] where `^` means not
  # cmd="${COMP_WORDS[0]//[^A-Za-z0-9_=]/_}"
  cmd="${words//[^A-Za-z0-9_=]/_}"

  # 07500558088_is_my_number
  echo $cmd
}

sh_variable_expansion_1


# useful to set `default` value:
#
# Use 'default' to make sure that the prompt is always set correctly.
# 
# PS1=${HOST:-localhost}"$ " ; export PS1 ;


function sh_variable_expansion_2
{
  local defined="original"
  local words="substitued"
  local result=${defined:-words}

  # "words is original" when defined is set
  # "words is words" when defined is not set
  echo "words is $result"
}

sh_variable_expansion_2


# ={=========================================================================
echo ""
echo "={====================================================================="
echo "sh-printf"

# 4.2 Bash Builtin Commands
# 
# printf
# 
#     printf [-v var] format [arguments]
# 
#     Write the formatted `arguments` to the standard output under the control of
#     the `format`. The -v option causes the output to be assigned to the variable
#     var rather than being printed to the standard output.
# 
#     The format is a character string which contains three types of objects:
#     plain characters, which are simply copied to standard output, character
#     escape sequences, which are converted and copied to the standard output, and
#     format specifications, each of which causes printing of the next successive
#     argument. In addition to the standard `printf(1)` formats, printf interprets
#     the following extensions:
# 
#     %b
#         Causes printf to expand backslash escape sequences in the corresponding
#         argument, except that ‘\c’ terminates output, backslashes in ‘\'’, ‘\"’,
#         and ‘\?’ are not removed, and octal escapes beginning with ‘\0’ may
#         contain up to four digits. 
# 
#     %q
#         Causes printf to output the corresponding argument in a format that can
#         be reused as shell input. 
# 
#     %(datefmt)T
#         Causes printf to output the date-time string resulting from using
#         datefmt as a format string for strftime(3). The corresponding argument
#         is an integer representing the number of seconds since the epoch. Two
#         special argument values may be used: -1 represents the current time, and
#         -2 represents the time the shell was invoked. If no argument is
#         specified, conversion behaves as if -1 had been given. This is an
#         exception to the usual printf behavior. 
# 
#     Arguments to non-string format specifiers are treated as C language
#     constants, except that a leading plus or minus sign is allowed, and if the
#     leading character is a single or double quote, the value is the ASCII value
#     of the following character.
# 
#     The format is reused as necessary to consume all of the arguments. If the
#     format requires more arguments than are supplied, the extra format
#     specifications behave as if a zero value or null string, as appropriate, had
#     been supplied. The return value is zero on success, non-zero on failure.

printf '\n\033[0;32m%s\n' "bash printf"


# ={=========================================================================
echo ""
echo "={====================================================================="
echo "sh-eval"

# 'eval'
#           eval [ARGUMENTS]
# 
#      The arguments are concatenated together into a single command,
#      which is then read and executed, and its exit status returned as
#      the exit status of 'eval'.  If there are no arguments or only empty
#      arguments, the return status is zero.

foo=10 x=foo

# escape $
# y='$'$x   # same as
y=\$$x

# will be "$foo"
echo $y

# unlike `echo`, eval evaluate or execute "z=$foo" and it will be z=10
# eval z='$'$x  # same as
eval z=\$$x

# will be 10
echo $z

c="echo" a1="Hello, " a2="world!"
eval $c $a1 $a2


# ={=========================================================================
echo ""
echo "={====================================================================="
echo "sh-function"

myfunction1 ()
{
  echo "this is myfunction1"
  echo "num of args is $#"
  return 0
};

myfunction2 ()
{
  echo "this is myfunction2"; echo "num of args is $#"; return 0;
};

myfunction1
myfunction1 1 2 3 4

myfunction2
myfunction2 1 2 3 4

# [[ declare -f myfunction1 ]] && (echo "declare -f returns true")

if declare -f myfunction1 > /dev/null; then
  echo "declare -f returns true"
fi

if declare -f myfunction20 > /dev/null; then
  echo "declare -f myfunction20 returns true"
else
  echo "declare -f myfunction20 returns false"
fi


# myfunction="
#   function '__myfunction'
#   {
#     ((COMP_CWORD+=$arg_count))
#     COMP_WORDS=( "$@" \${COMP_WORDS[@]:1} )
#     "$comp_function_name"
#     return 0
#   }"
# 
# echo $myfunction
# # function '__myfunction' { ((COMP_CWORD+=)) COMP_WORDS=( ${COMP_WORDS[@]:1} ) return 0 }

# myfunction="
#   function '__myfunction'
#   {
#     echo "this is myfunction";
#     echo "num of args is $#";
#     return 0
#   }"

# echo $myfunction

