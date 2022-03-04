# General information

This project is called "Processor". It includes 3 programs: **Assembler**, **Processor** and **Disassembler**.

**Assembler** receives a .txt file with instructions on my own assembler language. The result of its work is a .bin file with these instructions translated into the binary format.

**Disassembler** does exactly the opposite to what **Assembler** does.

**Processor** receives a .bin file from **Assembler**. Then it process the content.

# Assembler

## Assembler syntaxis

### General rules

1. Each insruction has to be written on a new line.

2. An instruction can be prefaced with a number of spaces and/or tab characters.

3. If an instruction has arguments, they can be separeted from the instruction and from each other by a number of spaces and/or tab characters.

4. Empty lines are allowed.

5. All characters from a semicolon (';') to the line ending charachter ('\n') are interpreted as a comment and ignrored.

6. Label is any sequence of charachers preceding the colon. Label and the colon have to be only character on the line.

### List of allowed instructions

> [!NOTE]
> In this section the top of the stack is reffered as **T** (top), an element of the stack that is first "under" the top is reffered as **PT** (pre-top)

1. Jumps

    1. jmp (jump): makes jump to the label which name is "label_name".

    `jmp "label_name"`

    2. ja (jump if above): makes jump to the label which name is "label_name" if **PT** > **T**.

    `ja "label_name"`

    3. jae (jump if above or equal): makes jump to the label which name is "label_name" if **PT** >= **T**.

    `jae "label_name"`

    4. jb (jump if below): makes jump to the label which name is "label_name" if **PT** < **T**.

    `jb "label_name"`

    5. jbe (jump if below): makes jump to the label which name is "label_name" if **PT** <= **T**.

    `jbe "label_name"`

    6. je (jump if equal): makes jump to the label which name is "label_name" if **PT** == **T**.

    `je "label_name"`

    7. jne (jump if equal): makes jump to the label which name is "label_name" if **PT** != **T**.

    `jne "label_name"`