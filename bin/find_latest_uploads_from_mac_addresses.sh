#!/bin/bash

#
# find_latest_uploads_from_mac_addresses.sh
#
# This script will find uploads from specific MAC addresses.  It was written
# to allow us to determine whether the STBs in D12/SNUG trials were still active.
# The list of MAC addresses are provided in a file passed in from the command line.
#
# The file should be formatted as follows:
#   mac_address_1 trialist_name_1
#   mac_address_2 trialist_name_2
#   mac_address_3 trialist_name_3
#
# The MAC address should be numbers and letters only (no colons), and the Trialist
# names must contain no spaces (underscores are allowed).  There should be a space
# between the MAC address and Trialist name.
#
# In addition to providing a list of MAC addresses, you can also specify the
# number of uploads you'd like reported for each MAC address (assuming more than
# one upload is found, of course)..
#

OLD_IFS=IFS
IFS=$'\n'

# Definitions for bold/normal formatting.  Used to prettify the output.
bold=$(tput bold)
normal=$(tput sgr0)

# Parameter defaults.
MAC_ADDRESS_LIST_FILE=""
MAXIMUM_NUMBER_OF_UPLOADS_TO_FIND=1


#############################################
## FUNCTIONS
#############################################

# Parse command line arguments.
function parse_args
{
    while [ $# -gt 0 ]
    do
        case "$1" in
            -m | --maximum)
                shift
                MAXIMUM_NUMBER_OF_UPLOADS_TO_FIND=$1
                if [ $MAXIMUM_NUMBER_OF_UPLOADS_TO_FIND -lt 1 ]
                then
                    echo "'$MAXIMUM_NUMBER_OF_UPLOADS_TO_FIND' isn't large enough.  Please enter a value greater than 0."
                    echo ""
                    exit 1
                fi
                ;;
            -l | --list)
                shift
                MAC_ADDRESS_LIST_FILE=$1
                if [ ! -e $MAC_ADDRESS_LIST_FILE ]
                then
                    echo "The MAC address list file '$MAC_ADDRESS_LIST_FILE' cannot be found."
                    echo ""
                    exit 1
                fi
                MAC_ADDRESS_LIST=$(cat $MAC_ADDRESS_LIST_FILE)
                NUMBER_OF_MAC_ADDRESSES=$(cat $MAC_ADDRESS_LIST_FILE | wc -l)
                ;;
            -h | --help)
                usage
                exit 0
                ;;
            *)
                # Bail out if we hit something unexpected.
                echo "'$1' is not a valid parameter.  Please use the -h or --help parameter for more information."
                echo ""
                exit 1
                ;;
        esac
        shift
    done

    # Check all mandatory parameters are set.
    if [ -z "$MAC_ADDRESS_LIST_FILE" ]
    then
        usage
        echo "The mandatory -l/--list parameter was not provided."
        echo ""
        exit 1
    fi
}


# Run the SQL command specified in the first parameter.
# If a second parameter wasn't provided, the result is converted to a
# pipe-delimited format.  This makes extraction of the columns easy.  If there
# was a second parameter (the actual value doesn't) matter, then we don't convert
# the output, and instead just display it using the SQL table format.  This makes
# output of the results in tabular format easier.
function run_sql_command
{
    # If we weren't called with any parameters.
    if [ -z "$1" ]
    then
        echo "The 'run_sql_command' function should be supplied with a parameter containing the SQL statement to be executed."
        exit 1
    fi

    if [ -n "$2" ]
    then
        # Output the results in SQL table format.
        echo "$(mysql --batch --table --column-names -u root uploads -e "$1")"
    else
        # Output the results in pipe-delimited format.
        echo "$(mysql --batch --skip-column-names -u root uploads -e "$1" | sed 's/\t/|/g')"
    fi
}


