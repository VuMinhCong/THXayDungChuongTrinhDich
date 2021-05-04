//Em sử dụng tham số dòng lệnh thầy ạ, argv[1] là tên file văn bản : "lice30.txt", argv[2] là tên file chứa các từ mà khi chương trình gặp thì không đếm : "stopw.txt"
//File output là : "output.txt"
#include<stdio.h>
#include<ctype.h>
#include<stdlib.h>
#include<string.h>

static int local[15000][1000];
static char words[15000][50];

void main(int argc, char *argv[]) {
	//Khai bao bien
	FILE* f1, *f2, *f3;
	long i = 0, kt = 1, a = 0, nline = 0, x = 0, n = 0, j = 0, slen = 0;
	char buff[50];
	char tmpstr[20][50], ttmp;
	f1 = fopen(argv[1], "r");
	f2 = fopen(argv[2], "r");
	f3 = fopen("output.txt", "w");

	//Doc file "stopw.txt" va luu cac tu vao trong mang "tmpstr"
	while (!feof(f2)) {
		fgets(buff, 50, f2);
		if (buff[strlen(buff)-1] == '\n') buff[strlen(buff)-1] = '\0';
		strcpy(tmpstr[n++], buff);
	}

	//Bat dau doc file "vanban.txt"
	i = 0;
	while (!feof(f1)) {
		i++;
		x = 0;
		while ((ttmp = fgetc(f1)) != ' ' && (ttmp != '\n') && !feof(f1)) {	//Doc cac tu, phan cach nhau boi dau cach hoac dau xuong dong.
			buff[x++] = ttmp;
		}
		buff[x] = '\0';
		if (ttmp == '\n') nline += 1;							//Neu phat hien ki tu xuong dong, tang so dong len 1.
		if (strlen(buff) == 0) continue;
		if ((kt != 1) && ((int)buff[0] <= 90) && ((int)buff[0] >= 65)) continue;	//Kiem tra chu cai dau, neu truoc do khong co dau cham ma viet hoa thi skip, vi do la ten rieng
		if (buff[strlen(buff) - 1] == '.') {
			kt = 1;
			buff[strlen(buff) - 1] = '\0';
		}
		else kt = 0;

		for (j = 0; j < strlen(buff); j++) {
			if (((int)buff[j] <= 90) && ((int)buff[j] >= 65)) buff[j] = tolower(buff[j]);	//Chuan hoa tat ca ki tu ve chu thuong, xoa cac dau cau.
			if (((int)buff[j] <= 96) || ((int)buff[j] >= 123)) {
				slen = strlen(buff);
				for (int k = j; k < slen; k++) buff[k] = buff[k+1];
				j--;
			}
		}

		//printf("buff = %s\n", buff);

		kt = 1;
		for (j = 0; j < n; j++) {							//Kiem tra xem tu vua doc co nam trong stopw.txt khong.
			if (strcmp(buff, tmpstr[j]) == 0) {kt = 0; break;}
		}
		if (kt == 0) {kt = 1; continue;}

		for (j = 0; j <= a; j++) {							//a la so words khac nhau da phat hien, tim lan luot tung words va so sanh voi buff
			if (strcmp(buff, words[j]) == 0) {kt = 0; break;}
		}
		if (kt == 0) {										//Them chi so dong` ma tu xuat hien vao mang
			//printf("line = %d, a = %d, j = %d\n", nline, a, j);
			local[j][0]++;
			if (nline != local[j][local[j][0]-1]) local[j][local[j][0]] = nline;
		}
		else {												//Neu chay den het ma khong gap tu nao trung voi buff thi tang a len 1 va luu tu moi vao danh sach words
			//printf("line = %d, a = %d, j = %d\n", nline, a, j);
			local[j][0] = 1;
			local[j][1] = nline;
			strcpy(words[a], buff);
			a++;
		}
	}

	j = 0;
	for (i = 0; i < a; i++) {
		fprintf(f3, "\n%s ", words[i]);
		for (j = 1; j < local[i][0]; j++) fprintf(f3, "%d,", local[i][j]);
		fprintf(f3, "%d", local[i][j]);
	}

	fclose(f1);
	fclose(f2);
	fclose(f3);
}