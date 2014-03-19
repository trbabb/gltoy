# this is probably the least automatic makefile ever
# and it's not reliable because the deps aren't really right
# but oh well

CC = g++
LD = g++
AR = ar

# compile
INCLUDES = /usr/local/boost src
CFLAGS   = -O3 -Wall -c -fmessage-length=0 -Wno-unused -Wno-unused-local-typedefs -Wno-deprecated-declarations -g
IFLAGS   = $(addprefix -I, $(INCLUDES))

# link
LIBDIRS  = /usr/local/boost/stage/lib \
           /System/Library/Frameworks/OpenGL.framework/Libraries
LIBS     = geomc GL png z SDL_mixer GLU boost_system
LDFLAGS  = $(addprefix -l, $(LIBS)) $(addprefix -L, $(LIBDIRS)) \
           -framework GLUT -framework OpenGL \
           `/opt/local/bin/sdl-config --libs --cflags` \

# sources
# not included right now: module `thruster`
MODULES  = sound visible toolbox thruster
SRC      = $(wildcard src/*.cpp) \
           $(foreach m, $(MODULES), $(wildcard src/$(m)/*.cpp))
OBJ      = $(patsubst src/%.cpp, build/%.o, $(SRC))

LIBNAME  = libgltoy.a
LIB      = lib/$(LIBNAME)
INCDIR   = /opt/local/include
LIBDIR   = /opt/local/lib

# todo: see http://scottmcpeak.com/autodepend/autodepend.html

all: lib sound occlusion

clean:
	rm -rf ./build/*
	rm -f ./lib/*.a

## binaries

random: $(OBJ) build/scene/random.o
	$(CC) $(LDFLAGS) $(OBJ) build/scene/random.o -o bin/random

sound: $(OBJ) build/scene/sound.o
	$(CC) $(LDFLAGS) $(OBJ) build/scene/sound.o -o bin/sound

occlusion: $(OBJ) build/scene/occlusion.o
	$(CC) $(LDFLAGS) $(OBJ) build/scene/occlusion.o -o bin/occlusion

build/scene/random.o : src/scene/RandomScene.cpp
	mkdir -p build/scene
	$(CC) $(CFLAGS) $(IFLAGS) -o build/scene/random.o src/scene/RandomScene.cpp

build/scene/sound.o : src/scene/SoundScene.cpp
	mkdir -p build/scene
	$(CC) $(CFLAGS) $(IFLAGS) -o build/scene/sound.o src/scene/SoundScene.cpp

build/scene/occlusion.o : src/scene/OcclusionScene.cpp
	mkdir -p build/scene
	$(CC) $(CFLAGS) $(IFLAGS) -o build/scene/occlusion.o src/scene/OcclusionScene.cpp

# lib

lib: $(OBJ)
	$(AR) rs $(LIB) $(OBJ)
	@echo
	@echo Done building library.

# other targets

build/%.o : src/%.cpp
	mkdir -p $(patsubst src/%, build/%, $(dir $<))
	$(CC) $(CFLAGS) $(IFLAGS) -o $@ $<

