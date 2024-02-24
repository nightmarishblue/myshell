TARGET := my-shell
OUT_DIR := out/
ENTRYPOINT := main.c

build: $(ENTRYPOINT) | $(OUT_DIR)
	gcc -o "$(OUT_DIR)$(TARGET)" "$(ENTRYPOINT)"

$(OUT_DIR):
	mkdir -p $(OUT_DIR)