CC     = gcc
CFLAGS = -Wall -Werror -Wshadow -Wfloat-equal -Wswitch-default

BIN      = ./bin/
SRCDIR   = ./src/
BUILDDIR = ./build/

SRC_LIST = main.c Lexer.c Parser.c
SRC = $(addprefix $(SRCDIR), $(SRC_LIST))

SUBS := $(SRC)
SUBS := $(subst $(SRCDIR), $(BUILDDIR), $(SUBS))

OBJ  = $(SUBS:.c=.o)
DEPS = $(SUBS:.c=.d)

.PHONY: all

all: $(DEPS) $(OBJ)
	@mkdir -p $(BIN)
	@echo "Linking project..."
	@$(CC) $(OBJ) -o $(BIN)Assembler.out

$(BUILDDIR)%.o: $(SRCDIR)%.c
	@mkdir -p $(dir $@)
	@echo "Compiling \"$<\"..."
	@$(CC) $(CFLAGS) -g $(OPT) -c $< -o $@

include $(DEPS)

$(BUILDDIR)%.d: $(SRCDIR)%.c
	@echo "Collecting dependencies for \"$<\"..."
	@mkdir -p $(dir $@)
	@$(CC) -E $(CFLAGS) $< -MM -MT $(@:.d=.o) > $@

.PHONY: run clean

clean:
	rm -rf $(OBJ) $(DEPS)

run: $(BIN)Assembler.out
	@echo "Running \"$<\"..."
	@$(BIN)Assembler.out $(IN)
