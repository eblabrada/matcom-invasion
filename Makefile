CFLAGS ?= -O2 -Wall

binary = matcom_invasion
file = score

objects = \
	Engine/title.o \
	Engine/sprite.o \
	Engine/game.o \
	Engine/screen.o \
	Engine/keys.o \
	Engine/memory.o \
	Engine/score.o \
	Engine/utils.o \
	Engine/main.o

all: $(binary)

$(binary): $(objects)
	$(CC) $(LDFLAGS) -o $(binary) $(objects) -lncurses

clean:
	rm -f $(binary) $(objects) $(file)

.PHONY: all clean