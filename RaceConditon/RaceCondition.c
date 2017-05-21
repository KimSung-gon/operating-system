#include <stdio.h>

int Load();
void Store(int data);
void Call();
void OpenFile();

void main(){
		
	Call();
}

int Load() {
	
	int num;
	int returnNum;
	int testNum=0;
	
	FILE *infile;
	infile = fopen("input.txt", "r");
	
	while(fscanf(infile, "%d", &num) == 1){
		returnNum = num;
	}

	fclose(infile);
	return returnNum;
}


void Store(int data) {

	int num;
	FILE *infile; 	

	infile = fopen("input.txt", "a+");
	fprintf(infile, "\n%d", data);
	fclose(infile);
	
}


int Add(int i,int j) {
   return i+j;
}


void Call() {

	int i;
	int x;
	for (i=0; i<1000; i++) {
		x=Load();
		x=Add(x,1);
		Store(x);
	}
}
