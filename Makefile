TARGET := myshell

ENTRYPOINT := main.c

SRC_DIR := src/
LIB_DIR := libraries/
OUT_DIR := bin/

build: $(SRC_DIR)$(ENTRYPOINT) | $(OUT_DIR)
	gcc -o "$(OUT_DIR)$(TARGET)" $(SRC_DIR)$(LIB_DIR)* "$(SRC_DIR)$(ENTRYPOINT)"

$(OUT_DIR):
	mkdir -p $(OUT_DIR)