

CC = g++

CFLAGS = -c -Wall -W -pthread -std=c++0x

DEBUGFLAGS = -ggdb

INCLUDEFLAGS = -Isrc/dis/stv/dec \
	-Isrc/dis/stv/draw \
	-Isrc/dis/stv/gen \
	-Isrc/dis/stv/io \
	-Isrc/dis/stv/main \
	-Isrc/dis/stv/physics \
	-I/usr/local/boost \
	-I/usr/include/libxml2 \
	-I/usr/include/ImageMagick
	
LDFLAGS = -L/usr/lib \
	-Wl,-Bsymbolic-functions \
	-L/usr/lib/X11 \
	-L/usr/lib \
	-lMagick++ \
	-lMagickWand \
	-lMagickCore \
	-llcms \
	-ltiff \
	-lfreetype \
	-ljpeg \
	-lXext \
	-lSM \
	-lICE \
	-lX11 \
	-lXt \
	-lbz2 \
	-lz \
	-lm \
	-lgomp \
	-lpthread \
	-lcurl \
 	-lxml2 \
	-lltdl
	
SOURCES = src/dis/stv/system/discursive_error.cpp \
	src/dis/stv/system/discursive_debug.cpp \
	src/dis/stv/system/discursive_print.cpp \
	src/dis/stv/system/discursive_time.cpp \
	src/dis/stv/io/repository_access.cpp \
	src/dis/stv/io/git_repository_access.cpp \
	src/dis/stv/io/svn_remote_repository_access.cpp \
	src/dis/stv/io/cvs_remote_repository_access.cpp \
	src/dis/stv/io/github_repository_access.cpp \
	src/dis/stv/gen/space_colonizer.cpp \
	src/dis/stv/gen/colonization_leader.cpp \
	src/dis/stv/gen/digitizer_factory.cpp \
	src/dis/stv/draw/scanline_artist.cpp \
	src/dis/stv/draw/drawable_data.cpp \
	src/dis/stv/draw/drawable_datum.cpp \
	src/dis/stv/dec/spatial_displacement.cpp \
	src/dis/stv/dec/decorator_factory.cpp \
	src/dis/stv/main/simple_tree.cpp \
	src/dis/stv/main/initial_agents.cpp \
	src/dis/stv/main/configuration_agent.cpp \
	src/dis/stv/physics/physical_mass.cpp \
	src/dis/stv/physics/physics_engine.cpp \
	src/dis/stv/physics/time_stepped_physics_engine.cpp \
	src/dis/stv/physics/time_stepped_physics_object.cpp \
	src/dis/stv/physics/tree_displacement_node.cpp \
	src/dis/stv/physics/unidimensional_spatial_object.cpp \
	src/dis/stv/physics/spatial_object.cpp \
	src/dis/stv/physics/fixed_attractor.cpp \
	src/dis/stv/model/surrogate_tree_node.cpp \
	src/dis/stv/model/property_transformer.cpp \
	src/dis/stv/model/property_inverter.cpp \
	src/dis/stv/model/property_shifter.cpp
	

OBJECTS = $(SOURCES:.cpp=.o)
EXAMPLEBINARY = bin/tree_gen_example
PROJECTBINARY = bin/simple_tree

all: $(SOURCES) $(PROJECTBINARY)

$(PROJECTBINARY): $(OBJECTS)
	$(CC) $(DEBUGFLAGS) $(INCLUDEFLAGS) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(DEBUGFLAGS) $(CFLAGS) $(INCLUDEFLAGS) $(LDFLAGS)  $< -o $@

clean:
	rm $(OBJECTS)
