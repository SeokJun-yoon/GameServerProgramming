// Chess.cpp : 애플리케이션에 대한 진입점을 정의합니다.

#include <iostream>
#include <TCHAR.H>
#include <WS2tcpip.h>
#include <windows.h>
#define WIN32_LEAN_AND_MEAN
using namespace std;
#pragma comment (lib, "Ws2_32.lib")
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
constexpr int PORT = 50000;

char SERVER_ADDR[] = "0.0.0.0";
//constexpr char SERVER_ADDR[] = "127.0.0.1"; // 내 IP
// 체스말 구조체
struct ChessPiece {
	short x, y;
	char command;
};

ChessPiece chesspiece;

// 전역 변수:
int WindowPosition_X = 400;
int WindowPosition_Y = 50;
int WindowSize_X = 660;
int WindowSize_Y = 680;
int interval = 80; // 간격 값
bool is_connected = false;

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


LRESULT CALLBACK    WndProc(HWND hwnd, UINT iMsg,
	WPARAM wParam, LPARAM IParam);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpszCmdLine, int nCmdShow)
{
	HWND hwnd;
	MSG msg;
	WNDCLASS WndClass;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	WndClass.lpfnWndProc = WndProc;
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
		NULL,
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
	HDC hdc;
	PAINTSTRUCT ps;
	HBRUSH hBrush;
	HBRUSH oldBrush;

	static RECT ChessBoard[8][8];

	static WSADATA WSAData;
	static SOCKET s_socket;
	static SOCKADDR_IN server_addr;

	switch (iMsg)
	{
	case WM_CREATE:
		memset(&server_addr, 0, sizeof(server_addr));
		WSAStartup(MAKEWORD(2, 2), &WSAData);

		while (true)
		{
			s_socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, 0);
			server_addr.sin_family = AF_INET;
			server_addr.sin_port = htons(PORT);

			cout << "서버 IP 주소 입력 :";
			cin >> SERVER_ADDR;
			inet_pton(AF_INET, SERVER_ADDR, &server_addr.sin_addr);
			//connect(s_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
			if (connect(s_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
			{
				closesocket(s_socket);
				cout << "연결 실패. 주소를 다시 입력하세요!" << endl;
			}
			else
			{
				cout << "서버 연결 성공!" << endl;
				break;
			}

		}

		chesspiece.x = interval * 3;
		chesspiece.y = interval * 3;

		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_LEFT:
			chesspiece.command = 'L';
			send(s_socket, (char*)&chesspiece, sizeof(chesspiece), 0);
			break;

		case VK_RIGHT:
			chesspiece.command = 'R';
			send(s_socket, (char*)&chesspiece, sizeof(chesspiece), 0);
			break;
		case VK_DOWN:
			chesspiece.command = 'D';
			send(s_socket, (char*)&chesspiece, sizeof(chesspiece), 0);
			break;

		case VK_UP:
			chesspiece.command = 'U';
			send(s_socket, (char*)&chesspiece, sizeof(chesspiece), 0);
			break;
		}

		recv(s_socket, (char*)&chesspiece, sizeof(ChessPiece), 0);
		InvalidateRect(hwnd, NULL, TRUE);
		break;

	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		// 체스 판 출력 부분
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				ChessBoard[i][j].left = i * interval;
				ChessBoard[i][j].right = interval + i * interval;
				ChessBoard[i][j].top = j * interval;
				ChessBoard[i][j].bottom = interval + j * interval;
				if (i % 2 == 0 && j % 2 == 1) {
					hBrush = CreateSolidBrush(RGB(0, 0, 0));
					oldBrush = (HBRUSH)SelectObject(hdc, hBrush);

					Rectangle(hdc, ChessBoard[i][j].left, ChessBoard[i][j].top, ChessBoard[i][j].right, ChessBoard[i][j].bottom);
					SelectObject(hdc, oldBrush);
					DeleteObject(hBrush);
				}

				else if (i % 2 == 1 && j % 2 == 0) {
					hBrush = CreateSolidBrush(RGB(0, 0, 0));
					oldBrush = (HBRUSH)SelectObject(hdc, hBrush);

					Rectangle(hdc, ChessBoard[i][j].left, ChessBoard[i][j].top, ChessBoard[i][j].right, ChessBoard[i][j].bottom);
					SelectObject(hdc, oldBrush);
					DeleteObject(hBrush);
				}

				else
					Rectangle(hdc, ChessBoard[i][j].left, ChessBoard[i][j].top, ChessBoard[i][j].right, ChessBoard[i][j].bottom);
			}
		}

		// 체스 말 출력 부분
		hBrush = CreateSolidBrush(RGB(255, 0, 0));
		oldBrush = (HBRUSH)SelectObject(hdc, hBrush);

		Ellipse(hdc, chesspiece.x, chesspiece.y, chesspiece.x + interval, chesspiece.y + interval);
		EndPaint(hwnd, &ps);
		break;

	case WM_DESTROY:
		closesocket(s_socket);
		WSACleanup();
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}
