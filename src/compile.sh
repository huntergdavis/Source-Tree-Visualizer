#!/bin/sh

if [ ! -d 'bin' ]; then
    mkdir bin
fi

if [ ! -d 'out' ]; then
    mkdir out
fi

if [ ! -d 'generate' ]; then
    echo Source directory 'generate' not found.
    exit 1
fi

g++ -g -Wall -o bin/tree_gen_example generate/tree.cpp `Magick++-config --cppflags --cxxflags --ldflags --libs`

