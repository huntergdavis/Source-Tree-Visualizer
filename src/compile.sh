echo here is a great starting point  http://www.graphicsmagick.org/Magick++/Image.html
echo this is the compilation line g++ -o tree_gen_example generate/tree.cpp `Magick++-config --cppflags --cxxflags --ldflags --libs`
echo ""
g++ -o tree_gen_example generate/tree.cpp `Magick++-config --cppflags --cxxflags --ldflags --libs`

