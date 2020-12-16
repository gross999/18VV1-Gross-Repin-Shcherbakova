#include <iostream>

#include <Windows.h>

#include <conio.h>

#include <fstream>

#include <string.h>

using namespace std;

char* secret = new char[6]{ 's', 'e', 'c', 'r', 'e', 't' }; //тайное слово

void write_secret(const wchar_t* picture, const wchar_t* text) {

	char* buffer_picture;
	char* buffer_text;
	ifstream BMPFile(picture, ios::binary); //открытие бинарного файла
	size_t size_picture = BMPFile.seekg(0, ios::end).tellg(); //указатель в конец, вычисляем размер файла

	BMPFile.seekg(0, ios::beg); //возвращаем указатель на начало
	buffer_picture = new char[size_picture]; //выделяем память для буффера изображения
	BMPFile.read(buffer_picture, size_picture); //записываем файл в буффер
	BMPFile.close();

	ifstream TXTFile(text, ios::binary); //открытие бинарного файла
	size_t size_text = TXTFile.seekg(0, ios::end).tellg(); //указатель в конец, вычисляем размер файла

	TXTFile.seekg(0, ios::beg); //возвращаем указатель на начало
	buffer_text = new char[size_text]; //выделяем память для буффера тескта
	TXTFile.read(buffer_text, size_text); //записываем файл в буффер
	TXTFile.close();

	int bit;
	int mask = 0;
	int num_bit = 31;
	int width_picture = 0;

	for (int i = 21; i > 17; i--) {

		for (bit = 7; bit > -1; bit--) {

			mask = ((buffer_picture[i] >> bit) & 1);

			if (mask == 1) {

				width_picture += pow(2, num_bit);

			}

			num_bit--;

		}

	}

	num_bit = 31;
	int high_picture = 0;

	for (int i = 25; i > 21; i--) {

		for (bit = 7; bit > -1; bit--) {

			mask = ((buffer_picture[i] >> bit) & 1);

			if (mask == 1) {

				high_picture += pow(2, num_bit);

			}
			num_bit--;
		}

	}

	int size_for_secret = (high_picture * width_picture * 3 - sizeof(size_t) - sizeof(secret)) / 4;

	if (size_text < size_for_secret) {

		bit = 15;
		mask = 0;

		for (int i = 54; i < 54 + sizeof(size_t) * 4; i++) {

			buffer_picture[i] = buffer_picture[i] & 0xFC;

			for (int j = 1; j > -1; j--) {

				mask = ((size_text >> bit) & 1) << j;
				buffer_picture[i] = buffer_picture[i] | mask;
				bit--;

			}

		}

		bit = 7;
		int num_text = 0; //отвечает за номер бита который будем записывать
		size_text = 54 + sizeof(size_t) * 4 + (size_text * 4); //записываем с 62 до 62+ размер картинки умноженный на 4

		for (int i = 54 + sizeof(size_t) * 4; i < size_text; i++) {

			buffer_picture[i] = buffer_picture[i] & 0xFC;

			for (int j = 1; j > -1; j--) { //то же самое что и с размером

				mask = ((buffer_text[num_text] >> bit) & 1) << j;
				buffer_picture[i] = buffer_picture[i] | mask;
				bit--;

			}

			if (bit == -1) { //если мы записали все 8 бит

				num_text++; //то передвигаем указатель на следующий
				bit = 7;

			}

		}

		num_text = 0;
		bit = 7;

		int size = size_text + (6 * 4); //записываем секретное сообщение

		for (int i = size_text; i < size; i++) { //начинаем после записи текстового файла

			buffer_picture[i] = buffer_picture[i] & 0xFC;

			for (int j = 1; j > -1; j--) {

				mask = ((secret[num_text] >> bit) & 1) << j;
				buffer_picture[i] = buffer_picture[i] | mask;
				bit--;

			}

			if (bit == -1) {

				num_text++;
				bit = 7;

			}

		}

		ofstream BMPprintFile(picture, ios_base::trunc | ios::binary);

		for (int i = 0; i < size_picture; i++) {

			BMPprintFile << buffer_picture[i]; //поэлементно перезаписываем картинку

		}

		ofstream TXTFilePrint(text, ios::binary);
		TXTFilePrint.open(text);
		TXTFilePrint.close();

	}

	else {

		cout << ("Text file is bigger than max size") << endl;
	}

}

