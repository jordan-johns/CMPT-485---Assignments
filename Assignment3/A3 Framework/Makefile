#  Copyright (C) 2002 -- Daniel Neilson

# To add files to the project, scroll down to OBJ_FILES =
#  add any files that you add to the project to that assignment.
#  That should be all that is required. It's like magic!

.SUFFIXES = .o .cpp .c

# What compiler are we going to use?
CC = gcc
CXX = g++

# Version #'s of the program

# Flags for the preprocessor.
CPPFLAGS =

# Our include directories
LDFLAGS = -lGL -lglfw -lm -lXrandr -lpng

# Set the compile flags depending on the make target

ifeq ($(MAKECMDGOALS),release)
#  If the release target is selected, then add in optimizations
# and turn off debugging
CPPFLAGS := $(CPPFLAGS) -DNDEBUG
CFLAGS = -Wall -msse2 -O2 -mfpmath=sse
else
#  If not release, then it's debug. 
CFLAGS = -Wall -Wextra -g
endif

# Code uses stuff from the C++0x standard, so set the dialect to that.
CXXFLAGS = $(CFLAGS) -std=c++0x

OBJ_FILES = \
	src/assign1.o \
	src/assign2.o \
	src/assign3.o \
	src/demo.o \
	src/main.o \
	src/Camera/camera.o \
	src/glUtils.o \
	src/UI/ui.o \
	src/GL3/gl3w.o \
	src/Shaders/shader.o \
	src/Shaders/manager.o \
	src/Shaders/glprogram.o \
	src/Shaders/material.o \
	src/Shaders/Constant/Specular/gouraud.o \
	src/Shaders/Constant/Specular/phong.o \
	src/Shaders/Constant/Lambertian/gouraud.o \
	src/Shaders/Constant/Lambertian/phong.o \
	src/Shaders/Constant/simple.o \
	src/Shaders/Constant/depth.o \
	src/Shaders/Texture/Specular/gouraud.o \
	src/Shaders/Texture/Specular/phong.o \
	src/Shaders/Texture/Lambertian/gouraud.o\
	src/Shaders/Texture/Lambertian/phong.o \
	src/ShadowMapping/shadowmap.o \
	src/Texture/texture.o \
	src/Texture/Decoders/decoder.o \
	src/Texture/Decoders/png.o \
	src/Objects/mesh.o \
	src/Objects/Models/sphere.o \
	src/Objects/Models/octahedron.o \
	src/Objects/Models/plane.o \
	src/Objects/object.o \
	src/Objects/geometry.o \
	src/RayTracing/rayintersector.o \
	src/RayTracing/ray.o \
	src/Scene/scene.o 

# What are we going to call our executable
OUT_FILE = assign3

debug: $(OBJ_FILES)
	$(CXX) $(CXXFLAGS) $(OBJ_FILES) -o $(OUT_FILE) $(LDFLAGS)

release: $(OBJ_FILES)
	$(CXX) $(CXXFLAGS) $(OBJ_FILES) -o $(OUT_FILE) $(LDFLAGS)


# The rule for making the .d files from the .c & .cpp files
# The 'sed' part just makes it so that the generated .d file will depend on 
# the same things as the .o file -- It also adds a dependancy on this file
# to the .o & the .d
%.d: %.cpp
	@echo 'Creating/Updating $*.d'
	@set -e; $(CXX) -MM -w $(CXXFLAGS) $< \
	| sed 's/\($(notdir $*)\)\.o[ :]*/$(subst /,\/,$(@:.d=.o)) $(subst /,\/,$(@)) : Makefile /g' \
	> $@; \
	[ -s $@ ] || rm -f $@
%.d: %.c
	@echo 'Creating/Updating $*.d'
	@set -e; $(CXX) -MM -w $(CXXFLAGS) $< \
	| sed 's/\($(notdir $*)\)\.o[ :]*/$(subst /,\/,$(@:.d=.o)) $(subst /,\/,$(@)) : Makefile /g' \
	> $@; \
	[ -s $@ ] || rm -f $@

# include the dependency files.
# -- If they're not there, then the "%.d: %.c" & "%.d: %.cpp" rule will 
# make them
ifneq (clean,$(findstring clean,$(MAKECMDGOALS)))
-include $(OBJ_FILES:.o=.d)
endif

clean: clean_obj clean_tilde clean_core
	@echo Deleting executable
	@[ ! -f $(OUT_FILE) ] || rm $(OUT_FILE)

clean_obj: FORCE
	@echo Deleting object files
	@rm -f $(OBJ_FILES)
	@rm -f $(OBJ_FILES:.o=.d)

clean_tilde: FORCE
	@echo Deleting temporary files.
	@rm -f `find . -name "*~"` 
	@rm -f `find . -name ".#*"`

clean_core: FORCE
	@echo Deleting core
	@[ ! -f core ] || rm -f core
	@rm -f core*

FORCE:

