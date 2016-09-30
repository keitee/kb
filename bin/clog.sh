#!/bin/bash
F=`echo $1 | cut -d'_' -f3`
echo "=== on $F ..."
ssh theyard check_log $F
