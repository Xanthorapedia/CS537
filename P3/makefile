# author1: Dasong Gao
# author2: Haozhe Luo

CC = gcc
WARNING_FLAGS = -Wall -Wextra
EXTRA_FLAGS = -O0 -g
CPPFLAGS = $(WARNING_FLAGS) $(EXTRA_FLAGS)
EXE = 537make
SCAN_BUILD_DIR = scan-build-out

all: $(EXE)

$(EXE): main.o parser.o utils.o autoarr.o goal.o
	$(CC) $(CPPFLAGS) -o $(EXE) *.o

clean:
	rm -rf *.o $(EXE) $(SCAN_BUILD_DIR)

scan-build: clean
	scan-build -o $(SCAN_BUILD_DIR) make

scan-view: scan-build
	firefox -new-window $(SCAN_BUILD_DIR)/*/index.html

