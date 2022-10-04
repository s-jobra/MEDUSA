#SOURCES:=custom-leaf-example.c
#OUTPUT_BINARY:=example

SOURCES:=my-example.c
OUTPUT_BINARY:=my-example

CC:=gcc
CFLAGS:=-g
CLIBS=-lgmp -lpthread -lm
INC_DIRS:=-I sylvan/src/ -I lace/src/ -I lace/build/

N_JOBS=4

example: $(SOURCES) sylvan/build/src/libsylvan.a lace/build/liblace.a
	gcc $(INC_DIRS) $(CFLAGS) -o $(OUTPUT_BINARY) $^ $(CLIBS)

download-sylvan:
	@git clone https://github.com/trolando/sylvan.git || true

sylvan/build/src/libsylvan.a: download-sylvan
	cd sylvan;			\
	mkdir build;		\
	cd build;			\
	cmake ..;			\
	make -j $(N_JOBS);			

download-lace:
	git clone https://github.com/trolando/lace.git || true

lace/build/liblace.a: download-lace
	cd lace;			\
	mkdir build;		\
	cd build;			\
	cmake ..;			\
	make -j $(N_JOBS);			

clean: clean-artifacts clean-deps

clean-artifacts:
	rm -f $(OUTPUT_BINARY)

clean-deps:
	rm -rf sylvan lace

.PHONY := clean clean-artifacts clean-deps
.DEFAULT := example
