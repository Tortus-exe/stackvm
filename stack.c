#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define STACK_SIZE 4096

uint32_t pc;
uint32_t* stack;
uint16_t sp;

void nop() {}
void vtaskdelay() {}
void iprint() {
	printf("0x%x\n", stack[sp--]);
}
void fprint() {
	printf("%lf\n", ((float*)stack)[sp--]);
}
void pop() {sp--;}
void ftoi() {
	stack[sp] = (uint32_t)*(float*)&stack[sp];
}
void itof() {
	float x = (float) stack[sp];
	stack[sp] = *(uint32_t*)&x;
}
void iadd() {
	stack[sp] = stack[sp--] + stack[sp];
}
void isub() {
	stack[sp] = stack[sp-1] - stack[sp--];
}
void imul() {
	stack[sp] = stack[sp--] * stack[sp];
}
void idiv() {
	stack[sp] = stack[sp-1] / stack[sp--];
}
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
void pushi(uint32_t val) {
	stack[++sp] = val;
}
void swap() {
	uint32_t k = stack[sp];
	stack[sp] = stack[sp-1];
	stack[sp-1] = k;
}

typedef void(*stdop)(void);
stdop ops[] = {
	nop, vtaskdelay, nop, nop, iprint, fprint, ftoi, itof,
	iadd, isub, imul, idiv, fadd, fsub, fmul, fdiv,
	pop, swap
};

void run(char* prg, long len) {
	pc = 0;
	stack = malloc(STACK_SIZE * sizeof(uint32_t));
	sp = -1;
	while(pc < len) {
		unsigned char x = prg[pc++];
		switch(x) {
			case 0x02: 
				pushi(*(uint32_t*)(prg + pc));
				pc += 4;
				break;
			case 0x03:
				pushi((uint32_t)(*(uint16_t*)(prg + pc)));
				pc += 2;
				break;
			default:
				ops[x]();
				break;
		}
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
	run(buffer, filelen);
	return 0;
}