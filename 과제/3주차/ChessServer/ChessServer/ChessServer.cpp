#include <iostream>
#include <WS2tcpip.h>
#include <map>
#include <list>
using namespace std;

#pragma comment(lib, "Ws2_32.lib") // Ws2_32 라이브러리 추가
#define PORT 50000
#define _CRT_SECURE_NO_WARINGS
#define _WINSOCK_DEPRECATED_NO_WARINGS
#define MAX_BUFFER 1024


#define KEY_DOWN 'D'
#define KEY_LEFT 'L'
#define KEY_RIGHT 'R'
#define KEY_UP 'U'

struct ChessPiece {
    short x, y, id;
    bool connect;
};

struct SOCKETINFO
{
    WSAOVERLAPPED s_over;
    WSABUF data_buffer;
    SOCKET socket;
    char message_buffer[MAX_BUFFER];
};

struct KEY
{
    char c_key;
    short key_id;
};


map <SOCKET, SOCKETINFO> clients;
ChessPiece Arr_PieceData[10]; // 최대 10개
list<int> indexList;

short index = 0;
void KeyMessage(const char* key, ChessPiece& piece_info);
void CALLBACK recv_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED s_over, DWORD recv_flag);
void CALLBACK send_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED s_over, DWORD send_flag);

void error_display(const char* msg, int err_no)
{
    WCHAR* lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, err_no,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&lpMsgBuf, 0, NULL);
    std::cout << msg;
    std::wcout << L"< ERROR ! > " << lpMsgBuf << std::endl;
    while (true);
    LocalFree(lpMsgBuf);
}


int interval = 80; // 간격 값
ChessPiece chesspiece;

int main()
{
    cout << "< START >" << endl;
    WSADATA WSAData;
    WSAStartup(MAKEWORD(2, 2), &WSAData);

    // 소켓 생성 단계 (IPv4, TCP)
    SOCKET s_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

    // 주소 설정 단계 (아이피 주소 / 포트 번호 설정)
    SOCKADDR_IN server_addr;
    ZeroMemory(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET; // IPv4
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // 누구나 접속 가능하도록

    // bind 단계 (소켓과 주소를 묶는다.)
    bind(s_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));

    // listen 단계 (클라이언트가 접속 가능한 대기 상태)
    listen(s_socket, SOMAXCONN);
    INT addr_size = sizeof(server_addr);
    SOCKADDR_IN clientAddr;
    ZeroMemory(&clientAddr, sizeof(addr_size));
    SOCKET c_socket;
    DWORD flags;
    
    for (int i = 0; i < 10; ++i)
    {
        indexList.push_back(i);
    }

    while (true) {
        c_socket = WSAAccept(s_socket, reinterpret_cast<sockaddr*>(&server_addr), &addr_size, 0, 0);
        if (c_socket == INVALID_SOCKET)
            cout << "INVALID SOCKET" << endl;
        else
            cout << "CONNECT !" << endl;

        if (clients.size()<10)
        {
            clients[c_socket] = SOCKETINFO{};
            memset(&clients[c_socket], 0, sizeof(struct SOCKETINFO));
            clients[c_socket].socket = c_socket;
            
            index = indexList.front();
            indexList.pop_front();

            Arr_PieceData[index] = ChessPiece{ 0,0,index,true };
            Arr_PieceData[index].id = index;
            clients[c_socket].data_buffer.len = sizeof(ChessPiece);
            clients[c_socket].data_buffer.buf = (char*)&Arr_PieceData[index];
            flags = 0;
            clients[c_socket].s_over.hEvent = (HANDLE)clients[c_socket].socket;
            WSASend(clients[c_socket].socket, &clients[c_socket].data_buffer, 1, NULL, 0, &(clients[c_socket].s_over), send_callback);
        }
    }
    closesocket(s_socket);
    WSACleanup();
}

void KeyMessage(const char* key, ChessPiece& piece_info)
{
	if (*key == KEY_LEFT) // 좌 방향키 입력 시
	{
		piece_info.x -= interval;
		if (piece_info.x < interval * 0)
            piece_info.x = interval * 0;
	}

	if (*key == KEY_RIGHT) // 우 방향키 입력 시
	{
        piece_info.x += interval;
		if (piece_info.x > interval * 7)
            piece_info.x = interval * 7;
	}

	if (*key == KEY_DOWN) // 하단 방향키 입력 시
	{
        piece_info.y += interval;
		if (piece_info.y > interval * 7)
            piece_info.y = interval * 7;
	}

	if (*key == KEY_UP) // 상단 방향키 입력 시
	{
        piece_info.y -= interval;
		if (piece_info.y < interval * 0)
            piece_info.y = interval * 0;
	}
}

void CALLBACK send_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED s_over, DWORD send_flag)
{
    DWORD receiveBytes = 0;
    DWORD flags = 0;
    SOCKET client_s = reinterpret_cast<int>(s_over->hEvent);
    if (num_bytes == 0)
    {
        closesocket(clients[client_s].socket);
        clients.erase(client_s);
        return;
    }
    clients[client_s].data_buffer.len = MAX_BUFFER;
    clients[client_s].data_buffer.buf = clients[client_s].message_buffer;
    memset(&(clients[client_s].s_over), 0x00, sizeof(WSAOVERLAPPED));
    clients[client_s].s_over.hEvent = (HANDLE)client_s;
    WSARecv(client_s, &clients[client_s].data_buffer, 1, 0, &flags, &(clients[client_s].s_over), recv_callback);
}
void CALLBACK recv_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED s_over, DWORD recv_flag)
{
    SOCKET client_s = reinterpret_cast<int>(s_over->hEvent);
    KEY keyInfo{};
    memcpy(&keyInfo, clients[client_s].message_buffer, sizeof(KEY)); // 받은 키 구조체 데이터 복사
    if (num_bytes == 0)
    {
        closesocket(clients[client_s].socket);
        clients.erase(client_s);
        Arr_PieceData[keyInfo.key_id] = ChessPiece{};
        indexList.push_front(keyInfo.key_id);
        return;
    }

    clients[client_s].message_buffer[num_bytes] = 0;
    KeyMessage(&keyInfo.c_key, Arr_PieceData[keyInfo.key_id]);
    
    clients[client_s].data_buffer.len = sizeof(Arr_PieceData);
    clients[client_s].data_buffer.buf = (char*)&Arr_PieceData;

    memset(&(clients[client_s].s_over), 0x00, sizeof(WSAOVERLAPPED));
    clients[client_s].s_over.hEvent = (HANDLE)client_s;
    WSASend(client_s, &(clients[client_s].data_buffer), 1, &num_bytes, 0, &(clients[client_s].s_over), send_callback);
}
