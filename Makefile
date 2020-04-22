CC	=	gcc

CFLAGS	=	-Wall -Werror -pedantic -std=gnu99
EXE = ping
OBJ = main.o argparse.o ping.o
all: $(EXE)

debug: CFLAGS = -Wall -Werror -pedantic -lpthread -std=gnu99 -O0 -g -DDEBUG

main.o: argparse.h ping.h
ping.o: ping.h argparse.h
argparse.o: argparse.h


debug: $(OBJ)
	$(CC) -o $(EXE) $(OBJ) $(CFLAGS)

	
$(EXE): $(OBJ)
		$(CC) -o $(EXE) $(OBJ) $(CFLAGS)

.PHONY: clean

clean:
	rm $(OBJ)
	rm $(EXE)