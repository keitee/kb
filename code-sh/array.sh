# array sh

# ={=========================================================================
# sh-array

# 6.7 Arrays
# ==========
# 
# Bash provides one-dimensional indexed and associative array variables.
# 
# Any variable may be used as an indexed array; the 'declare' builtin will
# explicitly declare an array.  There is no maximum limit on the size of an
# array, nor any requirement that members be indexed or assigned contiguously.
# Indexed arrays are referenced using integers (including arithmetic expressions
# (*note Shell Arithmetic::)) and are zero-based;
# 
# Unless otherwise noted, indexed array indices must be non-negative integers.
# 
# An indexed array is created *automatically* if any variable is assigned to
# using the syntax
# 
#      NAME[SUBSCRIPT]=VALUE
# 
# The SUBSCRIPT is treated as an arithmetic expression that must evaluate to a
# number. To explicitly declare an array, use
# 
#      declare -a NAME
# 
# The syntax
# 
#      declare -a NAME[SUBSCRIPT]
# 
# is also accepted; the SUBSCRIPT is ignored.


# associative arrays use arbitrary strings. 
# 
# Associative arrays are created using
# 
#      declare -A NAME

# Attributes may be specified for an array variable using the 'declare' and
# 'readonly' builtins.  Each attribute applies to all members of an array.

# Arrays are assigned to using compound assignments of the form
# 
#      NAME=(VALUE1 VALUE2 ... )
# 
# where each VALUE is of the form '[SUBSCRIPT]='STRING.  Indexed array
# assignments do not require anything but STRING.  When assigning to
# indexed arrays, if the optional subscript is supplied, that index is
# assigned to; otherwise the index of the element assigned is the last
# index assigned to by the statement plus one.
# 
#    When assigning to an associative array, the subscript is required.
# 
#    This syntax is also accepted by the 'declare' builtin.  Individual
# array elements may be assigned to using the 'NAME[SUBSCRIPT]=VALUE'
# syntax introduced above.
# 
#    When assigning to an indexed array, if NAME is subscripted by a
# negative number, that number is interpreted as relative to one greater
# than the maximum index of NAME, so negative indices count back from the
# end of the array, and an index of -1 references the last element.
# 
#
# *sh-array-reference*
# Any element of an array may be referenced using '${NAME[SUBSCRIPT]}'. The
# braces are required to avoid conflicts with the shell's filename expansion
# operators.

# If the SUBSCRIPT is '@' or '*', the word expands to all members of the array
# NAME.

# These subscripts differ only when the
# word appears within double quotes.  If the word is double-quoted,
# '${NAME[*]}' expands to a single word with the value of each array
# member separated by the first character of the 'IFS' variable, and
# '${NAME[@]}' expands each element of NAME to a separate word.  When
# there are no array members, '${NAME[@]}' expands to nothing.  If the
# double-quoted expansion occurs within a word, the expansion of the first
# parameter is joined with the beginning part of the original word, and
# the expansion of the last parameter is joined with the last part of the
# original word.  This is analogous to the expansion of the special
# parameters '@' and '*'.  '${#NAME[SUBSCRIPT]}' expands to the length of
# '${NAME[SUBSCRIPT]}'.  If SUBSCRIPT is '@' or '*', the expansion is the
# number of elements in the array.  If the SUBSCRIPT used to reference an
# element of an indexed array evaluates to a number less than zero, it is
# interpreted as relative to one greater than the maximum index of the
# array, so negative indices count back from the end of the array, and an
# index of -1 refers to the last element.
# 
#    Referencing an array variable without a subscript is equivalent to
# referencing with a subscript of 0.  Any reference to a variable using a
# valid subscript is legal, and 'bash' will create an array if necessary.
# 
#    An array variable is considered set if a subscript has been assigned
# a value.  The null string is a valid value.
# 
#    It is possible to obtain the keys (indices) of an array as well as
# the values.  ${!NAME[@]} and ${!NAME[*]} expand to the indices assigned
# in array variable NAME.  The treatment when in double quotes is similar
# to the expansion of the special parameters '@' and '*' within double
# quotes.
# 
#    The 'unset' builtin is used to destroy arrays.  'unset
# NAME[SUBSCRIPT]' destroys the array element at index SUBSCRIPT.
# Negative subscripts to indexed arrays are interpreted as described
# above.  Care must be taken to avoid unwanted side effects caused by
# filename expansion.  'unset NAME', where NAME is an array, removes the
# entire array.  A subscript of '*' or '@' also removes the entire array.
# 
#    The 'declare', 'local', and 'readonly' builtins each accept a '-a'
# option to specify an indexed array and a '-A' option to specify an
# associative array.  If both options are supplied, '-A' takes precedence.
# The 'read' builtin accepts a '-a' option to assign a list of words read
# from the standard input to an array, and can read values from the
# standard input into individual array elements.  The 'set' and 'declare'
# builtins display array values in a way that allows them to be reused as
# input.

# sh-assign
# `no space` between "name", "=", "[value]". Otherwise, treat it as command.

fruit1[0]=apple
fruit1[1]=banana
fruit1[2]=orange

fruit2=(apple banana orange)

# gets the first
echo "fruit1 = $fruit1"

#
echo "fruit1 = ${fruit1[0]}, ${fruit1[1]}, ${fruit1[2]}"
echo "fruit2 = ${fruit2[0]}, ${fruit2[1]}, ${fruit2[2]}"

echo "fruit1 expansion = ${fruit1[*]}"
echo "fruit1 expansion = ${fruit1[@]}"
echo "fruit1 expansion = ${fruit1[@]:1}"

fruit3[0]="apple"
fruit3[1]="banana"
fruit3[2]="orange"

echo "fruit3 expansion = ${fruit3[*]}"
echo "fruit3 expansion = ${fruit3[@]}"


# see how `expansion` is used
for i in ${fruit1[*]}; do
  echo "fruit1 has ${i}"
done


# note: have to use `bash` but not `sh`
#
# keitee@kit-hdebi:~/git/kb/code-sh$ bash array.sh
# fruit1 = apple
# fruit1 = apple, banana, orange
# fruit2 = apple, banana, orange
# fruit1 expansion = apple banana orange
# fruit1 expansion = apple banana orange
# fruit1 expansion = banana orange
# fruit3 expansion = apple banana orange
# fruit3 expansion = apple banana orange
# fruit1 has apple
# fruit1 has banana
# fruit1 has orange


# <ex>
# The both are the same.
# 
# declare -ar list=(
#     'darwin_0019fbd18c9a_487032cd821312aea3b11274c2ff7384'
#     'darwin_0019fbb71c47_c4130c208206b80b3963ee05c861b5c3'
#     'darwin_0019fbb71bb1_d23afbe329e75a1d9bcabc233373d398'
# )
# 
# declare -ar list2=(
#     darwin_0019fbd18c9a_487032cd821312aea3b11274c2ff7384
#     darwin_0019fbb71c47_c4130c208206b80b3963ee05c861b5c3
#     darwin_0019fbb71bb1_d23afbe329e75a1d9bcabc233373d398
# )
