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

