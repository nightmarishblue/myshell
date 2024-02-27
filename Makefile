TARGET := myshell

ENTRYPOINT := main.c

LIB_DIR := libraries/
OUT_DIR := out/

build: $(ENTRYPOINT) | $(OUT_DIR)
	gcc -o "$(OUT_DIR)$(TARGET)" $(LIB_DIR)* "$(ENTRYPOINT)"

$(OUT_DIR):
	mkdir -p $(OUT_DIR)