OS_NAME := $(shell uname)

CXX = g++
CC = gcc
BASEDIR = .
INCLUDE = -I$(BASEDIR) -ISupportCode
OPTFLAGS = -O3 -g -Wall
CXXFLAGS = $(OPTFLAGS) $(INCLUDE)
SUP = SupportCode/

  OBJ	= main.o GUI.o $(SUP)Util.o ObjIO.o SimpleMesh.o \
HalfEdgeMesh.o $(SUP)ColorMap.o Mesh.o $(SUP)Heap.o \
$(SUP)DecimationMesh.o SimpleDecimationMesh.o QuadricDecimationMesh.o \
 Implicit.o $(SUP)MarchingCubes.o Quadric.o Sphere.o Cyclide.o SphereFractal.o

ifeq "$(OS_NAME)" "Darwin"
LDFLAGS = -framework GLUT -framework OpenGL -framework Cocoa
else
LDFLAGS = -lGL -lGLU -lglut
endif

all: objs

objs: $(OBJ) main #tests

main: $(OBJ)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o "main" $(OBJ)

#tests: main
#	cd Tests; make


clean:
	rm -fr *.o
	rm -fr *~
	rm -fr $(SUP)*.o
	rm -fr $(SUP)*~
