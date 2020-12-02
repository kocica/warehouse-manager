#!/bin/bash

DATADIR=/storage/brno2/home/xkocic01/

echo "$PBS_JOBID is running on node `hostname -f` in a scratch directory $SCRATCHDIR" >> $DATADIR/jobs_info.txt

test -n "$SCRATCHDIR" || { echo >&2 "Variable SCRATCHDIR is not set!"; exit 1; }

cp -r $DATADIR/warehouse-manager $SCRATCHDIR || { echo >&2 "Error while copying input file(s)!"; exit 2; }

cp $DATADIR/configs/optimizer_de.xml $SCRATCHDIR/warehouse-manager/cfg/optimizer.xml

cd $SCRATCHDIR/warehouse-manager

# TODO: make, config, ...

./warehouse_manager_opt -o data/orders_train.xml -a data/articles.csv -l data/layout.xml -i data/locations.csv -O2 > res_opt_de.txt

cp res_opt_de.txt $DATADIR/res/de/ || { echo >&2 "Result file(s) copying failed (with a code $?) !!"; exit 3; }
cp data/locations.csv $DATADIR/res/de/

clean_scratch