# Table of contents

1. [General information](#general-information)
2. [Assembler](#assembler)

    1. [Assembler syntaxis](#assembler-syntaxis)

        1. [General rules](#general-rules)

        2. [List of allowed instructions](#list-of-allowed-instructions)

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

> In this section the top of the stack is reffered as **T** (top), an element of the stack that is first "under" the top is reffered as **PT** (pre-top).

1. **Work with stack**

    - **push**: pushes something in stack. There are 5 variants of **push**:

        * `push 123.456`: pushes 123.456 (generally: double number) in stack.

        * `push ax`: pushes number from register **ax** (generally: ax, bx, cx or dx) in stack.

        * `push [4]`: pushes number from RAM cell with index 4 (generally: positive integer number) in stack.

        * `push [ax]`: pushes number from RAM cell with index from register **ax** (generally: ax, bx, cx or dx) in stack.

        * `push [ax + 4]`: pushes number from RAM cell which index is 4 (generally: positive interger number) greater than a number form register 
        **ax** (generally: ax, bx, cx or dx) in stack.
    
    - **pop**: pops something from stack. There are 4 variants of **pop**:

        * `pop ax`: pops number from stack and puts it in register **ax** (generally: ax, bx, cx or dx).

        * `push [4]`: pops number from stack and puts it in RAM cell with index 4 (generally: positive integer number).

        * `push [ax]`: pops number from stack and puts it in RAM cell with index from register **ax** (generally: ax, bx, cx or dx).

        *  `push [ax + 4]`: pops number from stack and puts it in RAM cell which index is 4 (generally: positive interger number) greater than a number form register **ax** (generally: ax, bx, cx or dx).

2. **Jumps**

    - **jmp** (jump): makes jump to the label which name is "label_name".

    `jmp "label_name"`

    > Using any of next 6 jumps leads to popping 2 elements from stack.

    - **ja** (jump if above): makes jump to the label which name is "label_name" if **PT > T**.

    `ja "label_name"`

    - **jae** (jump if above or equal): makes jump to the label which name is "label_name" if **PT >= T**.

    `jae "label_name"`

    - **jb** (jump if below): makes jump to the label which name is "label_name" if **PT < T**.

    `jb "label_name"`

    - **jbe** (jump if below): makes jump to the label which name is "label_name" if **PT <= T**.

    `jbe "label_name"`

    - **je** (jump if equal): makes jump to the label which name is "label_name" if **PT == T**.

    `je "label_name"`

    - **jne** (jump if equal): makes jump to the label which name is "label_name" if **PT != T**.

    `jne "label_name"`

3. **Arithmetic operations**

    - **sqrt**: pops **T**, calculates quadratic root from **T** and pushes result in stack.

    > Processing any of 4 next arithmetic operations starts with popping **T** and **PT**.
    
    - **add**: calculates **PT + T** and pushes result in stack.

    `add`

    - **sub**: calculates **PT - T** and pushes result in stack.

    `sub`

    - **mul**: calculates **PT * T** and pushes result in stack.

    `mul`

    - **dvd**: calculates **PT / T** and pushes result in stack. If **T == 0**, processing stops with error.

    `dvd`

4. **Usage of functions**

    - **call**: saves ip of the next command in the call stack, then makes jump to the label which name is "label_name".

    `call "label_name"`

    - **ret**: pops element of the call stack than changes ip to poped value.

    `ret`

5. **Service instructions**

    - **hlt**: ends processing instructions.

    `hlt`

    - **in**: askes for a double number from stdin.

    `in`

    - **out**: puts a dobule number in stdout.

    `out`

    Message on the screen:

    `Popped number: 1.123457`

