#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define STACK_SIZE 4096
#define NUMLOCALS 256

#define HALT 0xff

uint32_t pc;
uint32_t* stack;
uint32_t* locals;
uint16_t sp;
unsigned char* prg;

void nop() {}
void vtaskdelay() {}
void iprint() { printf("0x%x\n", stack[sp]); }
void fprint() { printf("%lf\n", ((float*)stack)[sp]); }
void pop() {sp--;}
void ftoi() { stack[sp] = (uint32_t)*(float*)&stack[sp]; }
void itof() {
	float x = (float) stack[sp];
	stack[sp] = *(uint32_t*)&x;
}
void iadd() { stack[sp] = stack[sp--] + stack[sp]; }
void isub() { stack[sp] = stack[sp-1] - stack[sp--]; }
void imul() { stack[sp] = stack[sp--] * stack[sp]; }
void idiv() { stack[sp] = stack[sp-1] / stack[sp--]; }
void fadd() {
	float res = (((float*)stack)[sp--] + ((float*)stack)[sp]);
	stack[sp] = *(uint32_t*)&res;
}
void fsub() {
	float res = (((float*)stack)[sp-1] - ((float*)stack)[sp--]);
	stack[sp] = *(uint32_t*)&res;
}
void fmul() {
	float res = (((float*)stack)[sp--] * ((float*)stack)[sp]);
	stack[sp] = *(uint32_t*)&res;
}
void fdiv() {
	float res = (((float*)stack)[sp-1] / ((float*)stack)[sp--]);
	stack[sp] = *(uint32_t*)&res;
}
void pushi32() {
	stack[++sp] = *(uint32_t*)(prg + pc);
	pc += 4;
}
void pushi16() {
	stack[++sp] = (uint32_t)(*(uint16_t*)(prg + pc));
	pc += 2;
}
void swap() {
	uint32_t k = stack[sp];
	stack[sp] = stack[sp-1];
	stack[sp-1] = k;
}

void beq() {
	if(stack[sp] == stack[--sp]) {
		pc += *(int16_t*)(prg + pc); // add 2 for the 2 bytes for args
	}
	pc += 2;
}
void bgt() {
	if(stack[sp] > stack[--sp]) {
		pc += *(int16_t*)(prg + pc);
	}
	pc += 2;
}
void blt() {
	if(stack[sp] < stack[--sp]) {
		pc += *(int16_t*)(prg + pc);
	}
	pc += 2;
}
void bge() {
	if(stack[sp] >= stack[--sp]) {
		pc += *(int16_t*)(prg + pc);
	}
	pc += 2;
}
void ble() {
	if(stack[sp] <= stack[--sp]) {
		pc += *(int16_t*)(prg + pc);
	}
	pc += 2;
}
void bne() {
	if(stack[sp] != stack[--sp]) {
		pc += *(int16_t*)(prg + pc);
	}
	pc += 2;
}
void jmp() { pc += *(int16_t*)(prg + pc) + 2; }
void dup() { stack[++sp] = stack[sp]; }

void andb() { stack[sp] = stack[sp--] & stack[sp]; }
void orb() { stack[sp] = stack[sp--] | stack[sp]; }
void xorb() { stack[sp] = stack[sp--] ^ stack[sp]; }
void notb() { stack[sp] = ~stack[sp]; }

void store() { locals[*(unsigned char*)(prg+(pc++))] = stack[sp--]; }
void load() { stack[++sp] = locals[*(unsigned char*)(prg+(pc++))]; }

void pushpc() { stack[++sp] = pc; }
void writepc() { pc = stack[sp--]; }
void pushsp() { stack[++sp] = sp; }
void writesp() { sp = stack[sp--]; }
void jsr() { pushpc(); stack[sp]+=2; jmp(); }
void jsrs() { 
	uint32_t newpc = stack[sp--];
	pushpc();
	pc = newpc;
}
void pushab() { stack[sp] = prg[stack[sp]]; }
void pusha() { stack[sp] = *(uint16_t*)(prg+stack[sp]); }
void pushaw() { stack[sp] = *(uint32_t*)(prg+stack[sp]); }
void sprint() { printf("%s\n", (char*) prg+stack[sp]); }
void lsl() { stack[sp] = stack[sp-1] << stack[sp--]; }
void lsr() { stack[sp] = stack[sp-1] >> stack[sp--]; }

typedef void(*stdop)(void);
stdop ops[] = {
	nop, vtaskdelay, pushi32, pushi16, iprint, fprint, ftoi, itof,
	iadd, isub, imul, idiv, fadd, fsub, fmul, fdiv,
	pop, swap, beq, bgt, blt, bge, ble, bne, jmp, dup, andb, orb, xorb, notb,
	store, load, pushpc, writepc, pushsp, writesp, jsr, jsrs, pushab, pusha, pushaw,
	sprint, lsl, lsr
};

void run(long len) {
	pc = 0;
	stack = malloc(STACK_SIZE * sizeof(uint32_t));
	locals = malloc(NUMLOCALS * sizeof(uint32_t));
	sp = -1;
	while(pc < len && prg[pc] != HALT) {
		ops[prg[pc++]]();
	}
}

int main(int argc, char* argv[]) {
	if(argc!=2) {
		printf("not enough arguments! usage: \n%s [inputfile]\n", argv[0]);
		exit(1);
	}
	FILE* inputfile = fopen(argv[1], "r");
	if(inputfile == NULL) {
		printf("No such file %s!\n", argv[1]);
		exit(1);
	}
	fseek(inputfile, 0, SEEK_END);
	long filelen = ftell(inputfile);
	rewind(inputfile);
	char* buffer = (char*) malloc(filelen * sizeof(char));
	fread(buffer, filelen, 1, inputfile);
	fclose(inputfile);
	prg = buffer;
	run(filelen);
	return 0;
}