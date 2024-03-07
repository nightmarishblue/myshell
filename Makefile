TARGET := myshell

ENTRYPOINT := main
MAN_FILE := readme

SRC_DIR := src/
LIB_DIR := libraries/
HEAD_DIR := headers/
OUT_DIR := bin/
MAN_DIR := manual/

# build the shell in the out directory, including all necessary files
build: $(OUT_DIR)$(TARGET) $(OUT_DIR)$(MAN_FILE)

$(OUT_DIR)$(TARGET): $(SRC_DIR)* $(SRC_DIR)$(LIB_DIR)*.c $(SRC_DIR)$(HEAD_DIR)*.h | $(OUT_DIR)
	gcc -o "$(OUT_DIR)$(TARGET)" "$(SRC_DIR)$(LIB_DIR)"* "$(SRC_DIR)$(ENTRYPOINT).c" -Wall -Wpedantic

# make the out directory
make_out: $(OUT_DIR)

$(OUT_DIR):
	mkdir -p $(OUT_DIR)

# copy the manual into the out directory
move_manual: $(OUT_DIR)$(MAN_FILE)

$(OUT_DIR)$(MAN_FILE): | $(OUT_DIR)
	cp "$(MAN_DIR)$(MAN_FILE)" "$(OUT_DIR)"