INCLUDES =-I./include \
	-I./libs/stb_image \
	-I./libs/mach/include
	
CC=g++ -std=c++11
CFLAGS=

ODIR=obj
SRC_DIR=./src
	
LIBS= \
	-lGLEW \
	-lglfw \
	-lGL

_OBJ= \
	minimap.o \
	game.o \
	maze.o \
	matrix.o \
	shader.o

OBJ=$(patsubst %,$(ODIR)/%,$(_OBJ))

OUTPUT=maze_3d

.PHONY: all
	
all: $(OUTPUT)

.PHONY: debug

debug: CFLAGS += -D DEBUG -g
debug: $(OUTPUT)

$(OUTPUT): main.cpp $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(INCLUDES) $(LIBS)

$(ODIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(ODIR)
	$(CC) -c -o $@ $< $(CFLAGS) $(INCLUDES) $(LIBS)

$(ODIR)/%.o: ./libs/mach/src/%.cpp
	@mkdir -p $(ODIR)
	$(CC) -c -o $@ $< $(CFLAGS) $(INCLUDES) $(LIBS)

.PHONY: clean

clean:
	@rm -rf $(ODIR) $(OUTPUT)
