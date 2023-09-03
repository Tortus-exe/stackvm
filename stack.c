#include <stdio.h>

int main(int argc, char* argv[]) {
	if(argc!=2) {
		printf("not enough arguments! usage: \n%s [inputfile]\n", argv[0]);
	}
	return 0;
}