SOURCES:= *.c
HEADER_FILES:= *.h
OUTPUT_BINARY:=sim

CC:=gcc
CFLAGS:=-g -O2
CLIBS=-lgmp -lpthread -lm
INC_DIRS:=-I sylvan/src/ -I lace/src/ -I lace/build/

N_JOBS=4

F=bell-state.qasm
OF_TYPE=svg
F_OUT_NAME=res

.DEFAULT := all
.PHONY := clean clean-all clean-artifacts clean-deps run install

#TODO: deps + srcs

all: $(SOURCES) $(HEADER_FILES) sylvan/build/src/libsylvan.a lace/build/liblace.a
	gcc $(INC_DIRS) $(CFLAGS) -o $(OUTPUT_BINARY) $^ $(CLIBS)

run:
	$(MAKE)
	@./$(OUTPUT_BINARY) ./examples/$(F)
	@dot -T$(OF_TYPE) $(F_OUT_NAME).dot -o $(F_OUT_NAME).$(OF_TYPE)
	@rm $(F_OUT_NAME).dot

test:
	@bash ./test.sh

benchmark:
	@bash ./test-all.sh > test_new.out

test-init:
	cd .. &&\
	git clone https://github.com/NTU-ALComLab/SliQSim.git || true &&\
	cd SliQSim/cudd &&\
	./configure --enable-dddmp --enable-obj --enable-shared --enable-static &&\
	cd .. &&\
	make

# INIT:
install: make-sylvan make-lace

make-sylvan: download-sylvan
	cd sylvan;			\
	mkdir build;		\
	cd build;			\
	cmake ..;			\
	make -j $(N_JOBS);

make-lace: download-lace
	cd lace;			\
	mkdir build;		\
	cd build;			\
	cmake ..;			\
	make -j $(N_JOBS);

download-sylvan:
	@git clone https://github.com/trolando/sylvan.git || true

download-lace:
	@git clone https://github.com/trolando/lace.git || true

# CLEAN:
clean: clean-artifacts

clean-all: clean-artifacts clean-deps

clean-artifacts:
	rm -f $(OUTPUT_BINARY) *.dot $(F_OUT_NAME).$(OF_TYPE)

clean-deps:
	rm -rf sylvan lace

# OLD:
########
#test:
#example: my-example.c custom_mtbdd.c gates.c $(HEADER_FILES) sylvan/build/src/libsylvan.a lace/build/liblace.a
#	gcc $(INC_DIRS) $(CFLAGS) -o my-example $^ $(CLIBS)

#test:
#	$(MAKE) test_example
#	@./my-example
#	@dot -Tjpeg to.dot -o torig.jpeg
#	@dot -Tjpeg tn.dot -o tres.jpeg
#	@rm *.dot
