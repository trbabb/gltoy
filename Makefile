# this is probably the least automatic makefile ever
# and it's not reliable because the deps aren't really right
# but oh well

CC = clang++
LD = clang++
AR = ar

# compile
INCLUDES = src
CFLAGS   = -std=c++11 -O3 -Wall -c -fmessage-length=0 -Wno-deprecated-declarations -g
IFLAGS   = $(addprefix -I, $(INCLUDES))

# link
LIBDIRS  = /System/Library/Frameworks/OpenGL.framework/Libraries
LIBS     = geomc png z GLEW glfw
LDFLAGS  = $(addprefix -l, $(LIBS)) $(addprefix -L, $(LIBDIRS)) \
           -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo 

# sources
# not included right now: module `thruster`
MODULES  = visible toolbox thruster
SRC      = $(wildcard src/*.cpp) \
           $(foreach m, $(MODULES), $(wildcard src/$(m)/*.cpp))
OBJ      = $(patsubst src/%.cpp, build/%.o, $(SRC))

LIBNAME  = libgltoy.a
LIB      = lib/$(LIBNAME)

# todo: see http://scottmcpeak.com/autodepend/autodepend.html

all: lib random texture occlusion

clean:
	rm -rf ./build/*
	rm -f ./lib/*.a
	rm -f ./bin/*

## binaries

random: $(OBJ) bindir build/scene/random.o
	$(CC) $(LDFLAGS) $(OBJ) build/scene/random.o -o bin/random

texture: $(OBJ) bindir build/scene/texture.o
	$(CC) $(LDFLAGS) $(OBJ) build/scene/texture.o -o bin/texture

occlusion: $(OBJ) bindir build/scene/occlusion.o
	$(CC) $(LDFLAGS) $(OBJ) build/scene/occlusion.o -o bin/occlusion

perlin: $(OBJ) bindir build/scene/Perlin.o
	$(CC) $(LDFLAGS) $(OBJ) build/scene/Perlin.o -o bin/perlin

pick: $(OBJ) bindir build/scene/PickerScene.o
	$(CC) $(LDFLAGS) $(OBJ) build/scene/PickerScene.o -o bin/pick

build/scene/random.o : src/scene/RandomScene.cpp
	mkdir -p build/scene
	$(CC) $(CFLAGS) $(IFLAGS) -o build/scene/random.o src/scene/RandomScene.cpp

build/scene/texture.o : src/scene/TextureScene.cpp
	mkdir -p build/scene
	$(CC) $(CFLAGS) $(IFLAGS) -o build/scene/texture.o src/scene/TextureScene.cpp

build/scene/occlusion.o : src/scene/OcclusionScene.cpp
	mkdir -p build/scene
	$(CC) $(CFLAGS) $(IFLAGS) -o build/scene/occlusion.o src/scene/OcclusionScene.cpp

bindir:
	mkdir -p bin

# lib

lib: $(OBJ)
	mkdir -p lib
	$(AR) rs $(LIB) $(OBJ)
	@echo
	@echo Done building library.

# other targets

build/%.o : src/%.cpp
	mkdir -p $(patsubst src/%, build/%, $(dir $<))
	$(CC) $(CFLAGS) $(IFLAGS) -o $@ $<

