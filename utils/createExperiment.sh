#!/bin/bash
#
# Simple script to nackup actual working directory for an future experiment
#

if [ $# -eq 0 ] || [ -z "$1" ] || [ ! -d "$1" ] then
    echo "Invalid arguments. Usage: createExperiment.sh <eperiment_dir>"
    exit 1
fi

cp -r cfg "$1"/cfg
cp -r data "$1"/data
