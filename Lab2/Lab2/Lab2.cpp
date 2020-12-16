#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <conio.h>
#include <locale.h>

#define FILE_NAME "Lab2.pdb"

void char_to_bin(char text[4], int bin32[32])
{
	int i = 0;

	while (i < 32)
		for (int j = 0; j < 4; j++)
		{
			for (unsigned q = 1U << 7; q; q >>= 1)
			{
				bin32[i] = !!((int)text[j] & q);
				i++;
			}
		}
}

void bin_to_char(int bin32[], char ntext[5])
{
	int j = 3;
	int count = 0;
	int count1 = 0;
	int in_code[4] = { 0,0,0,0 };

	for (int i = 31; i > -1; i--)
	{
		if (bin32[i] == 1) {
			in_code[j] += pow(2, count - count1 * 8);
		}
		if (i % 8 == 0)
		{
			j--;
			count1++;
		}
		count++;
	}
	for (int i = 0; i < 4; i++)
	{
		ntext[i] = (char)in_code[i];
	}
	ntext[4] = '\0';

}
char coder_decoder(char key[32], int mod) {            //шифрование/дешифрование

	FILE *data_file, *data_file_new;
	char data[5];
	data[4] = '\0';
	char ntext[5];
	int ntext_in_binary_code[32];
	int text_in_binary_code[32];

	if (mod == 0)
	{
		data_file = fopen(FILE_NAME, "rb");
		data_file_new = fopen("encrypted.txt", "wb+");
	}
	else
	{
		data_file = fopen("encrypted.txt", "rb");
		data_file_new = fopen("decrypted.txt", "wb+");
	}
	while (fread(data, 4, 1, data_file) != NULL) {
		char_to_bin(data, text_in_binary_code);
		if (mod == 0)
			for (int i = 0; i < 32; i++)
				ntext_in_binary_code[i] = text_in_binary_code[key[i]];     //осуществление шифрования файла в заданной последовательности битов
		else
			for (int i = 0; i < 32; i++)
				ntext_in_binary_code[key[i]] = text_in_binary_code[i];          //расшировка кода

		bin_to_char(ntext_in_binary_code, ntext);

		fwrite(ntext, 1, 4, data_file_new);
	}
	fclose(data_file_new);
	fclose(data_file);

	if (mod == 0)
		printf("Файл зашифрован в encrypted.txt.\n");
	else
		printf("Файл расшифрован в decrypted.txt.\n");
}

void GetKey(char key[32])
{
	FILE *mykey;

	mykey = fopen("key.txt", "rb");
	for (int i = 0; i < 32; i++)
		fscanf(mykey, "%d", &key[i]);
	fclose(mykey);
}

void OptText()			//добавление пробелов для правильного шифрования
{
	FILE *data_file;

	data_file = fopen(FILE_NAME, "rb+");
	fseek(data_file, 0, SEEK_END);
	while (ftell(data_file) % 4 != 0)
		fputs(" ", data_file);
	fclose(data_file);
}

void Test()
{
	FILE *data_file, *data_file_new;
	char s1 = { }, s2 = { };

	data_file = fopen(FILE_NAME, "rb");
	data_file_new = fopen("decrypted.txt", "rb");
	while (!feof(data_file_new) && !feof(data_file) && (s1 == s2)) {
		s1 = fgetc(data_file_new);
		s2 = fgetc(data_file);
	}
	if (s1 == s2)
		printf(" \nФайлы одинаковы!");
	else
		printf("\nФайлы разные!");
	fclose(data_file);
	fclose(data_file_new);

}

int main()
{
	setlocale(LC_ALL, "Rus");

	char key[32];
	GetKey(key);				//Получение ключа
	OptText();					//Оптимизатор текста
	coder_decoder(key, 0);		//шифрование
	coder_decoder(key, 1);		//дешифрование
	Test();						//Сравнение
	_getch();
}
