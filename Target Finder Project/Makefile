
GL_LIBS = `pkg-config --static --libs glfw3` -lGLEW 
EXT = 
CPPFLAGS = `pkg-config --cflags glfw3`

CC = g++
EXE = assign3_part2
OBJS = model-view.o Shader.o Viewer.o anObj.o loadTex.o loadObj.o plane.o

.PHONY: all clean

# If you haven't seen this before, notice how the rules
# of the Makefile build .o files from c++ source, and then
# build the executable from the .o files. Files are only
# re-made if they've been modified, or something they depend
# on has been modified.

all: $(EXE)

$(EXE): $(OBJS)
	$(CC) -o $(EXE) $(OBJS) $(GL_LIBS)

model-view.o: model-view.cpp InputState.h tiny_obj_loader.h 
	$(CC) $(CPPFLAGS) -c model-view.cpp

Shader.o : Shader.cpp Shader.hpp
	$(CC) $(CPPFLAGS) -c Shader.cpp

Viewer.o: Viewer.h Viewer.cpp InputState.h
	$(CC) $(CPPFLAGS) -c Viewer.cpp

anObj.o : anObj.cpp anObj.h tiny_obj_loader.h
	$(CC) $(CPPFLAGS) -c anObj.cpp

loadTex.o : loadTex.cpp loadTex.h tiny_obj_loader.h
	$(CC) $(CPPFLAGS) -c loadTex.cpp

loadObj.o : loadObj.cpp loadObj.h tiny_obj_loader.h
	$(CC) $(CPPFLAGS) -c loadObj.cpp

plane.o : plane.cpp plane.h
	$(CC) $(CPPFLAGS) -c plane.cpp

clean:
	rm -f *.o $(EXE)$(EXT)
