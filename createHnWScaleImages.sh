#!/bin/bash

# this crappy script expects scriptname Hbottom Htop Wbottom Wtop

echo this crappy script expects scriptname Hbottom Htop Wbottom Wtop

COUNT=0

# this is the height loop
for i in $(eval echo {$1..$2})
do
COUNT=$((COUNT + 1))
./bin/simple_tree -g /home/programmer/Projects/twitter-txt-rb/twitter-text-rb -H .$i -W .$3 -O  "$COUNT"twitter-txt-rb-h$i-w$3.jpg
done

# this is the width loop
for j in $(eval echo {$3..$4})
do
COUNT=$((COUNT + 1))
./bin/simple_tree -g /home/programmer/Projects/twitter-txt-rb/twitter-text-rb -H .$2 -W .$i -O "$COUNT"twitter-txt-rb-h$2-w$j.jpg
done

