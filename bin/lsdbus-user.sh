#!/bin/sh -e

# Print a list of all dbus connections and the processes that own them.

# The format of the output is:
# pid | bus name | command

function ListBusNames {
    dbus-send --session --type=method_call --print-reply \
              --dest=org.freedesktop.DBus / \
              org.freedesktop.DBus.ListNames
}

function GetProcessID {
    dbus-send --session --type=method_call --print-reply \
              --dest=org.freedesktop.DBus / \
              org.freedesktop.DBus.GetConnectionUnixProcessID string:$1 2>/dev/null \
        | xargs -n1 | tail -1
}

for i in $(ListBusNames | grep string | cut -d'"' -f2)
do
    DCNAME=$i
    DCPID=$(GetProcessID $DCNAME)
    if [ -n "$DCPID" ]
    then
         DCPCMD=$(cat /proc/$DCPID/cmdline 2>/dev/null | xargs -0 echo) && \
#        DCPCMD=$(cat /proc/$DCPID/cmdline 2>/dev/null) && \
               printf "%-6s%-25s%-40s%-8s%s\n" \
                  "$DCPID" "$DCNAME" "$DCPCMD"
    fi
done | sort -n
