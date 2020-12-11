#!/bin/bash
#
# Simple script to generate N articles
#

if [ $# -ne 3 ]
then
    echo "Invalid arguments. Usage: createArticles.sh <File> <Number> <Prefix>"
    exit 1
fi

echo "ArticleName" >> $1

for i in $(seq 1 $2)
do
   echo "$3$i" >> $1
done

unix2dos $1
