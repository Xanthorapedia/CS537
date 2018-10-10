# author1: Dasong Gao
# author2: Haozhe Luo

CC = gcc
WARNING_FLAGS = -Wall -Wextra
EXTRA_FLAGS = -pthread -O2
CPPFLAGS = $(WARNING_FLAGS) $(EXTRA_FLAGS)
EXE = prodcomm
SCAN_BUILD_DIR = scan-build-out

all: $(EXE)

$(EXE): main.o queue.o worker.o
	$(CC) -lpthread  $(CPPFLAGS) -o $(EXE) *.o

clean:
	rm -rf *.o $(EXE) $(SCAN_BUILD_DIR)

scan-build: clean
	scan-build -o $(SCAN_BUILD_DIR) make

scan-view: scan-build
	firefox -new-window $(SCAN_BUILD_DIR)/*/index.html

