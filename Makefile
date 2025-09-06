CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

SRC = main.cpp FileSystem.cpp BlockManager.cpp Directory.cpp Inode.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = filesystem

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ)

clean:
	rm -f $(OBJ) $(TARGET)

run: $(TARGET)
	./$(TARGET)
