#!/bin/bash
cd ~/Projects/source_tree_vis/ExampleScripts
convert -limit memory 40MiB -limit map 40MiB -loop 1 ~/Projects/source_tree_vis/out/*.jpg sourceTreeVis.gif
