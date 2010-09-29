#!/bin/bash
cd ~/Projects/source_tree_vis/ExampleScripts
convert -limit memory 400MiB -limit map 400MiB -loop 1 ~/Projects/source_tree_vis/out/*.jpg sourceTreeVis.gif
