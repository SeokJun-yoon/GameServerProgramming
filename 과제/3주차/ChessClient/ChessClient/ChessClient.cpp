// Chess.cpp : 애플리케이션에 대한 진입점을 정의합니다.

#include <iostream>
#include <TCHAR.H>
#include <WS2tcpip.h>
#include <windows.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
using namespace std;
#pragma comment (lib, "Ws2_32.lib")
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
constexpr int PORT = 50000;
constexpr int BUF_SIZE = 200;

char SERVER_ADDR[] = "0.0.0.0";
//constexpr char SERVER_ADDR[] = "127.0.0.1"; // 내 IP

#define KEY_DOWN 'D'
#define KEY_LEFT 'L'
#define KEY_RIGHT 'R'
#define KEY_UP 'U'

// 체스말 구조체
struct ChessPiece {
	short x, y, id;
	bool connect;
};

struct KEY
{
	char c_key;
	short key_id;
};

int r[10];
int g[10];
int b[10];

ChessPiece chesspiece;

// 전역 변수:
int WindowPosition_X = 400;
int WindowPosition_Y = 50;
int WindowSize_X = 660;
int WindowSize_Y = 680;
int interval = 80; // 간격 값

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
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpszCmdLine, int nCmdShow)
{
	HWND hwnd;
	MSG msg;
	WNDCLASS WndClass;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	WndClass.lpfnWndProc = (WNDPROC)WndProc;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = hInstance;
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.lpszMenuName = NULL;
	WndClass.lpszClassName = _T("Chess");
	RegisterClass(&WndClass);
	hwnd = CreateWindow(_T("Chess"),
		_T("Chess"),
		WS_OVERLAPPEDWINDOW,
		WindowPosition_X,
		WindowPosition_Y,
		WindowSize_X,
		WindowSize_Y,
		NULL,
		(HMENU)NULL,
		hInstance,
		NULL
	);

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg,
	WPARAM wParam, LPARAM lParam)
{
	HDC hdc, mdc;
	PAINTSTRUCT ps;
	HBRUSH hBrush;
	HBRUSH oldBrush;
	static HDC BackBuffer;
	static HBITMAP hBitmap, BackBit, BackoBit;
	COLORREF text_color;
	COLORREF bk_color;
	RECT rcClient;

	static RECT ChessBoard[8][8];

	static int x, y;
	static bool chess_piece = false;
	static bool socket_connect = false;

	static WSADATA WSAData;
	static SOCKET s_socket;
	static SOCKADDR_IN server_addr;

	static ChessPiece Chesspieces[10];
	static KEY keyInfo{ 0 };
	static char message_buffer[BUF_SIZE];

	switch (iMsg)
	{
	case WM_CREATE:
		WSAStartup(MAKEWORD(2, 2), &WSAData);
		memset(&server_addr, 0, sizeof(server_addr));

		while (true)
		{
			s_socket = WSASocket(AF_INET, SOCK_STREAM, 0, 0, 0, WSA_FLAG_OVERLAPPED);
			server_addr.sin_family = AF_INET;
			server_addr.sin_port = htons(PORT);

			cout << "서버 IP 주소 입력 :";
			cin >> SERVER_ADDR;
			inet_pton(AF_INET, SERVER_ADDR, &server_addr.sin_addr);
			//connect(s_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
			if (connect(s_socket, (struct sockaddr*) & server_addr, sizeof(server_addr)) == -1)
			{
				closesocket(s_socket);
				cout << "연결 실패. 주소를 다시 입력하세요!" << endl;
			}
			else
			{
				cout << "서버 연결 성공!" << endl;
				break;
			}
			/*recv(s_socket, message_buffer, sizeof(ChessPiece), 0);
			Chesspieces[0] = *(ChessPiece*)message_buffer;
			keyInfo.key_id = Chesspieces[0].id;

			socket_connect = true;
			chess_piece = true;*/
		}
		recv(s_socket, message_buffer, sizeof(ChessPiece), 0);
		Chesspieces[0] = *(ChessPiece*)message_buffer;
		keyInfo.key_id = Chesspieces[0].id;

		socket_connect = true;
		chess_piece = true;
		for (int i = 0; i < 10; ++i) {
			r[i] = rand() % 25 * 10;
			g[i] = rand() % 25 * 10;
			b[i] = rand() % 25 * 10;
		}

		SetTimer(hwnd, 1, 50, NULL);
		break;

	case WM_TIMER:
		switch (wParam)
		{
		case 1:
			if (true == socket_connect)
			{
				send(s_socket, (char*)&keyInfo, sizeof(KEY), 0);
				recv(s_socket, (char*)&Chesspieces, sizeof(Chesspieces), 0);
				keyInfo.c_key = 0;
			}
			break;
		}
		InvalidateRect(hwnd, NULL, FALSE);
		break;

	case WM_KEYFIRST:
		if (wParam == VK_RIGHT)
		{
			keyInfo.c_key = KEY_RIGHT;
		}
		else if (wParam == VK_LEFT)
		{
			keyInfo.c_key = KEY_LEFT;
		}
		else if (wParam == VK_UP)
		{
			keyInfo.c_key = KEY_UP;
		}
		else if (wParam == VK_DOWN)
		{
			keyInfo.c_key = KEY_DOWN;
		}
		InvalidateRect(hwnd, NULL, FALSE);
		break;

	case WM_PAINT:
		PAINTSTRUCT ps;
		hdc = BeginPaint(hwnd, &ps);
		GetClientRect(hwnd, &rcClient);

		BackBuffer = CreateCompatibleDC(hdc);

		BackoBit = CreateBitmap(rcClient.right - rcClient.left, rcClient.bottom - rcClient.top, 1, 32, NULL);
		hBitmap = (HBITMAP)SelectObject(BackBuffer, BackoBit);
		
		// 체스 판 출력 부분
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				ChessBoard[i][j].left = i * interval;
				ChessBoard[i][j].right = interval + i * interval;
				ChessBoard[i][j].top = j * interval;
				ChessBoard[i][j].bottom = interval + j * interval;
				if (i % 2 == 0 && j % 2 == 1) {
					hBrush = CreateSolidBrush(RGB(0, 0, 0));
					oldBrush = (HBRUSH)SelectObject(BackBuffer, hBrush);

					Rectangle(BackBuffer, ChessBoard[i][j].left, ChessBoard[i][j].top, ChessBoard[i][j].right, ChessBoard[i][j].bottom);
					SelectObject(BackBuffer, oldBrush);
					DeleteObject(hBrush);
				}

				else if (i % 2 == 1 && j % 2 == 0) {
					hBrush = CreateSolidBrush(RGB(0, 0, 0));
					oldBrush = (HBRUSH)SelectObject(BackBuffer, hBrush);

					Rectangle(BackBuffer, ChessBoard[i][j].left, ChessBoard[i][j].top, ChessBoard[i][j].right, ChessBoard[i][j].bottom);
					SelectObject(BackBuffer, oldBrush);
					DeleteObject(hBrush);
				}

				else
					Rectangle(BackBuffer, ChessBoard[i][j].left, ChessBoard[i][j].top, ChessBoard[i][j].right, ChessBoard[i][j].bottom);
			}
		}

		// 체스 말 출력 부분
		if (chess_piece == true)
		{
			for (int i = 0; i < 10; ++i)
			{
				if (Chesspieces[i].connect == true)
				{
					if (keyInfo.key_id == i)
					{
						hBrush = CreateSolidBrush(RGB(255, 0, 0));
						oldBrush = (HBRUSH)SelectObject(BackBuffer, hBrush);
						Ellipse(BackBuffer, Chesspieces[i].x, Chesspieces[i].y, Chesspieces[i].x + interval, Chesspieces[i].y + interval);
						SelectObject(BackBuffer, oldBrush);
						DeleteObject(hBrush);

						TCHAR str[10];
						wsprintf(str, TEXT("My Piece"));
						TextOut(BackBuffer, Chesspieces[i].x+10, Chesspieces[i].y+30, str, lstrlen(str));
					}
					else
					{
						hBrush = CreateSolidBrush(RGB(r[i], g[i], b[i]));
						oldBrush = (HBRUSH)SelectObject(BackBuffer, hBrush);
						Ellipse(BackBuffer, Chesspieces[i].x, Chesspieces[i].y, Chesspieces[i].x + interval, Chesspieces[i].y + interval);
						SelectObject(BackBuffer, oldBrush);
						DeleteObject(hBrush);

						TCHAR str[10];
						wsprintf(str, TEXT("%dPlayer"), i + 1);
						TextOut(BackBuffer, Chesspieces[i].x + 10, Chesspieces[i].y + 30, str, lstrlen(str));
					}
				}
			}
		}

		BitBlt(hdc, 0, 0, WindowSize_X, WindowSize_Y, BackBuffer, 0, 0, SRCCOPY);
		DeleteDC(BackBuffer);
		DeleteObject(hBitmap);
		EndPaint(hwnd, &ps);
		break;

	case WM_KEYUP:
		keyInfo.c_key = '0';
		break;

	case WM_DESTROY:
		SelectObject(BackBuffer, BackoBit);
		DeleteObject(BackBit);
		DeleteDC(BackBuffer);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, iMsg, wParam, lParam);
	}
	return 0;
}
