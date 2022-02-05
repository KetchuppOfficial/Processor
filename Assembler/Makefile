CC=g++

CFLAGS=-c -Wall -Werror

ASM_FILE=../Factorial.txt

all: Assembler

Assembler: main.o Lexer.o Parser.o Reading_File.o Log_File.o
	$(CC) main.o Lexer.o Parser.o Reading_File.o Log_File.o -o Assembler

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

Lexer.o: Lexer.cpp
	$(CC) $(CFLAGS) Lexer.cpp

Parser.o: Parser.cpp
	$(CC) $(CFLAGS) Parser.cpp

Reading_File.o: ../Reading_File.cpp
	$(CC) $(CFLAGS) ../Reading_File.cpp

Log_File.o: ../Log_File.cpp
	$(CC) $(CFLAGS) ../Log_File.cpp

run:
	./Assembler $(ASM_FILE)

clean:
	rm -rf *.o
	rm Assembler