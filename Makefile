CXX = gcc
CFLAGS = -Wall -Wextra -Wshadow -fno-diagnostics-color $(shell pkg-config --cflags freetype2) 
LDLIBS += $(shell pkg-config --libs freetype2)

SRC_EXT := c
SOURCES := $(wildcard *.$(SRC_EXT))
OBJECTS := $(patsubst %.$(SRC_EXT), %.o, $(SOURCES))

TARGET := fontfetch 

.PHONY = clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@echo "$(CXX) $(CFLAGS) $(LDLIBS) $^ -o $@"; $(CXX) $(CFLAGS) $(LDLIBS) $^ -o $@

%.o: %.$(SRC_EXT)
	@echo "$(CXX) $(CFLAGS) -c $< -o $@"; $(CXX) $(CFLAGS) -c $< -o $@

clean:
	@echo "$(RM) $(OBJECTS) $(TARGET)"; $(RM) $(OBJECTS) $(TARGET)
