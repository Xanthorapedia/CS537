# author1: Dasong Gao
# author2: Haozhe Luo

CC = gcc
WARNING_FLAGS = -Wall -Wextra
EXTRA_FLAGS = -O0 -g
CPPFLAGS = $(WARNING_FLAGS) $(EXTRA_FLAGS)
SCAN_BUILD_DIR = scan-build-out
TESTS := $(shell ls *.c -1 | sed -e 's/\.c/.out/')

all: 537malloch $(TESTS)
	rm ./537malloc.h -f

537malloch:
	cp ../537malloc.h .

%.o: %.c
	$(CC) $(CPPFLAGS)  $< -c

%.out: %.o ../537malloc.o
	$(CC) $(CPPFLAGS)  $< ../537malloc.o -o $@

clean:
	rm -rf *test*.o *.out $(SCAN_BUILD_DIR)

scan-build: clean
	scan-build -o $(SCAN_BUILD_DIR) make

scan-view: scan-build
	firefox -new-window $(SCAN_BUILD_DIR)/*/index.html

