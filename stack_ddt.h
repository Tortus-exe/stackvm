#define STACK_SIZE 4096
#define NUMLOCALS 256
#define NUM_ISRS 0

typedef struct {
	uint32_t pc;
	uint32_t* stack;
	uint32_t* locals;
	uint16_t sp;
	unsigned char* prg;
#ifdef NUM_ISRS
	uint32_t isrs[NUM_ISRS];
	unsigned char isrid;
#endif
} PRG;