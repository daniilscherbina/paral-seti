#include <iostream>  
#define _WINSOCK_DEPRECATED_NO_WARNINGS  
// подавление предупреждений библиотеки winsock2
#include <winsock2.h> 
#include <string>
#include <windows.h>
#pragma comment (lib, "Ws2_32.lib")
#pragma warning(disable: 4996)  // подавление предупреждения 4996 
using namespace std;
#define SRV_HOST "localhost"  //адрес сервера
#define SRV_PORT 1234 //порт сервера
#define CLNT_PORT 1235  //порт клиента
#define BUF_SIZE 64  //размер сообщений
char TXT_ANSW[] = "I am your student\n";

struct Person
{
	string name;  //имя
	int grades[4];  //оценки
} A;

int answer = -1;

int main()
{
	char buff[1024];
	if (WSAStartup(0x0202, (WSADATA*)&buff[0]))
	{
		cout << "Error WSAStartup \n" << WSAGetLastError();
		return -1;
	}
	SOCKET s;
	int from_len;
	char buf[BUF_SIZE] = { 0 };
	hostent* hp;
	sockaddr_in clnt_sin, srv_sin;
	s = socket(AF_INET, SOCK_STREAM, 0);
	clnt_sin.sin_family = AF_INET;
	clnt_sin.sin_addr.s_addr = 0;
	clnt_sin.sin_port = htons(CLNT_PORT);
	bind(s, (sockaddr*)&clnt_sin, sizeof(clnt_sin));
	hp = gethostbyname(SRV_HOST);
	srv_sin.sin_port = htons(SRV_PORT);
	srv_sin.sin_family = AF_INET;
	((unsigned long*)&srv_sin.sin_addr)[0] = ((unsigned long**)hp->h_addr_list)[0][0];
	connect(s, (sockaddr*)&srv_sin, sizeof(srv_sin));
	do {
		string mst;
		from_len = recv(s, (char*)&buf, BUF_SIZE, 0);
		buf[from_len] = 0;
		cout << buf << endl;
		
		int n;

		cout << "1. write request" << endl;
		cout << "2. exit" << endl;
		cin >> n;
		if (n == 2)
		{
			A.grades[0] = -1;
			send(s, (char*)&A, sizeof(A), 0);
			break;
		}
		cout << "you:";
		cin >> A.name;
		for (int i = 0; i < 4; i++)
			cin >> A.grades[i];

		send(s, (char*)&A, sizeof(A), 0);
		from_len = recv(s, (char*)&answer, sizeof(answer), 0);
		cout << "server: ";
		switch (answer) {
			case 2: {cout << "dolg\n"; break; }
			case 3: {cout << "net stependii\n"; break; }
			case 4: {cout << "obucnaia stependii\n"; break; }
			case 5: {cout << "povishinaia stependii"; break; }
			case -1: {cout << "Error, server sent wrong data"; }
		}
	} while (true);
	cin.get();
	closesocket(s);
	return 0;
}