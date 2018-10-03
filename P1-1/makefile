# author1: Dasong Gao
# author2: Haozhe Luo

CC = gcc
WARNING_FLAGS = -Wall -Wextra
EXE = 537ps
SCAN_BUILD_DIR = scan-build-out

all: $(EXE)

premium: main.o premium_utils.o $(EXE) clean

$(EXE): main.o utils.o
	$(CC) -o $(EXE) main.o utils.o

main.o: main.c utils.h
	$(CC) $(WARNING_FLAGS) -c main.c

utils.o: utils.c utils.h
	$(CC) $(WARNING_FLAGS) -c utils.c

premium_utils.o: utils.c utils.h
	$(CC) $(WARNING_FLAGS) -c utils.c -DMORE_FEATURES

clean:
	rm -rf *.o 537ps $(SCAN_BUILD_DIR)

scan-build: clean
	scan-build -o $(SCAN_BUILD_DIR) make

scan-view: scan-build
	firefox -new-window $(SCAN_BUILD_DIR)/*/index.html