void read_secret(const wchar_t* picture, const wchar_t* text) {

	char* buffer_picture;
	char* buffer_text;

	ifstream BMPFile(picture, ios::binary); //чтение из файла
	size_t size_picture = BMPFile.seekg(0, ios::end).tellg(); //указатель на конец,что бы узнать размер

	BMPFile.seekg(0, ios::beg); //возвращаем указатель на начало
	buffer_picture = new char[size_picture]; //выделяем память для буффера изображения
	BMPFile.read(buffer_picture, size_picture); //записываем файл в буффер
	BMPFile.close();

	int size = 0;
	int mask = 0;
	int bit = 15;

	for (int i = 54; i < 54 + sizeof(size_t) * 4; i++) { //получаем размер текста

		for (int j = 1; j > -1; j--) {

			mask = (buffer_picture[i] >> j) & 1;
			mask = mask << bit;
			size = size | mask;
			bit--;

		}

		buffer_picture[i] = buffer_picture[i] & 0xFC;

	}

	buffer_text = new char[size]; //приравниваем размер буффера тхт к полученному размеру
	bit = 7;
	int num_text = 0;
	int mask_text = 0;
	int size_res = size * 4;
	size_res = 54 + sizeof(size_t) * 4 + size_res;
	
	for (int i = 54 + sizeof(size_t) * 4; i < size_res; i++) { //получаем текстовое сообщение из картинки

		for (int j = 1; j > -1; j--) {

			mask = (buffer_picture[i] >> j) & 1;
			mask = mask << bit;
			mask_text = mask_text | mask;
			bit--;

		}

		buffer_picture[i] = buffer_picture[i] & 0xFC;

		if (bit == -1) {

			buffer_text[num_text] = mask_text;
			mask_text = 0;
			num_text++;
			bit = 7;

		}

	}

	for (int i = size_res; i < size_res + 6 * 4; i++) {

		buffer_picture[i] = buffer_picture[i] & 0xFC;

	}

	ofstream BMPprintFile(picture, ios_base::trunc | ios::binary);
	
	for (int i = 0; i < size_picture; i++) {

		BMPprintFile << buffer_picture[i]; //поэлементно перезаписываем картинку

	}

	ofstream TXTprint(text, ios::binary);
	int sz = sizeof(buffer_text);

	for (int i = 0; i < size; i++) { //записываем полученное сообщение в txt

		TXTprint << buffer_text[i];

	}

}

void check_secret(const wchar_t* picture) {

	char* bufferbmp;
	char* buffertxt;

	ifstream BMPFile(picture, ios::binary); //чтение из файла
	size_t size1 = BMPFile.seekg(0, ios::end).tellg(); //указатель в конец,что бы узнать размер
	BMPFile.seekg(0, ios::beg); //возрващаем указатель на начало
	bufferbmp = new char[size1]; //выделяем память для буффера изображения
	BMPFile.read(bufferbmp, size1); //записываем файл в буффер
	BMPFile.close();

	int size = 0;
	int mask = 0;
	int bit = 15;

	for (int i = 54; i < 54 + sizeof(size_t) * 4; i++) { //получаем размер текста

		for (int j = 1; j > -1; j--) {

			mask = (bufferbmp[i] >> j) & 1;
			mask = mask << bit;
			size = size | mask;
			bit--;

		}

	}

	int txtnext = 0;
	bit = 7;
	int masktxt = 0;
	buffertxt = new char[sizeof(secret)];
	size = size * 4;
	size = size + 54 + sizeof(size_t) * 4;

	int sizee = size + (8 * 4);

	for (int i = size; i < sizee; i++) { //тайное сообщение находится после 62 байта плюс размер картинки умноженный на 4

		for (int j = 1; j > -1; j--) {

			mask = (bufferbmp[i] >> j) & 1;

			if (mask == 1) {

				masktxt = masktxt | (1 << bit);

			}

			else {

				mask = 1;
				mask = mask << bit;
				mask = ~mask;
				masktxt = masktxt & mask;

			}

			bit--;

		}

		if (bit == -1) {

			buffertxt[txtnext] = masktxt;
			txtnext++;
			bit = 7;

		}

	}

	bool check = TRUE;

	for (int i = 0; i < 6; i++) {

		if (buffertxt[i] != secret[i]) {

			check = FALSE;

		}

	}

	if (check == FALSE) {

		cout << ("TRUE") << endl;

	}

	else {

		cout << ("FALSE") << endl;

	}

}

int wmain(int argc, wchar_t* argv[])

{

	if (!wcscmp(argv[1], (L"-embed"))) {

		write_secret(argv[2], argv[3]);

		cout << ("Well done!") << endl;

	}

	if (!wcscmp(argv[1], (L"-retrieve"))) {

		read_secret(argv[2], argv[3]);

		cout << ("Well done") << endl;

	}

	if (!wcscmp(argv[1], (L"-check"))) {

		check_secret(argv[2]);

		cout << ("Well done") << endl;

	}

}