OS_NAME := $(shell uname)

CXX = g++
CC = gcc
BASEDIR = .
INCLUDE = -I$(BASEDIR) -ISupportCode
OPTFLAGS = -O3 -g -Wall
CXXFLAGS = $(OPTFLAGS) $(INCLUDE)
SUP = SupportCode/

UTIL = $(SUP)Util.o ObjIO.o $(SUP)ColorMap.o \
$(SUP)ScalarCutPlane.o $(SUP)VectorCutPlane.o $(SUP)Heap.o

MESH = HalfEdgeMesh.o $(SUP)DecimationMesh.o SimpleDecimationMesh.o\
QuadricDecimationMesh.o $(SUP)MarchingCubes.o SimpleMesh.o Mesh.o

IMPLICITS =  Implicit.o Quadric.o Sphere.o SphereFractal.o \
SignedDistanceSphere.o

LEVELSET = $(SUP)LevelSetGrid.o LevelSet.o $(SUP)LevelSetOperator.o

FLUIDSOLVER =

OBJ = main.o GUI.o $(UTIL) $(MESH) $(IMPLICITS) $(LEVELSET) $(FLUIDSOLVER)

ifeq "$(OS_NAME)" "Darwin"
LDFLAGS = -framework GLUT -framework OpenGL -framework Cocoa
else
LDFLAGS = -lGL -lGLU -lglut
endif

all: objs

objs: $(OBJ) main

main: $(OBJ)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o "main" $(OBJ)

$(IMPLICITS) : $(SUP)Bbox.h
$(LEVELSET) : $(SUP)Bbox.h

GUI.o : CSG.h $(SUP)Bbox.h OperatorReinitialize.h OperatorMeanCurvatureFlow.h\
OperatorAdvect.h OperatorDilateErode.h

clean:
	rm -fr *.o
	rm -fr *~
	rm -fr $(SUP)*.o
	rm -fr $(SUP)*~
