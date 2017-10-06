#include <stdio.h>
#include <stdlib.h> //For system()
#include <string.h> //For strcmp()

//#define LOCATION "C:\\Users\\user\\Desktop\\z3-4.3.2-x64-win\\z3-4.3.2-x64-win\\bin\\"
#define Z3 "C:\\Users\\user\\Desktop\\z3-4.3.2-x64-win\\z3-4.3.2-x64-win\\bin\\z3.exe -dimacs formula.txt > temp.txt"
#define INPUT "input.txt"
#define TEMP "temp.txt"
#define OUTPUT "output.txt"
#define FORMULA "formula.txt"

#define SIZE 9    //9x9


int main(void) {

	FILE * fpfor;
	FILE * fpin;
	FILE * fptempmake;    //For making 'temp.txt' file    

	fptempmake = fopen(TEMP, "w");
	fpfor = fopen(FORMULA, "w");
	fpin = fopen(INPUT, "r");    //this('INPUT') already exists by the user.
	if (fpfor == NULL || fpin == NULL || fptempmake == NULL) {
		printf("File open error!!1");
		return -1;
	}
	fclose(fptempmake);

	//Transfrom the input.txt to fomula.txt
	int i, j, n;
	char temp;
	int arr_size = 0, star_size = 0, hash_size = 0;
	int arr[SIZE*SIZE];
	int star[SIZE];
	int hash[SIZE];
	//Scan input.txt file
	for (i = 0; i < SIZE*SIZE; i++) {
		fscanf(fpin, " %c", &temp);
		if (temp == '0')
			continue;
		else if (temp == '*')
			star[star_size++] = (i*SIZE) + 1;
		else if ('1' <= temp && temp <= SIZE + '0')
			arr[arr_size++] = (i * SIZE) + temp - '0';
		else if (temp == '#')
			hash[hash_size++] = (i*SIZE) + 1;
		else {
			printf("Input file is invalid.\n");
			return -1;
		}
	}


	if (SIZE == 3)
		fprintf(fpfor, "p cnf %d %d\n", SIZE*SIZE*SIZE, SIZE*SIZE * 2 + ((SIZE*(SIZE - 1)) / 2)*(SIZE * SIZE) + SIZE*SIZE + arr_size + star_size > 1 ? star_size-1 : 0 + (int)(hash_size * (hash_size-1)*SIZE*(0.5))); //1.( SIZE * SIZE ) + 2.( SIZE * SIZE ) + 3.( (C(SIZE,2)+1)*SIZE*SIZE ) + arr_size + asterisk + hash
	else if (SIZE == 9)
		fprintf(fpfor, "p cnf %d %d\n", SIZE*SIZE*SIZE, SIZE*SIZE * 2 + ((SIZE*(SIZE - 1)) / 2)*(SIZE * SIZE) + SIZE*SIZE + SIZE*SIZE + arr_size + star_size > 1 ? star_size - 1 : 0 + (int)(hash_size * (hash_size - 1)*SIZE*(0.5))); // 4. (SIZE * SIZE) 

	//Conditions that should be originally checked

	//1.row check
	for (i = 1; i <= SIZE; i++) {
		for (n = 1; n <= SIZE; n++) {
			for (j = 1; j <= SIZE; j++) {
				fprintf(fpfor, "%d ", (j - 1) * SIZE + n + (i - 1) * SIZE*SIZE);
			}
			fprintf(fpfor, "0\n");
		}
	}
	//2.column check
	for (j = 1; j <= SIZE; j++) {
		for (n = 1; n <= SIZE; n++) {
			for (i = 1; i <= SIZE; i++) {
				fprintf(fpfor, "%d ", (j - 1) * SIZE + n + (i - 1) * SIZE*SIZE);
			}
			fprintf(fpfor, "0\n");
		}
	}

	//3.Check that only one value must be true in the same column and row
	for (i = 1; i <= SIZE; i++) {
		for (j = 1; j <= SIZE; j++) {
			for (n = 1; n <= SIZE; n++) {
				fprintf(fpfor, "%d ", n + (j - 1) * SIZE + (i - 1) * SIZE*SIZE);
			}
			fprintf(fpfor, "0\n");
		}
	}
	int m;
	for (i = 1; i <= SIZE; i++) {
		for (j = 1; j <= SIZE; j++) {
			for (n = 1; n <= SIZE - 1; n++) {
				for (m = n + 1; m <= SIZE; m++) {
					fprintf(fpfor, "-%d -%d 0\n", n + (j - 1)*SIZE + (i - 1)*SIZE*SIZE, m + (j - 1)*SIZE + (i - 1)*SIZE*SIZE);
				}
			}
		}
	}
	//4. Check 3*3 box in 9*9 sdoku.
	if (SIZE == 9) {
		int r, s;
		for (r = 0; r <= 2; r++) {
			for (s = 0; s <= 2; s++) {
				for (n = 1; n <= 9; n++) {
					for (i = 1; i <= 3; i++) {
						for (j = 1; j <= 3; j++) {
							fprintf(fpfor, "%d ", (j - 1) * 9 + (i - 1) * 81 + s * 27 + r * 243 + n);
						}
					}
					fprintf(fpfor, "0\n");
				}
			}
		}
	}

	//Conditions that should be checked by input file(INPUT)

	//Check numbers already present.
	for (i = 0; i<arr_size; i++)
		fprintf(fpfor, "%d 0\n", arr[i]);

	//Check asterisk.
	for (i = 0; i < star_size - 1; i++) {
			for (m = 0; m < SIZE; m++) {
				fprintf(fpfor, "-%d %d 0\n", star[i] + m, star[i + 1] + m);
			}
	}


	//Check Hash.(The hash is another number each other.)
	for (i = 0; i < hash_size - 1; i++) {
		for (j = i + 1; j < hash_size; j++) {
			for (m = 0; m < SIZE; m++) {
				fprintf(fpfor, "-%d -%d 0\n", hash[i]+m, hash[j]+m);
			}
		}
	}

	fclose(fpfor);
	fclose(fpin);
	printf("Running z3...\n");
	system(Z3);

	//Make output file from temp file
	FILE * fptemp;
	FILE * fpoutput;

	fptemp = fopen(TEMP, "r");
	fpoutput = fopen(OUTPUT, "w");
	if (fptemp == NULL || fpoutput == NULL) {
		printf("File open error!2");
		return -1;
	}

	char str[10];
	int tmp;
	fscanf(fptemp, "%s", str);
	if (strcmp(str, "unsat") == 0) {
		fprintf(fpoutput, "unsat");
		printf("unsat\n");
	}
	else if (strcmp(str, "sat") == 0) {
		for (i = 0; i < SIZE*SIZE*SIZE; i++) {
			fscanf(fptemp, "%d", &tmp);    //temp(int) is declared before.
			if (tmp > 0)
				fprintf(fpoutput, "%d ", (tmp - 1) % SIZE + 1);
			if ((i + 1) % (SIZE*SIZE) == 0)
				fprintf(fpoutput, "\n");
		}
		printf("Success! check the output file.(%d x %d)\n", SIZE, SIZE);
	}
	else
		printf("Both not 'sat' and 'unsat'\n");

	return 0;
}
