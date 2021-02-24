cc = g++

objs = ${patsubst %.cpp, %.o, $(wildcard *.cpp)}

.PHONY: all
all: FileSystem

FileSystem: $(objs)
	$(cc) $(objs) -o main

$(objs): %.o: %.cpp %.h utils.h
	$(cc) -c $< -o $@

.PHONY: clean
clean:
	del *.o sfs.exe disk -rf
