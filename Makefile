CFLAGS = -std=gnu99
objects = main.o myList.o myDirectoryApi.o myFileApi.o synchronizer.o logger.o myUtils.o config.o
# all: main

main: $(objects)
	gcc $(objects) -o mysync

main.o: synchronizer.h myUtils.h logger.h config.h
myDirectoryApi.o: myDirectoryApi.h myFileApi.h myList.h logger.h myUtils.h
myFileApi.o: myFileApi.h myList.h logger.h myUtils.h config.h
synchronizer.o: synchronizer.h myList.h myFileApi.h myDirectoryApi.h myUtils.h config.h logger.h
logger.o: logger.h
myUtils.o: myUtils.h
myList.o: myList.h
config.o: config.h

.PHONY: clean
clean:
	rm -f *.o mysync
