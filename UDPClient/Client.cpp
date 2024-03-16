
#define WIN32_LEAN_AND_MEAN

#include <ws2tcpip.h>
#include <windows.h>
#include <iostream>
#include <string>
using namespace std;

#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 4096

#define SERVER_IP "127.0.0.1"
#define DEFAULT_PORT "8888"

SOCKET client_socket;


WORD clientColor;//2
string clientNick;//2






WORD GetRandomColor(SOCKET clientSocket){//2
    const WORD colors[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14};
    srand((clientSocket));
    return colors[rand() % 14 + 1]; 
}



DWORD WINAPI Sender(void* param)//2
{
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

    bool isFirstMessage = true; 
     cout << "Enter your nickname and message:\n";




   

    while (true) {
      
    
        string query;
        if (isFirstMessage) {


            getline(cin, query);
            clientNick = query.substr(0, query.find(' '));
            isFirstMessage = false; 
        }
        else
        {
            getline(cin, query);
        }
       
        string message = query.substr(query.find(' ') + 1);
        string fullMessage = clientNick + ": " + message;

       
        SetConsoleTextAttribute(h, clientColor);

        send(client_socket, fullMessage.c_str(), fullMessage.size(), 0);
    }
    //while (true) {
    //    // cout << "Please insert your query for server: ";
    //    char query[DEFAULT_BUFLEN];
    //    cin.getline(query, DEFAULT_BUFLEN);
    //    send(client_socket, query, strlen(query), 0);
    //}
}

DWORD WINAPI Receiver(void* param)//2
{
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
  

   

    while (true) {
        char response[DEFAULT_BUFLEN];
        int result = recv(client_socket, response, DEFAULT_BUFLEN, 0);
        response[result] = '\0';
        
        if (strcmp(response, "New client joined!") == 0) {
            cout << "\nA new client has joined.\n";
        }
        else if (strcmp(response, "Client left!") == 0) {
            cout << "\nA client has left.\n";
        }
        else {

            string message(response);
            string nickname = message.substr(0, message.find(':') + 2); 
            string actualMessage = message.substr(message.find(':') + 2); 

            SetConsoleTextAttribute(h, 7); 
            cout << nickname;
            

         
            clientColor = GetRandomColor(client_socket);
            SetConsoleTextAttribute(h, clientColor); 
            cout << actualMessage << "\n";

            SetConsoleTextAttribute(h, 7);
          
        }
      
    }
    SetConsoleTextAttribute(h, 7);
}





int main()
{
    // обработчик закрытия окна консоли
    //SetConsoleCtrlHandler((PHANDLER_ROUTINE)ExitHandler, true);

    system("title Client");

    // initialize Winsock
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }
    addrinfo hints = {};
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // разрешить адрес сервера и порт
    addrinfo* result = nullptr;
    iResult = getaddrinfo(SERVER_IP, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 2;
    }

    addrinfo* ptr = nullptr;
    // пытаться подключиться к адресу, пока не удастся
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
        // создать сокет на стороне клиента для подключения к серверу
        client_socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

        if (client_socket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return 3;
        }

        // connect to server
        iResult = connect(client_socket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(client_socket);
            client_socket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (client_socket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 5;
    }


   
    CreateThread(0, 0, Sender, 0, 0, 0);
    CreateThread(0, 0, Receiver, 0, 0, 0);


    Sleep(INFINITE);
}