CFLAGS = -std=gnu99
objects = main.o myList.o myDirectoryApi.o myFileApi.o synchronizer.o logger.o myUtils.o config.o
all: main

main: $(objects)
	gcc $(objects) -o main

$(objects): myList.h
main.o: synchronizer.h myUtils.h logger.h config.h
myDirectoryApi.o: myDirectoryApi.h logger.h myUtils.h
myFileApi.o: myFileApi.h logger.h myUtils.h
synchronizer.o: synchronizer.h myFileApi.h myDirectoryApi.h config.h
logger.o: logger.h
myUtils.o: myUtils.h
config.o: config.h

.PHONY: clean
clean:
	rm -f *.o main
