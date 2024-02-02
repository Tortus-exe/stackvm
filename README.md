# MINI STACK MACHINE #

This is a toy stack-based VM for embedded firmware development.
Created because, well, I was bored. Also because I needed one.

### ISA ###

|opcode|name|args|description|
|---|---|---|---|
|0x00|NOP|none|stalls the VM|
|0x01|sleep|none|currently unimplemented.
|0x02|pushi32|4 bytes of 32-bit integer. endianness is system-dependent.|Pushes a 32-bit integer to the top of the stack.
|0x03|pushi16|2 bytes of 16-bit integer. endianness is system-dependent.|Pushes a 32-bit integer to the top of the stack with the high two bytes set to 0|
|0x04|iprint|none|prints the element on the top of the stack as an integer
|0x05|fprint|none|prints the element on the top of the stack as a float
|0x06|ftoi|none|casts the element on the top of the stack from a float to an integer representation.
|0x07|itof|none|casts the element on the top of the stack from an integer to a float representation.
|0x08|iadd|none|adds the top two elements on the stack and pushes the result to the stack. Operands are popped.
|0x09|isub|none|subtracts the element on the top of the stack from the 2nd topmost element on the stack, and pushes the result. Operands are popped.|
|0x0A|imul|none|multiplies the top two elements on the stack and pushes the result.
|0x0B|idiv|none|pushes the result of the second-topmost element on the stack divided by the topmost element on the stack, popping the operands.
|0x0C|fadd|none|iadd but for floats instead of integers.
|0x0D|fsub|none|isub but for floats instead of integers.
|0x0E|fmul|none|imul but for floats instead of integers.
|0x0F|fdiv|none|idiv but for floats instead of integers.
|0x10|pop|none|pops the top element off the stack and throws it into the void, never to be seen again.
|0x11|swap|none|swaps the top two elements on the stack.
|0x12|beq|2 bytes of 16-bit relative-offset to jump the PC to.|branches if the top two elements on the stack are equal to each other.
|0x13|bgt|2 bytes of 16-bit relative-offset to jump the PC to.|branches if the top element is strictly greater than the second-topmost element.
|0x14|blt|2 bytes of 16-bit relative-offset to jump the PC to.|bacon lettuce tomato (branches if the topmost element is strictly less than the second-topmost element)
|0x15|bge|2 bytes of 16-bit relative-offset to jump the PC to.|branches if the topmost element is greater than or equal to the second topmost element.
|0x16|ble|2 bytes of 16-bit relative-offset to jump the PC to.|branches if the topmost element is less than or equal to the second topmost element.
|0x17|bne|2 bytes of 16-bit relative-offset to jump the PC to.|branches if the topmost element and the second-topmost element are different.
|0x18|jmp|2 bytes of 16-bit relative-offset to jump the PC to.|unconditionally branches.
|0x19|dup|none|duplicates the top element on the stack and pushes it.
|0x1A|andb|none|performs a bitwise and operation on the two topmost elements on the stack. Operands are popped and result is pushed.
|0x1B|orb|none|performs a bitwise or operation on the two topmost elements on the stack. Operands are popped and result is pushed.
|0x1C|xorb|none|performs a bitwise XOR operation on the two topmost elements on the stack. Operands are popped and result is pushed.
|0x1D|notb|none|inverts the topmost element on the stack.
|0x1E|store|1 byte indicating which of 256 local variables to save to|pops the top element off the stack and writes it to the argument local variable|
|0x1F|load|1 byte indicating which of 256 local variables to save to|reads the argument local variable and pushes its value to the stack.
|0x20|pushpc|none|pushes the PC to the stack.|
|0x21|writepc|none|pops the topmost value off the stack and writes the value to the PC|
|0x22|pushsp|none|pushes the current stack pointer to the stack.|
|0x23|writesp|none|pops the topmost value off the stack and writes the value to the stack pointer.|
|0x24|jsr|address of the subroutine to jump to|pushes address of the next instruction to the stack and sets the PC to the argument value.|
|0x25|jsrs|none|pops the new pc value off the stack, psuhes the next instruction address to the stack, and jumps to the new pc value.|
|0x26|pushab|none|dereference the value at the top of the stack, and push it to the stack as a single byte.|
|0x27|pusha|none|dereference the value at the top of the stack, and push it to the stack as a 16-bit word.
|0x28|pushaw|none|dereference the value at the top of the stack, and push it to the stack as a 32-bit word.|
|0x29|sprint|none|prints the value at the top of the stack as a pointer to a string|
|0x2A|lsl|none|shifts the second-topmost value on the stack as many places left as indicated by the top-most value of the stack.|
|0x2B|lsr|none|shifts the second-topmost value on the stack as many places right as indicated by the top-most value of the stack.|
|0x2C|mod|none|pushes the result of the second-topmost value on the stack modulo the top-most value.|
|0x2D|pushi8|none|pushes an 8-bit unsigned integer to the stack.|
|0x2E|neg|none|negates the top-most value on the stack as an integer.|
|0x2F|negf|none|negates the top-most value on the stack as a float.|
|0x30|c0|none|pushes unsigned integer 0 to the top of the stack.|
|0x31|c1|none|pushes unsigned integer 1 to the top of the stack.|
|0x32|c2|none|pushes unsigned integer 2 to the top of the stack.|
|0x33|c3|none|pushes unsigned integer 3 to the top of the stack.|
|0x34|c4|none|pushes unsigned integer 4 to the top of the stack.|
|0x35|c5|none|pushes unsigned integer 5 to the top of the stack.|
|0x36|c6|none|pushes unsigned integer 6 to the top of the stack.|
|0x37|c7|none|pushes unsigned integer 7 to the top of the stack.|
|0x38|malloca|none|calls malloc to allocate as many bytes as indicated by the topmost value on the stack. The operand is NOT POPPED.|
|0x39|freea|none|calls free to free any allocated space made by a malloc call, operand at the top of the stack. The operand is popped.|
|0x3A|indexab|none|indexes an absolute pointer that has been malloc'd and pushes it as an 8-bit unsigned byte to the top of the stack. pointer is given on the second-topmost value, and index is given on the topmost value. Operands are not popped.
|0x3B|indexa|none|indexes an absolute pointer that has been malloc'd and pushes it as an 16-bit unsigned byte to the top of the stack. pointer is given on the second-topmost value, and index is given on the topmost value. Operands are not popped.
|0x3C|indexaw|none|indexes an absolute pointer that has been malloc'd and pushes it as an 32-bit unsigned byte to the top of the stack. pointer is given on the second-topmost value, and index is given on the topmost value. Operands are not popped.
|0x3D|storea|none|writes the topmost value on the stack to a pointer to an array pointer and an index given in the second- and third-topmost values on the stack.
|0x3E|store0|none|writes the topmost value on the stack to the 0th local value.
|0x3F|store1|none|writes the topmost value on the stack to the 1st local value.
|0x40|store2|none|writes the topmost value on the stack to the 2nd local value.
|0x41|store3|none|writes the topmost value on the stack to the 3rd local value.
|0x42|store4|none|writes the topmost value on the stack to the 4th local value.
|0x43|load0|none|pushes the 0th local value to the stack.
|0x44|load1|none|pushes the 1st local value to the stack.
|0x45|load2|none|pushes the 2nd local value to the stack.
|0x46|load3|none|pushes the 3rd local value to the stack.
|0x47|load4|none|pushes the 4th local value to the stack.
