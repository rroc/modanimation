OS_NAME := $(shell uname)

CXX = g++
CC = gcc
BASEDIR = .
INCLUDE = -I$(BASEDIR) -ISupportCode
OPTFLAGS = -O2 -g -Wall
CXXFLAGS = $(OPTFLAGS) $(INCLUDE) 
SUP = SupportCode/

OBJ	= main.o GUI.o $(SUP)Util.o ObjIO.o SimpleMesh.o HalfEdgeMesh.o $(SUP)ColorMap.o Mesh.o

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
	rm -fr SupportCode/*.o
	rm -fr Tests/*.o
