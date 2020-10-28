#!/bin/sh
#
# Simple script to nackup actual working directory for an future experiment
# Note: Call from project root directory
#

if [ $# -eq 0 ] || [ -z "$1" ] || [ -d "$1" ]
then
    echo "Invalid arguments. Usage: createExperiment.sh <eperiment_dir>"
    exit 1
fi

mkdir "$1"
cp -r cfg "$1"
cp -r data "$1"
