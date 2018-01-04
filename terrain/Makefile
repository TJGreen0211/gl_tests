CC = gcc
CFLAGS = -Wall
PROG = cube

MAKE_DIR = ${PWD}

SRC_DIR := $(MAKE_DIR)/src

SRCS = Main.c LoadShader.c LinearAlg.c arcballCamera.c Window.c Sphere.c Geometry.c Obj.c QuadCube.c QuadTree.c Water.c SimplexNoise.c src/Common.c Textures.c

#SRCS := $(shell echo dir1/*.c)
INCLUDE_PATHS = -I/usr/local/include -I/opt/X11/include -I/opt/local/include -I/Users/tjgreen/Documents/OpenGL/Dependencies/include -I/$(SRC_DIR) -I/$(MAKE_DIR)
LIBRARY_PATHS = -I/opt/X11/lib -L/Users/tjgreen/Documents/OpenGL/Dependencies/lib -lSOIL -lglfw.3.1
ifeq ($(shell uname),Darwin)
	LIBS = -framework OpenGL -framework GLUT
else
	LIBS = -lglut
endif

all: $(PROG)
	@echo $(SRC_DIR)

$(PROG):	$(SRCS)
	$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(LIBS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(RELATIVE_PATHS)

clean:
	rm -f $(PROG)
