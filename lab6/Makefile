OS_NAME := $(shell uname)

CXX = g++
CC = gcc
BASEDIR = .
INCLUDE = -I$(BASEDIR) -ISupportCode
OPTFLAGS = -O3 -g3 -Wall
CXXFLAGS = $(OPTFLAGS) $(INCLUDE)
SUP = SupportCode/

UTIL = $(SUP)Util.cpp ObjIO.cpp $(SUP)ColorMap.cpp \
$(SUP)ScalarCutPlane.cpp $(SUP)VectorCutPlane.cpp $(SUP)Heap.cpp

GUI = GUI.cpp main.cpp

MESH = HalfEdgeMesh.cpp $(SUP)DecimationMesh.cpp SimpleDecimationMesh.cpp\
QuadricDecimationMesh.cpp $(SUP)MarchingCubes.cpp SimpleMesh.cpp Mesh.cpp

IMPLICITS =  Implicit.cpp Quadric.cpp Sphere.cpp SphereFractal.cpp \
SignedDistanceSphere.cpp Cube.cpp

LEVELSET = $(SUP)LevelSetGrid.cpp LevelSet.cpp $(SUP)LevelSetOperator.cpp

FLUIDSOLVER = EulerIntegrator.cpp SemiLagrangianIntegrator.cpp\
 NavierStokesSolver.cpp VolumeLevelSet.cpp FluidSolverSparseMatrix.cpp\
FluidSolverVector.cpp FluidSimSetup.cpp TrilinearInterpolator.cpp

SUBDIVISION = LoopSubdivisionMesh.cpp AdaptiveLoopSubdivisionMesh.cpp


SOURCE =  $(UTIL) $(GUI) $(MESH) $(IMPLICITS) $(LEVELSET)\
          $(FLUIDSOLVER) $(SUBDIVISION)

OBJ = $(SOURCE:.cpp=.o)

ifeq "$(OS_NAME)" "Darwin"
LDFLAGS = -framework GLUT -framework OpenGL -lobjc
else
LDFLAGS = -lGL -lGLU -lglut
endif

all: $(OBJ)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o "main" $(OBJ)

# Automatic dependency updating
%.d: %.cpp
	@echo "Building dependencies for"  $(basename $@).o
	@$(CXX) -MM -MT $(basename $@).o $(INCLUDE) $< > $@

ifneq ($(MAKECMDGOALS),clean)
include $(SOURCE:.cpp=.d)
endif

clean:
	rm -fr *.o
	rm -fr *~
	rm -fr $(SUP)*.o
	rm -fr $(SUP)*~
	rm -fr *.d
	rm -fr $(SUP)*.d
