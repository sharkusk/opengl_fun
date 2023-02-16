CC := gcc
CFLAGS := -std=c99 -Wall -Wextra
LDFLAGS := -lEGL -lSOIL -lGLESv2 -lGL

SOURCES := background.cpp
OBJECTS := $(SOURCES:.cpp=.o)
EXECUTABLE := background

all: $(SOURCES) $(EXECUTABLE)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
