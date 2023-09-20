#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/mman.h>

#define STACK_SIZE 4096
#define NUMLOCALS 256

#define HALT 0xff

uint32_t pc;
uint32_t* stack;
uint32_t* locals;
uint16_t sp;
unsigned char* prg;

void run() {
	const static void* ops[] = {
		&&nop, &&sleep, &&pushi32, &&pushi16, &&iprint, &&fprint, &&ftoi, &&itof,
		&&iadd, &&isub, &&imul, &&idiv, &&fadd, &&fsub, &&fmul, &&fdiv,
		&&pop, &&swap, &&beq, &&bgt, &&blt, &&bge, &&ble, &&bne, &&jmp, &&dup, &&andb, &&orb, &&xorb, &&notb,
		&&store, &&load, &&pushpc, &&writepc, &&pushsp, &&writesp, &&jsr, &&jsrs, &&pushab, &&pusha, &&pushaw,
		&&sprint, &&lsl, &&lsr, &&mod, &&pushi8, &&neg, &&negf, 
		&&c0, &&c1, &&c2, &&c3, &&c4, &&c5, &&c6, &&c7, &&malloca, &&freea, &&indexab, &&indexa, &&indexaw, &&storea,
		&&store0, &&store1, &&store2, &&store3, &&store4, &&load0, &&load1, &&load2, &&load3, &&load4, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
		&&halt
	};

	pc = 0;
	stack = malloc(STACK_SIZE * sizeof(uint32_t));
	locals = malloc(NUMLOCALS * sizeof(uint32_t));
	sp = -1;
	float res;

	#define NEXT() goto *ops[prg[pc++]]
nop:
sleep:
	NEXT();
iprint:
	printf("0x%x\n", stack[sp]);
	NEXT();
fprint:
	printf("%lf\n", ((float*)stack)[sp]);
	NEXT();
pop: 
	sp--;
	NEXT();
ftoi: 
	stack[sp] = (uint32_t)*(float*)&stack[sp]; 
	NEXT();
itof: 
	float x = (float) stack[sp];
	stack[sp] = *(uint32_t*)&x; 
	NEXT();
iadd: 
	stack[sp] = stack[sp--] + stack[sp]; 
	NEXT();
isub: 
	stack[sp] = stack[sp-1] - stack[sp--]; 
	NEXT();
imul: 
	stack[sp] = stack[sp--] * stack[sp]; 
	NEXT();
idiv: 
	stack[sp] = stack[sp-1] / stack[sp--]; 
	NEXT();
fadd:
	res = (((float*)stack)[sp--] + ((float*)stack)[sp]);
	stack[sp] = *(uint32_t*)&res;
	NEXT();
fsub:
	res = (((float*)stack)[sp-1] - ((float*)stack)[sp--]);
	stack[sp] = *(uint32_t*)&res;
	NEXT();
fmul: 
	res = (((float*)stack)[sp--] * ((float*)stack)[sp]);
	stack[sp] = *(uint32_t*)&res; 
	NEXT();
fdiv: 
	res = (((float*)stack)[sp-1] / ((float*)stack)[sp--]);
	stack[sp] = *(uint32_t*)&res; 
	NEXT();
pushi32: 
	stack[++sp] = *(uint32_t*)(prg + pc);
	pc += 4; 
	NEXT();
pushi16: 
	stack[++sp] = (uint32_t)(*(uint16_t*)(prg + pc));
	pc += 2; 
	NEXT();
swap: 
	uint32_t k = stack[sp];
	stack[sp] = stack[sp-1];
	stack[sp-1] = k; 
	NEXT();
beq:
	if(stack[sp] == stack[--sp]) {
		pc += *(int16_t*)(prg + pc); // add 2 for the 2 bytes for args
	}
	pc += 2;
	NEXT();
bgt:
	if(stack[sp] > stack[--sp]) {
		pc += *(int16_t*)(prg + pc);
	}
	pc += 2;
	NEXT();
blt:
	if(stack[sp] < stack[--sp]) {
		pc += *(int16_t*)(prg + pc);
	}
	pc += 2;
	NEXT();
bge:
	if(stack[sp] >= stack[--sp]) {
		pc += *(int16_t*)(prg + pc);
	}
	pc += 2;
	NEXT();
ble:
	if(stack[sp] <= stack[--sp]) {
		pc += *(int16_t*)(prg + pc);
	}
	pc += 2;
	NEXT();
bne:
	if(stack[sp] != stack[--sp]) {
		pc += *(int16_t*)(prg + pc);
	}
	pc += 2;
	NEXT();
jmp: 
	pc += *(int16_t*)(prg + pc) + 2; 
	NEXT();
dup: 
	stack[++sp] = stack[sp]; 
	NEXT();

andb: 
	stack[sp] = stack[sp--] & stack[sp]; 
	NEXT();
orb: 
	stack[sp] = stack[sp--] | stack[sp]; 
	NEXT();
xorb: 
	stack[sp] = stack[sp--] ^ stack[sp]; 
	NEXT();
notb: 
	stack[sp] = ~stack[sp]; 
	NEXT();

store: 
	locals[*(unsigned char*)(prg+(pc++))] = stack[sp--]; 
	NEXT();
load: 
	stack[++sp] = locals[*(unsigned char*)(prg+(pc++))]; 
	NEXT();

pushpc: 
	stack[++sp] = pc; 
	NEXT();
writepc: 
	pc = stack[sp--]; 
	NEXT();
pushsp: 
	stack[++sp] = sp; 
	NEXT();
writesp: 
	sp = stack[sp--]; 
	NEXT();
jsr: 
	stack[++sp] = pc; 
	stack[sp]+=2;
	goto jmp; 
jsrs: 
	uint32_t newpc = stack[sp--];
	stack[++sp] = pc; 
	pc = newpc; 
	NEXT();
pushab: 
	stack[sp] = prg[stack[sp]]; 
	NEXT();
pusha: 
	stack[sp] = *(uint16_t*)(prg+stack[sp]); 
	NEXT();
pushaw: 
	stack[sp] = *(uint32_t*)(prg+stack[sp]); 
	NEXT();
sprint: 
	printf("%s\n", (char*) prg+stack[sp]); 
	NEXT();
lsl: 
	stack[sp] = stack[sp-1] << stack[sp--]; 
	NEXT();
lsr: 
	stack[sp] = stack[sp-1] >> stack[sp--]; 
	NEXT();
mod: 
	stack[sp] = stack[sp-1] % stack[sp--]; 
	NEXT();
pushi8: 
	stack[++sp] = (uint32_t)(*(uint8_t*)(prg + pc));
	pc += 1; 
	NEXT();
negf: 
	stack[sp] = stack[sp] ^ 0x80000000; 
	NEXT();
neg: 
	stack[sp] = -stack[sp]; 
	NEXT();
c0: 
	stack[++sp] = 0; 
	NEXT();
c1: 
	stack[++sp] = 1; 
	NEXT();
c2: 
	stack[++sp] = 2; 
	NEXT();
c3: 
	stack[++sp] = 3; 
	NEXT();
c4: 
	stack[++sp] = 4; 
	NEXT();
c5: 
	stack[++sp] = 5; 
	NEXT();
c6: 
	stack[++sp] = 6; 
	NEXT();
c7: 
	stack[++sp] = 7; 
	NEXT();
malloca: 
	stack[sp] = (uint32_t)malloc(stack[sp]);
	// ((uint32_t*)k)[0] = 0;
	// ((uint32_t*)stack[sp])[0] = 0;
	// for(int i=0;i<10;++i) { ((uint32_t*)stack[sp])[i] = 30-i; } 
	NEXT();
freea: 
	free((uint32_t*)stack[sp--]); 
	NEXT();
indexab: 
	stack[sp] = *((unsigned char*)(stack[sp]+stack[sp-1])); 
	NEXT();
indexa: 
	stack[sp] = *((uint16_t*)(stack[sp]+stack[sp-1])); 
	NEXT();
indexaw: 
	stack[sp] = *((uint32_t*)(stack[sp]+stack[sp-1])); 
	NEXT();
storea: 
	(*(uint32_t*)(stack[sp-2]+stack[sp-1])) = stack[sp]; sp--; 
	NEXT();

store0: 
	locals[0] = stack[sp--]; 
	NEXT();
store1: 
	locals[1] = stack[sp--]; 
	NEXT();
store2: 
	locals[2] = stack[sp--]; 
	NEXT();
store3: 
	locals[3] = stack[sp--]; 
	NEXT();
store4: 
	locals[4] = stack[sp--]; 
	NEXT();
load0: 
	stack[++sp] = locals[0]; 
	NEXT();
load1: 
	stack[++sp] = locals[1]; 
	NEXT();
load2: 
	stack[++sp] = locals[2]; 
	NEXT();
load3: 
	stack[++sp] = locals[3]; 
	NEXT();
load4: 
	stack[++sp] = locals[4]; 
	NEXT();
	halt:
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
	struct stat statbuf;
	if(fstat(fileno(inputfile), &statbuf) < 0){
		printf("could not open file %s!\n", argv[1]);
		exit(2);
	}
	unsigned char* buffer = mmap(NULL, statbuf.st_size, PROT_READ|PROT_WRITE, MAP_PRIVATE, fileno(inputfile), 0);
	buffer[statbuf.st_size] = 0xff;
	fclose(inputfile);
	prg = buffer;
	run();

	if(munmap(buffer, statbuf.st_size)) {
		printf("unmapping failed!\n");
		return (1);
	}
	return 0;
}