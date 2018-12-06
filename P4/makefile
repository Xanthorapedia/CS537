# author1: Dasong Gao
# author2: Haozhe Luo

CC = gcc
WARNING_FLAGS = -Wall -Wextra
EXTRA_FLAGS = -O0 -g
CPPFLAGS = $(WARNING_FLAGS) $(EXTRA_FLAGS)
EXE = a
SCAN_BUILD_DIR = scan-build-out

all: $(EXE)

$(EXE): obj test_main.o
	$(CC) $(CPPFLAGS) -o $(EXE) test_main.o 537malloc.o

obj: 537malloc.c memavl.o utils.o autoarr.o
	$(CC) $(CPPFLAGS) -c -o 537mem.o 537malloc.c
	rm 537malloc.o -f
	ld -r -o 537malloc.o *.o

clean:
	rm -rf *.o $(EXE) $(SCAN_BUILD_DIR)

scan-build: clean
	scan-build -o $(SCAN_BUILD_DIR) make

scan-view: scan-build
	firefox -new-window $(SCAN_BUILD_DIR)/*/index.html

