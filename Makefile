BUILD_DIR ?= ./build
SRC_DIR = ./src
TEST_DIR = ./test
INC_DIR = $(SRC_DIR)

BITSHUFFLE=./bitshuffle/src
BITSHUFFLE_LZ4=./bitshuffle/lz4
CJSON=./cJSON

CFLAGS=-Wall -O2 -fpic -I$(INC_DIR) -I$(CJSON) -I$(BITSHUFFLE) -I$(BITSHUFFLE_LZ4) -std=c99

.PHONY: plugin
plugin: $(BUILD_DIR)/ramdisk-plugin.so

.PHONY: test
test: $(BUILD_DIR)/test

.PHONY: all
all: plugin test

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BITSHUFFLE)/%.o: $(BITSHUFFLE)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BITSHUFFLE_LZ4)/%.o: $(BITSHUFFLE_LZ4)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(CJSON)/%.o: $(CJSON)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TEST_DIR)/%.o: $(TEST_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/ramdisk-plugin.so: $(BUILD_DIR)/plugin.o $(CJSON)/cJSON.o $(BITSHUFFLE_LZ4)/lz4.o \
$(BITSHUFFLE)/bitshuffle.o $(BITSHUFFLE)/bitshuffle_core.o $(BITSHUFFLE)/iochain.o
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -shared $^ -o $(BUILD_DIR)/ramdisk-plugin.so

$(BUILD_DIR)/test: $(TEST_DIR)/test.o $(BUILD_DIR)/plugin.o $(CJSON)/cJSON.o $(BITSHUFFLE_LZ4)/lz4.o \
$(BITSHUFFLE)/bitshuffle.o $(BITSHUFFLE)/bitshuffle_core.o $(BITSHUFFLE)/iochain.o
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $^ -o $(BUILD_DIR)/test

.PHONY: clean
clean:
	rm -r $(BUILD_DIR)
