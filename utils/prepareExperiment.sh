#!/bin/sh
#
# Simple script to prepare working directory for an experiment
# Note: Local workspace will be overwritten - ie. all modifications lost
# Note: Call from project root directory
#

if [ $# -eq 0 ] || [ -z "$1" ] || [ ! -d "$1" ]
then
    echo "Invalid arguments. Usage: prepareExperiment.sh <eperiment_dir>"
    exit 1
fi

cp -r "$1"/cfg .
cp -r "$1"/data .
