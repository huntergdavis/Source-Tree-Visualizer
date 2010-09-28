#!/bin/bash

cd ~/Projects/source_tree_vis/
./bin/simple_tree -m 1:10000:1 -g ~/Projects/source_tree_vis -O sourceTreeVis.jpg
cd ~/Projects/source_tree_vis/ExampleScripts
convert ~/Projects/source_tree_vis/out/*.jpg sourceTreeVis.gif


