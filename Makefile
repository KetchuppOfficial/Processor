CC = g++

CFLAGS = -c -Wall -Werror

LOG_LIB = ./My_Lib/My_Lib.a

all: Assembler Processor Disassembler

#RELATED TO ASSEMBLER
#*************************************************
ASM_OBJECTS = asm_main.o Lexer.o Parser.o
ASM_FILES = ./Assembler/asm_main.o ./Assembler/Lexer.o ./Assembler/Parser.o

ASM_FILE = ./Code/Write_Circle.txt

Assembler: $(ASM_OBJECTS)
	$(CC) -o ./Assembler/Assembler.out $(ASM_FILES) $(LOG_LIB)

asm_main.o: ./Assembler/main.cpp
	$(CC) $(CFLAGS) ./Assembler/main.cpp -o ./Assembler/asm_main.o

Lexer.o: ./Assembler/Lexer.cpp
	$(CC) $(CFLAGS) ./Assembler/Lexer.cpp -o ./Assembler/Lexer.o

Parser.o: ./Assembler/Parser.cpp
	$(CC) $(CFLAGS) ./Assembler/Parser.cpp -o ./Assembler/Parser.o

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
	$(CC) -o ./Processor/Processor.out $(PROC_FILES) $(LOG_LIB)

proc_main.o: ./Processor/main.cpp
	$(CC) $(CFLAGS) ./Processor/main.cpp -o ./Processor/proc_main.o

Processor.o: ./Processor/Processor.cpp
	$(CC) $(CFLAGS) ./Processor/Processor.cpp -o ./Processor/Processor.o

Stack.o: ./Processor/Stack/Stack.cpp
	$(CC) $(CFLAGS) ./Processor/Stack/Stack.cpp -o ./Processor/Stack/Stack.o

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
	$(CC) -o ./Disassembler/Disassembler.out $(DISASM_FILES) $(LOG_LIB)

disasm_main.o: ./Disassembler/main.cpp
	$(CC) $(CFLAGS) ./Disassembler/main.cpp -o ./Disassembler/disasm_main.o

Disassembler.o: ./Disassembler/Disassembler.cpp
	$(CC) $(CFLAGS) ./Disassembler/Disassembler.cpp -o ./Disassembler/Disassembler.o

run_disasm:
	./Disassembler/Disassembler.out

clean_disasm:
	rm $(DISASM_FILES)
	rm ./Disassembler/Disassembler.out
#*************************************************

clean_log:
	rm *.log
