#include <iostream>  
#include <winsock2.h> 
#include <windows.h> 
#include <string> 
#include <thread>  // добавляем заголовок для работы с потоками
#include <atomic> // добавляем заголовок для работы с атомарными переменными
#pragma comment (lib, "Ws2_32.lib")  
using namespace std;

#define SRV_PORT 1234  // порт сервера (его обязательно должен знать клиент)
#define BUF_SIZE 64  // размер

std::atomic<int> count_clients = 0;

struct Person
{
    string name;  // имя
    int grades[4];  // оценки
};

const string QUEST = "Enter the data\n"; // первый вопрос для клиента, чтобы начать диалог

int getAnsw(int mark[4])
{
    int tree = 0;
    int two = 0;
    int four = 0;
    for (int i = 0; i < 4; i++)
    {
        if (mark[i] == 4)
            four++;
        if (mark[i] == 3)
            tree++;
        if (mark[i] == 2)
            two++;
    }

    if (two > 0)
        return 2;
    if (tree > 0)
        return 3;
    if (four > 0)
        return 4;
    return 5;
}

void clientThread(SOCKET s_new)
{
    Person B;
    int answer;
    while (true) {
        string msg;
        msg = QUEST;
        send(s_new, (char*)&msg[0], msg.size(), 0);
        recv(s_new, (char*)&B, sizeof(B), 0);
        if (B.grades[0] == -1)
            break;
        answer = getAnsw(B.grades);
        send(s_new, (char*)&answer, sizeof(answer), 0);
    }
    cout << "client is lost" << endl;
    // уменьшаем счетчик клиентов
    count_clients.fetch_sub(1);
    cout << "current connections: " << count_clients.load() << endl;
    closesocket(s_new);
}

int main()
{
    setlocale(LC_ALL, "rus");
    char buff[1024];
    if (WSAStartup(0x0202, (WSADATA*)&buff[0]))
    {
        cout << "Error WSAStartup \n" << WSAGetLastError();   // Ошибка!
        return -1;
    }
    SOCKET s;
    int from_len;
    sockaddr_in sin;
    s = socket(AF_INET, SOCK_STREAM, 0);
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = 0;
    sin.sin_port = htons(SRV_PORT);
    bind(s, (sockaddr*)&sin, sizeof(sin));
    string msg, msg1;
    listen(s, 6);

    while (1)
    {
        sockaddr_in from_sin;
        from_len = sizeof(from_sin);
        cout << "break 5" << endl;
        SOCKET s_new = accept(s, (sockaddr*)&from_sin, &from_len);
        cout << "break 6" << endl;
        if (s_new == INVALID_SOCKET) {
            cerr << "Error accepting client connection: " << WSAGetLastError() << endl;
            continue;
        }
        else
        {
            cout << "new connected client! " << endl;
            // увеличиваем счетчик клиентов
            count_clients.fetch_add(1);
            cout << "current connections: " << count_clients.load() << endl;

            // Создаем новый поток для общения с клиентом
            thread client(clientThread, s_new);
            client.detach();  // отсоединяем поток, чтобы он продолжал работу независимо, т.е. создаем демона
        }
    }
    return 0;
}
