CC = g++

CFLAGS = -c -Wall -Werror

all: Assembler Processor Disassembler

#RELATED TO ASSEMBLER
#*************************************************
ASM_OBJECTS = asm_main.o Lexer.o Parser.o Reading_File.o Asm_Log_File.o
ASM_FILES = ./Assembler/asm_main.o ./Assembler/Lexer.o ./Assembler/Parser.o Reading_File.o ./Assembler/Asm_Log_File.o

ASM_FILE = Quadratic_Equation.txt

Assembler: $(ASM_OBJECTS)
	$(CC) $(ASM_FILES) -o ./Assembler/Assembler.out

asm_main.o: ./Assembler/main.cpp
	$(CC) $(CFLAGS) ./Assembler/main.cpp -o ./Assembler/asm_main.o

Lexer.o: ./Assembler/Lexer.cpp
	$(CC) $(CFLAGS) ./Assembler/Lexer.cpp -o ./Assembler/Lexer.o

Parser.o: ./Assembler/Parser.cpp
	$(CC) $(CFLAGS) ./Assembler/Parser.cpp -o ./Assembler/Parser.o

Asm_Log_File.o: ./Assembler/Log_File.cpp
	$(CC) $(CFLAGS) ./Assembler/Log_File.cpp -o ./Assembler/Asm_Log_File.o

run_asm:
	./Assembler/Assembler.out $(ASM_FILE)

clean_asm:
	rm $(ASM_FILES)
	rm ./Assembler/Assembler.out
#*************************************************

#RELATED TO PROCESSOR
#*************************************************
PROC_OBJECTS = proc_main.o Processor.o Stack.o Reading_File.o Proc_Log_File.o
PROC_FILES = ./Processor/proc_main.o ./Processor/Processor.o ./Processor/Stack/Stack.o Reading_File.o ./Processor/Proc_Log_File.o

Processor: $(PROC_OBJECTS)
	$(CC) $(PROC_FILES) -o ./Processor/Processor.out

proc_main.o: ./Processor/main.cpp
	$(CC) $(CFLAGS) ./Processor/main.cpp -o ./Processor/proc_main.o

Processor.o: ./Processor/Processor.cpp
	$(CC) $(CFLAGS) ./Processor/Processor.cpp -o ./Processor/Processor.o

Stack.o: ./Processor/Stack/Stack.cpp
	$(CC) $(CFLAGS) ./Processor/Stack/Stack.cpp -o ./Processor/Stack/Stack.o

Proc_Log_File.o: ./Processor/Log_File.cpp
	$(CC) $(CFLAGS) ./Processor/Log_File.cpp -o ./Processor/Proc_Log_File.o

run_proc:
	./Processor/Processor.out

clean_proc:
	rm $(PROC_FILES)
	rm ./Processor/Processor.out
#*************************************************

#RELATED TO DISASSEMBLER
#*************************************************
DISASM_OBJECTS = disasm_main.o Disassembler.o Reading_File.o Disasm_Log_File.o
DISASM_FILES = ./Disassembler/disasm_main.o ./Disassembler/Disassembler.o Reading_File.o ./Disassembler/Disasm_Log_File.o

Disassembler: $(DISASM_OBJECTS)
	$(CC) $(DISASM_FILES) -o ./Disassembler/Disassembler.out

disasm_main.o: ./Disassembler/main.cpp
	$(CC) $(CFLAGS) ./Disassembler/main.cpp -o ./Disassembler/disasm_main.o

Disassembler.o: ./Disassembler/Disassembler.cpp
	$(CC) $(CFLAGS) ./Disassembler/Disassembler.cpp -o ./Disassembler/Disassembler.o

Disasm_Log_File.o: ./Disassembler/Log_File.cpp
	$(CC) $(CFLAGS) ./Disassembler/Log_File.cpp -o ./Disassembler/Disasm_Log_File.o

run_disasm:
	./Disassembler/Disassembler.out

clean_disasm:
	rm $(DISASM_FILES)
	rm ./Disassembler/Disassembler.out
#*************************************************

Reading_File.o: Reading_File.cpp
	$(CC) $(CFLAGS) Reading_File.cpp
