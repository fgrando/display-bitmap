// useful sorces:
// https://docs.microsoft.com/en-us/cpp/windows/walkthrough-creating-windows-desktop-applications-cpp?view=msvc-160

#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>


// instead of loading the bmp as a separate file, we will embbed it as binary data
#include "sample.bmp.h"
// the picture is the BIN_DATA





// The main window class name.
static TCHAR szWindowClass[] = _T("DesktopApp");

// The string that appears in the application's title bar.
static TCHAR szTitle[] = _T("Windows Desktop Guided Tour Application");

HINSTANCE hInst;

// Forward declarations of functions included in this code module:
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR     lpCmdLine,
    _In_ int       nCmdShow
)
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(wcex.hInstance, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

    if (!RegisterClassEx(&wcex))
    {
        MessageBox(NULL,
            _T("Call to RegisterClassEx failed!"),
            _T("Windows Desktop Guided Tour"),
            NULL);

        return 1;
    }

    // Store instance handle in our global variable
    hInst = hInstance;

    // The parameters to CreateWindowEx explained:
    // WS_EX_OVERLAPPEDWINDOW : An optional extended window style.
    // szWindowClass: the name of the application
    // szTitle: the text that appears in the title bar
    // WS_OVERLAPPEDWINDOW: the type of window to create
    // CW_USEDEFAULT, CW_USEDEFAULT: initial position (x, y)
    // 500, 100: initial size (width, length)
    // NULL: the parent of this window
    // NULL: this application does not have a menu bar
    // hInstance: the first parameter from WinMain
    // NULL: not used in this application
    HWND hWnd = CreateWindowEx(
        WS_EX_OVERLAPPEDWINDOW,
        szWindowClass,
        szTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        500, 100,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (!hWnd)
    {
        MessageBox(NULL,
            _T("Call to CreateWindow failed!"),
            _T("Windows Desktop Guided Tour"),
            NULL);

        return 1;
    }

    // The parameters to ShowWindow explained:
    // hWnd: the value returned from CreateWindow
    // nCmdShow: the fourth parameter from WinMain
    ShowWindow(hWnd,
        nCmdShow);
    UpdateWindow(hWnd);

    // Main message loop:
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

void build_bmp(const uint8_t* bmp, HBITMAP& hBitmap, BITMAP& bitmap) {

    tagBITMAPFILEHEADER bfh = *(tagBITMAPFILEHEADER*)bmp;
    tagBITMAPINFOHEADER bih = *(tagBITMAPINFOHEADER*)(bmp + sizeof(tagBITMAPFILEHEADER));
    RGBQUAD             rgb = *(RGBQUAD*)(bmp + sizeof(tagBITMAPFILEHEADER) + sizeof(tagBITMAPINFOHEADER));

    BITMAPINFO bi;
    bi.bmiColors[0] = rgb;
    bi.bmiHeader = bih;

    const uint8_t* pPixels = (bmp + bfh.bfOffBits);
    char* ppvBits;

    hBitmap = CreateDIBSection(NULL, &bi, DIB_RGB_COLORS, (void**)&ppvBits, NULL, 0);
    SetDIBits(NULL, hBitmap, 0, bih.biHeight, pPixels, &bi, DIB_RGB_COLORS);

    GetObject(hBitmap, sizeof(BITMAP), &bitmap);
}

//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HBITMAP hBitmap = NULL;
    static BITMAP  bitmap;

    switch (message)
    {
    case WM_CREATE:
        build_bmp(BIN_DATA, hBitmap, bitmap);
        break;

    case WM_PAINT:

        PAINTSTRUCT     ps;
        HDC             hdc;
        HDC             hdcMem;
        HGDIOBJ         oldBitmap;

        hdc = BeginPaint(hWnd, &ps);

        hdcMem = CreateCompatibleDC(hdc);
        oldBitmap = SelectObject(hdcMem, hBitmap);

        GetObject(hBitmap, sizeof(bitmap), &bitmap);

        BitBlt(hdc, 0, 0, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, SRCCOPY);

        SelectObject(hdcMem, oldBitmap);
        DeleteDC(hdcMem);

        EndPaint(hWnd, &ps);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
        break;
    }

    return 0;
}