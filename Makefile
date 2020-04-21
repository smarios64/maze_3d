IDIR =-I./include \
	-I./libs/include
	
CC=g++ -std=c++11
CFLAGS=$(IDIR)

ODIR=obj
SRC_DIR=./src

LIBS_DIR=
	
LIBS= \
	-lGLEW \
	-lglfw \
	-lGL

_DEPS=
DEPS=$(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ= \
	minimap.o \
	game.o

OBJ=$(patsubst %,$(ODIR)/%,$(_OBJ))

OUTPUT=maze_3d

.PHONY: all
	
all: $(OUTPUT)

.PHONY: debug

debug: CFLAGS += -D DEBUG -g
debug: $(OUTPUT)

$(OUTPUT): main.cpp $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS_DIR) $(LIBS)

$(ODIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(ODIR)
	$(CC) -c -o $@ $< $(CFLAGS) $(LIBS_DIR) $(LIBS)

.PHONY: clean

clean:
	@rm -rf $(ODIR) $(OUTPUT)
