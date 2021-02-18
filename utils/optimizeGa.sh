#!/bin/bash

DATADIR=/storage/brno2/home/xkocic01/

echo "$PBS_JOBID is running on node `hostname -f` in a scratch directory $SCRATCHDIR" >> $DATADIR/jobs_info.txt

test -n "$SCRATCHDIR" || { echo >&2 "Variable SCRATCHDIR is not set!"; exit 1; }

cp -r $DATADIR/warehouse-manager $SCRATCHDIR || { echo >&2 "Error while copying input file(s)!"; exit 2; }

#cp $DATADIR/configs/optimizer_ga.xml $SCRATCHDIR/warehouse-manager/cfg/optimizer.xml

cd $SCRATCHDIR/warehouse-manager

# TODO: make, config, ...

./whm_opt -o data/750_1000/orders_train.xml -a data/750_1000/articles.csv -l data/750_1000/layout.xml -i data/750_1000/locations.csv -O1 > res_opt_ga.txt

cp res_opt_ga.txt $DATADIR/res/ga/ || { echo >&2 "Result file(s) copying failed (with a code $?) !!"; exit 3; }
cp data/locations.csv $DATADIR/res/ga/

clean_scratch