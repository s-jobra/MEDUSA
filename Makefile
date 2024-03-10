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

OF_TYPE=svg
F_OUT_NAME=res
BSCRIPT_PATH=benchmark-utils/scripts

.DEFAULT : all
.PHONY : clean clean-all clean-artifacts clean-deps clean-benchmark plot benchmarks results-plot \
           install-deps make-sylvan make-lace download-sylvan download-lace make-sliqsim

all: $(OBJS) $(LIB_DIR)/sylvan/build/src/lib/libsylvan.a $(LIB_DIR)/lace/build/lib/liblace.a | $(BIN_DIR)
	$(CC) $(INC_DIRS) $(CFLAGS) -o $(EXEC) $^ $(CLIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(INC_DIRS) $(CFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

-include $(OBJ:.o=.d)

plot:
	@dot -T$(OF_TYPE) $(F_OUT_NAME).dot -o $(F_OUT_NAME).$(OF_TYPE)

benchmarks:
	@bash ./$(BSCRIPT_PATH)/run-benchmarks.sh

results-plot:
	@cd ./$(BSCRIPT_PATH)/ && bash ./plot_log.sh

# BENCHMARK INIT:
make-sliqsim:
	cd .. &&\
	git clone https://github.com/NTU-ALComLab/SliQSim.git || true &&\
	cd SliQSim/cudd &&\
	./configure --enable-dddmp --enable-obj --enable-shared --enable-static &&\
	cd .. &&\
	make

# INIT:
install-deps: make-sylvan make-lace
	mkdir $(LIB_DIR) && mv sylvan lace $(LIB_DIR)

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
	rm -rf $(EXEC) $(F_OUT_NAME).dot $(F_OUT_NAME).$(OF_TYPE) $(OBJ_DIR)

clean-deps:
	rm -rf $(LIB_DIR)

clean-benchmark:
	cd .. && rm -rf SliQSim