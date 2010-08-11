#!/bin/sh

if [ ! -d 'bin' ]; then
    mkdir bin
fi

if [ ! -d 'out' ]; then
    mkdir out
fi

if [ ! -d 'src/generate' ]; then
    echo Source directory 'generate' not found.
    exit 1
fi

g++ -g -Wall -o bin/tree_gen_example src/generate/tree.cpp src/generate/git.cpp `Magick++-config --cppflags --cxxflags --ldflags --libs`

