CFLAGS = -std=gnu99
objects = main.o myList.o directoryFunctions.o synchronizer.o logger.o
all: main

main: $(objects)
	gcc $(objects) -o main

$(objects): myList.h
main.o: directoryFunctions.h synchronizer.h
directoryFunctions.o: directoryFunctions.h logger.h
synchronizer.o: synchronizer.h directoryFunctions.h
logger.o: logger.h

.PHONY: clean
clean:
	rm -f *.o main
