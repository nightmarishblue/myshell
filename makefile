# AUTHOR: Niall Bastible Díaz | 22314443 | niall.bastiblediaz2@mail.dcu.ie
# ACKNOWLEDGEMENTS
#     I understand that the University regards breaches of academic integrity and plagiarism as grave and serious.
# 
#     I have read and understood the DCU Academic Integrity and Plagiarism Policy.
#     I accept the penalties that may be imposed should I engage in practice or practices that breach this policy.
# 
#     I have identified and included the source of all facts, ideas, opinions and viewpoints of others in the assignment references.
#     Direct quotations, paraphrasing, discussion of ideas from books, journal articles, internet sources, module text, or any other
#     source whatsoever are acknowledged and the sources cited are identified in the assignment references.
# 
#     I declare that this material, which I now submit for assessment, is entirely my own work and has not been taken from the work of
#     others save and to the extent that such work has been cited and acknowledged within the text of my work.

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

$(OUT_DIR)$(MAN_FILE): $(MAN_DIR)$(MAN_FILE) | $(OUT_DIR)
	cp "$(MAN_DIR)$(MAN_FILE)" "$(OUT_DIR)"