#!/bin/sh

if [ ! -d 'bin' ]; then
    mkdir bin
fi

if [ ! -d 'out' ]; then
    mkdir out
fi

if [ ! -d 'src/dis/stv' ]; then
    echo Project source directory not found.
    exit 1
fi

# Build example project
#g++ -g -Wall -o bin/tree_gen_example src/generate/tree.cpp src/generate/git.cpp src/generate/xml.cpp `Magick++-config --cppflags --cxxflags --ldflags --libs` > ./compile_results.txt
# Build real project
#g++ -g -Wall -o bin/simple_tree src/dis/stv/main/simple_tree.cpp src/dis/stv/io/git_repository_access.cpp src/dis/stv/gen/space_colonizer.cpp src/dis/stv/draw/scanline_artist.cpp src/dis/stv/dec/spatial_displacement.cpp -Isrc/dis/stv/dec -Isrc/dis/stv/draw -Isrc/dis/stv/gen -Isrc/dis/stv/io -Isrc/dis/stv/main -I/usr/local/boost `Magick++-config --cppflags --cxxflags --ldflags --libs` > ./compile_results.txt

g++ -g -Wall -W -pthread -o bin/simple_tree src/dis/stv/main/simple_tree.cpp src/dis/stv/io/git_repository_access.cpp src/dis/stv/gen/space_colonizer.cpp src/dis/stv/draw/scanline_artist.cpp src/dis/stv/dec/spatial_displacement.cpp -Isrc/dis/stv/dec -Isrc/dis/stv/draw -Isrc/dis/stv/gen -Isrc/dis/stv/io -Isrc/dis/stv/main -I/usr/local/boost -I/usr/include/ImageMagick -L/usr/lib -Wl,-Bsymbolic-functions -L/usr/lib/X11 -L/usr/lib -lMagick++ -lMagickWand -lMagickCore -llcms -ltiff -lfreetype -ljpeg -lXext -lSM -lICE -lX11 -lXt -lbz2 -lz -lm -lgomp -lpthread -lltdl > ./compile_results.txt

