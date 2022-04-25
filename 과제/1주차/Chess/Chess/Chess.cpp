// Chess.cpp : 애플리케이션에 대한 진입점을 정의합니다.

#include <windows.h>
#include <TCHAR.H>
using namespace std;

// 체스말 구조체
struct ChessPiece {
    short x, y;
};

// 전역 변수:
int WindowPosition_X = 400;
int WindowPosition_Y = 50;
int WindowSize_X = 660;
int WindowSize_Y = 680;
int interval = 80; // 간격 값
ChessPiece chesspiece;

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

    switch (iMsg)
    {
    case WM_CREATE:
        chesspiece.x = interval * 3;
        chesspiece.y = interval * 3;
        break;

    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_LEFT:
            chesspiece.x -= interval;
            if (chesspiece.x < interval * 0)
                chesspiece.x = interval * 0;
            break;
        case VK_RIGHT:
            chesspiece.x += interval;
            if (chesspiece.x > interval * 7)
                chesspiece.x = interval * 7;
            break;
        case VK_DOWN:
            chesspiece.y += interval;
            if (chesspiece.y > interval * 7)
                chesspiece.y = interval * 7;
            break;
        case VK_UP:
            chesspiece.y -= interval;
            if (chesspiece.y < interval * 0)
                chesspiece.y = interval * 0;
            break;
        }
        InvalidateRect(hwnd, NULL, TRUE);

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
        PostQuitMessage(0);
        break;
    }
    return DefWindowProc(hwnd, iMsg, wParam, lParam);
}
