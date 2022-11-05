SOURCES:=simulator.c custom_mtbdd.c gates.c
HEADER_FILES:=custom_mtbdd.h gates.h
OUTPUT_BINARY:=sim

CC:=gcc
CFLAGS:=-g
CLIBS=-lgmp -lpthread -lm
INC_DIRS:=-I sylvan/src/ -I lace/src/ -I lace/build/

N_JOBS=4

.DEFAULT := all
.PHONY := clean clean-artifacts clean-deps example

#TODO: deps

all: $(SOURCES) $(HEADER_FILES) sylvan/build/src/libsylvan.a lace/build/liblace.a
	gcc $(INC_DIRS) $(CFLAGS) -o $(OUTPUT_BINARY) $^ $(CLIBS)

run:
	$(MAKE)
	@./$(OUTPUT_BINARY) test.qasm
	@dot -Tjpeg orig.dot -o orig.jpeg
	@dot -Tjpeg res.dot -o res.jpeg
	@rm *.dot

clean: clean-artifacts #clean-deps

clean-artifacts:
	rm -f $(OUTPUT_BINARY) *.dot *.jpeg out.txt

clean-deps:
	rm -rf sylvan lace

#test:
example: my-example.c custom_mtbdd.c gates.c $(HEADER_FILES) sylvan/build/src/libsylvan.a lace/build/liblace.a
	gcc $(INC_DIRS) $(CFLAGS) -o my-example $^ $(CLIBS)

#test:
#	$(MAKE) test_example
#	@./my-example
#	@dot -Tjpeg to.dot -o torig.jpeg
#	@dot -Tjpeg tn.dot -o tres.jpeg
#	@rm *.dot

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


