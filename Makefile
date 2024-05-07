CC=gcc
CFLAGS=-lm
INC=-I Include/
EXEC=projet

all: $(EXEC)

projet: main.c files.c filter.c noise.c
	$(CC) $(INC) $^ -o $@ $(CFLAGS)