# Script usage.
function usage()
{
    echo ""
    echo "${bold}********************************************${normal}"
    echo "${bold}** Find Latest Uploads from MAC Addresses **${normal}"
    echo "${bold}********************************************${normal}"
    echo
    echo "${bold}DESCRIPTION${normal}"
    echo "    This script will find uploads from specific MAC addresses.  It was written"
    echo "    to allow us to determine whether the STBs in D12/SNUG trials were still active."
    echo "    The list of MAC addresses are provided in a file passed in from the command line."
    echo
    echo "    The file should be formatted as follows:"
    echo "        mac_address_1 trialist_name_1"
    echo "        mac_address_2 trialist_name_2"
    echo "        mac_address_3 trialist_name_3"
    echo
    echo "    The MAC address should be numbers and letters only (no colons), and the Trialist"
    echo "    names must contain no spaces (underscores are allowed).  There should be a space"
    echo "    between the MAC address and Trialist name."
    echo
    echo "    In addition to providing a list of MAC addresses, you can also specify the"
    echo "    number of uploads you'd like reported for each MAC address (assuming more than"
    echo "    one upload is found, of course)."
    echo
    echo "${bold}USAGE${normal}"
    echo "    find_latest_uploads_from_mac_addresses.sh [OPTION]..."
    echo
    echo "    ${bold}-l, --list [FILE] (mandatory)${normal}"
    echo "        File containing a list of MAC addresses on which the report should be"
    echo "        limited.  See the description above for the required file format."
    echo
    echo "    ${bold}-m, --maximum (optional)${normal}"
    echo "        Maximum number of uploads that should be reported for each MAC"
    echo "        address (this must be >0).  This defaults to $MAXIMUM_NUMBER_OF_UPLOADS_TO_FIND."
    echo
    echo "    ${bold}-h, --help${normal}"
    echo "        Show this help text."
    echo
    echo "${bold}REPORTING BUGS${normal}"
    echo "        Please report bugs to:"
    echo "            Karl Botragyi (kbotragy@cisco.com)"
    echo "            Hudkins Team  (bskyb-si-hudkins-support@external.cisco.com)"
    echo
    echo
}


#############################################
## MAIN
#############################################

# Parse command line arguments.
parse_args "$@"

echo
echo "${bold}********************************************${normal}"
echo "${bold}** Find Latest Uploads from MAC Addresses **${normal}"
echo "${bold}********************************************${normal}"
echo
echo "Number of MAC addresses provided ...... $NUMBER_OF_MAC_ADDRESSES"
echo "Maximum number of uploads to report ... $MAXIMUM_NUMBER_OF_UPLOADS_TO_FIND"
echo
echo "Searching..."
echo
echo

# Iterate over the file a line at a time.
for LINE in $MAC_ADDRESS_LIST
do
    # Extract MAC address and Trialist name from the line.
    MAC_ADDRESS=$(echo $LINE | cut -d ' ' -f 1)
    TRIALIST_NAME=$(echo $LINE | cut -d ' ' -f 2)

    # Find uploads for the current MAC address.
    SQL_FIND_MATCHING_UPLOADS="                                 \
        SELECT      version,                                    \
                    upload_time,                                \
                    filename                                    \
        FROM        upload                                      \
        WHERE       mac = '$MAC_ADDRESS'                        \
        ORDER BY upload_time DESC, version DESC, filename       \
        LIMIT       $MAXIMUM_NUMBER_OF_UPLOADS_TO_FIND;"
    MATCHING_UPLOADS=$(run_sql_command "$SQL_FIND_MATCHING_UPLOADS" with_table)

    # No uploads were found.
    if [ -z "$MATCHING_UPLOADS" ]
    then
        echo "${bold}- No uploads found for Trialist $TRIALIST_NAME (MAC address $MAC_ADDRESS).${normal}"
        echo
        echo
        echo
        continue
    fi

    # Uploads were found, output the table in MATCHING_UPLOADS.
    echo "${bold}+ Found the following uploads from Trialist $TRIALIST_NAME (MAC address $MAC_ADDRESS).${normal}"
    echo "$MATCHING_UPLOADS"
    echo
    echo
done

echo
echo "Finished."
echo
echo

IFS=OLD_IFS

# END
