CC = gcc -std=c17

CFLAGS = -c -Wall -Werror

LOG_LIB = ./My_Lib/My_Lib.a

all: Assembler Processor Disassembler

#RELATED TO ASSEMBLER
#*************************************************
ASM_OBJECTS = asm_main.o Lexer.o Parser.o
ASM_FILES = ./Assembler/asm_main.o ./Assembler/Lexer.o ./Assembler/Parser.o

ASM_FILE = ./Code/Quadratic_Equation.txt

Assembler: $(ASM_OBJECTS)
	$(CC) $(ASM_FILES) $(LOG_LIB) -o ./Assembler/Assembler.out -lm

asm_main.o: ./Assembler/main.c
	$(CC) $(CFLAGS) ./Assembler/main.c -o ./Assembler/asm_main.o

Lexer.o: ./Assembler/Lexer.c
	$(CC) $(CFLAGS) ./Assembler/Lexer.c -o ./Assembler/Lexer.o

Parser.o: ./Assembler/Parser.c
	$(CC) $(CFLAGS) ./Assembler/Parser.c -o ./Assembler/Parser.o

run_asm:
	./Assembler/Assembler.out $(ASM_FILE)

clean_asm:
	rm $(ASM_FILES)
	rm ./Assembler/Assembler.out
#*************************************************

#RELATED TO PROCESSOR
#*************************************************
PROC_OBJECTS = proc_main.o Processor.o Stack.o
PROC_FILES = ./Processor/proc_main.o ./Processor/Processor.o ./Processor/Stack/Stack.o

Processor: $(PROC_OBJECTS)
	$(CC) $(PROC_FILES) $(LOG_LIB) -o ./Processor/Processor.out -lm

proc_main.o: ./Processor/main.c
	$(CC) $(CFLAGS) ./Processor/main.c -o ./Processor/proc_main.o

Processor.o: ./Processor/Processor.c
	$(CC) $(CFLAGS) ./Processor/Processor.c -o ./Processor/Processor.o

Stack.o: ./Processor/Stack/Stack.c
	$(CC) $(CFLAGS) ./Processor/Stack/Stack.c -o ./Processor/Stack/Stack.o

run_proc:
	./Processor/Processor.out

clean_proc:
	rm $(PROC_FILES)
	rm ./Processor/Processor.out
#*************************************************

#RELATED TO DISASSEMBLER
#*************************************************
DISASM_OBJECTS = disasm_main.o Disassembler.o
DISASM_FILES = ./Disassembler/disasm_main.o ./Disassembler/Disassembler.o

Disassembler: $(DISASM_OBJECTS)
	$(CC) $(DISASM_FILES) $(LOG_LIB) -o ./Disassembler/Disassembler.out

disasm_main.o: ./Disassembler/main.c
	$(CC) $(CFLAGS) ./Disassembler/main.c -o ./Disassembler/disasm_main.o

Disassembler.o: ./Disassembler/Disassembler.c
	$(CC) $(CFLAGS) ./Disassembler/Disassembler.c -o ./Disassembler/Disassembler.o

run_disasm:
	./Disassembler/Disassembler.out

clean_disasm:
	rm $(DISASM_FILES)
	rm ./Disassembler/Disassembler.out
#*************************************************

clean_log:
	rm *.log
