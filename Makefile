TARGET = main.exe

CXX = g++
CXXFLAGS = -Isrc/Include -Itmxlite/include -Lsrc/lib -mwindows
LIBS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer

SOURCES = main.cpp tmxlite/src/tmx/*.cpp
OBJS = $(SOURCES:.cpp=.o)

all:
	$(CXX) $(CXXFLAGS) -o $(TARGET) main.cpp $(LIBS)
	$(TARGET) run

run:
	./$(TARGET)

clean:
	rm -f $(TARGET) $(OBJS)