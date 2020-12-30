#include<iostream>
#include<conio.h>
#include<ctime>

using namespace std;

int one = 0, two = 0, three = 0, four = 0, five = 0, six = 0;

void diapozon(int r)
{
	if (r >= 0 && r <= 50) {
		one++;
	}
	else if (r > 50 && r <= 100) {
		two++;
	}
	else if (r > 100 && r <= 150) {
		three++;
	}
	else if (r > 150 && r <= 200) {
		four++;
	}
	else if (r > 200 && r <= 250) {
		five++;
	}
	else if (r > 250 && r <= 300) {
		six++;
	}
}

void generator() {
	unsigned short  a, b, r;
	time_t timec = time(0);
	b = timec;
	const int num = 20;

	cout << "Введите первое число: ";
	cin >> a;
	cout << endl;
	for (int i = 0; i < num; i++) {
		r = a * b;
		r = r << 4;
		r = r >> 8;
		cout << "Число: " << i << " - " << r;
		a = b;
		b = r;
		cout << endl;
		diapozon(r);
	}
	cout << endl;
	cout << "Числа в промежутке 0 - 50: " << one << endl;
	cout << "Числа в промежутке 50 - 100: " << two << endl;
	cout << "Числа в промежутке 100 - 150: " << three << endl;
	cout << "Числа в промежутке 150 - 200: " << four << endl;
	cout << "Числа в промежутке 200 - 250: " << five << endl;
	cout << "Числа в промежутке 250 - 300: " << six << endl;
}

void menu() {
	char ch_key;
	switch ((ch_key = _getch()))
	{
	case'1':
		generator();
		system("pause");
		break;
	case 27:
		cout << "Exit from program" << endl;;
		exit(0);
		break;
	default:
		break;
	}
}

int main()
{
	setlocale(LC_ALL, "Russian");
	while (true) {
		system("cls");
		menu();
	}
	system("pause");
}
