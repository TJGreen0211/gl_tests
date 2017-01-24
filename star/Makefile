CC = gcc
CFLAGS = -Wall
PROG = cube

SRCS = Main.c LoadShader.c LinearAlg.c Sphere.c Camera.c

INCLUDE_PATHS = -I/usr/local/include -I/opt/X11/include -I/opt/local/include -I/Users/tjgreen/Documents/OpenGL/Dependencies/include
LIBRARY_PATHS = -lSOIL -I/opt/X11/lib -L/Users/tjgreen/Documents/OpenGL/Dependencies/lib -lSOIL -lglfw.3.1

ifeq ($(shell uname),Darwin)
	LIBS = -framework OpenGL -framework GLUT
else
	LIBS = -lglut
endif

all: $(PROG)

$(PROG):	$(SRCS)
	$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(LIBS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(RELATIVE_PATHS)

clean:
	rm -f $(PROG)
