/* author1: Dasong Gao
 * author2: Haozhe Luo
 */
CC = gcc
WARNING_FLAGS = -Wall -Wextra
EXE = 537ps

all: $(EXE) clean

$(EXE): main.o utils.o
	$(CC) -o $(EXE) main.o utils.o

main.o: main.c utils.h utils.c
	$(CC) $(WARNING_FLAGS) -c main.c

utils.o: utils.c utils.h
	$(CC) $(WARNING_FLAGS) -c utils.c

clean:
	rm *.o
