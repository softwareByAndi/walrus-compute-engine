# Compiler and compiler flags
CXX = g++
CFLAGS = -std=c++17 -O2
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

# Target and source file
TARGET = VulkanTest
SRC = test.cpp

# Makefile rules
all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CFLAGS) -o $@ $^ $(LDFLAGS)

.PHONY: test clean

test: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
