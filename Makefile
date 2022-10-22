SOURCES:=my-example.c custom_mtbdd.c gates.c
HEADER_FILES:=custom_mtbdd.h gates.h
OUTPUT_BINARY:=my-example

CC:=gcc
CFLAGS:=-g
CLIBS=-lgmp -lpthread -lm
INC_DIRS:=-I sylvan/src/ -I lace/src/ -I lace/build/

N_JOBS=4

.DEFAULT := example
.PHONY := clean clean-artifacts clean-deps

#TODO: graphviz + deps

example: $(SOURCES) $(HEADER_FILES) sylvan/build/src/libsylvan.a lace/build/liblace.a
	gcc $(INC_DIRS) $(CFLAGS) -o $(OUTPUT_BINARY) $^ $(CLIBS)

clean: clean-artifacts clean-deps

clean-artifacts:
	rm -f $(OUTPUT_BINARY)

clean-deps:
	rm -rf sylvan lace

#################################
#download-sylvan:
#	@git clone https://github.com/trolando/sylvan.git || true

#sylvan/build/src/libsylvan.a: download-sylvan
#	cd sylvan;			\
	mkdir build;		\
	cd build;			\
	cmake ..;			\
	make -j $(N_JOBS);

#download-lace:
#	git clone https://github.com/trolando/lace.git || true

#lace/build/liblace.a: download-lace
#	cd lace;			\
	mkdir build;		\
	cd build;			\
	cmake ..;			\
	make -j $(N_JOBS);


