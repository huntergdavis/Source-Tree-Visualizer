echo here is a great starting point  http://www.graphicsmagick.org/Magick++/Image.html
echo this is the compilation line g++ -o imagemagick_resize_example imagemagick_example.cpp `Magick++-config --cppflags --cxxflags --ldflags --libs`
echo ""
g++ -o imagemagick_resize_example imagemagick_example.cpp `Magick++-config --cppflags --cxxflags --ldflags --libs`

