SRC_DIR:=src
OBJ_DIR:=obj
BIN_DIR:=.
LIB_DIR:=lib

SRCS:=$(wildcard $(SRC_DIR)/*.c)
EXEC:=$(BIN_DIR)/sim
OBJS:=$(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

CC:=gcc
CFLAGS:=-g -O2
CLIBS=-lgmp -lpthread -lm
INC_DIRS:=-I $(LIB_DIR)/sylvan/src/ -I $(LIB_DIR)/lace/src/ -I $(LIB_DIR)/lace/build/

N_JOBS=4

F=bell
OF_TYPE=svg
F_OUT_NAME=res
T=BernsteinVazirani/01
BSCRIPT_PATH=benchmarks/scripts/
TEST_OUT=benchmark.out

.DEFAULT := all
.PHONY := clean clean-all clean-artifacts clean-deps clean-benchmark run install test benchmark plot plot-log \
          test-init install make-sylvan make-lace download-sylvan download-lace make-sliqsim get-benchmarks

all: $(OBJS) $(LIB_DIR)/sylvan/build/src/libsylvan.a $(LIB_DIR)/lace/build/liblace.a | $(BIN_DIR)
	$(CC) $(INC_DIRS) $(CFLAGS) -o $(EXEC) $^ $(CLIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(INC_DIRS) $(CFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

-include $(OBJ:.o=.d)

run-my:
	@./$(EXEC) ./examples/$(F).qasm
	@dot -T$(OF_TYPE) $(F_OUT_NAME).dot -o $(F_OUT_NAME).$(OF_TYPE)
	@rm $(F_OUT_NAME).dot

run-b:
	@./$(EXEC) ../benchmarks/$(T)/circuit.qasm
	@dot -T$(OF_TYPE) $(F_OUT_NAME).dot -o $(F_OUT_NAME).$(OF_TYPE)
	@rm $(F_OUT_NAME).dot

test:
	@bash ./$(BSCRIPT_PATH)/test-all.sh

plot:
	@cd ./$(BSCRIPT_PATH)/ && bash ./plot_log.sh

test-init: make-sliqsim get-benchmarks

make-sliqsim:
	cd .. &&\
	git clone https://github.com/NTU-ALComLab/SliQSim.git || true &&\
	cd SliQSim/cudd &&\
	./configure --enable-dddmp --enable-obj --enable-shared --enable-static &&\
	cd .. &&\
	make

get-benchmarks:
	cd .. && git clone https://github.com/alan23273850/AutoQ.git || true &&\
	mv AutoQ/benchmarks . && rm -rf AutoQ
	@bash ./$(BSCRIPT_PATH)/add-measure.sh

# INIT:
install-deps: make-sylvan make-lace
	mkdir $(LIB_DIR) && mv sylvan $(LIB_DIR) && mv lace $(LIB_DIR)

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

clean-all: clean-artifacts clean-deps clean-benchmark

clean-artifacts:
	rm -rf $(BIN_DIR)/$(EXEC) $(F_OUT_NAME).dot $(F_OUT_NAME).$(OF_TYPE) $(OBJ_DIR)

clean-deps:
	rm -rf $(LIB_DIR)

clean-benchmark:
	cd .. && rm -rf benchmarks SliQSim