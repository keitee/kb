# loop_2.sh

# ={=========================================================================
# sh-loop

# Works only in ksh88/ksh93, or zsh or bash 4.2 with lastpipe enabled
# In other shells, this will print 0

# keitee@kit-ubuntu:~/git/kb/code-sh$ bash loop_2.sh
# total number of lines: 0

linecount=0

# here "print" is necessary to feed "read" command. 

printf '%s\n' foo bar |
while ifs= read -r line
do
    linecount=$((linecount + 1))
done

echo "total number of lines: $linecount"


# keitee@kit-ubuntu:~/git/kb/code-sh$ bash loop_3.sh
# echo hello
# read echo hello
# variable:
keite
f() {
    if [[ -t 0 ]]; then
        echo "echo $1"
    else
        read var
        echo "read $var"
    fi
};

f 'hello'

# keitee@kit-ubuntu:~/git/kb/code-sh$ bash loop_3.sh
# echo hello
#
# since
# -t fd
# True if file descriptor fd is open and refers to a terminal.
# "0" is stdin and it's open.

f 'hello' | f
# howwever, when use "|", 0 is not stdin and gets false.

echo "variable: $var" # prints nothing
