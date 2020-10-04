# GLFW required (sudo apt install libglfw-dev)

SOURCE   = src/*.cpp src/*/*.c src/*/*.cpp
TARGET   = bin/imgui
CC       = g++
CPPFLAGS = -Iinclude -Isrc
CXXFLAGS = `pkg-config --cflags glfw3`
LDFLAGS  = -Llib -lGL `pkg-config --static --libs glfw3`

build:
	rm -f $(TARGET)
	$(CC) $(SOURCE) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) -o $(TARGET)
