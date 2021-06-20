#include"DirectX.h"
using namespace std;
bool gameover = false;
LRESULT WINAPI WinProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_DESTROY:
		gameover = true;
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}
int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprevinstance, LPSTR lpcmdline, int ncmdshow)
{
	WNDCLASSEX wc;
	RECT rc = { 100,100,1124,868 };
	AdjustWindowRectEx(&rc, WS_OVERLAPPEDWINDOW, FALSE, NULL);
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)WinProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hinstance;
	wc.hIcon = LoadIcon(hinstance, MAKEINTRESOURCE(IDI_ICON1));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "MainWindowClass";
	wc.hIconSm = NULL;
	RegisterClassEx(&wc);
	HWND window = CreateWindow("MainWindowClass", APPTITLE.c_str(), WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX& ~WS_THICKFRAME, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, NULL, NULL, hinstance, NULL);
	if (window == 0)return 0;
	ShowWindow(window, ncmdshow);
	UpdateWindow(window);
	if (!Game_init(window))return 0;
	MSG message;
	while (!gameover)
	{
		if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		Game_run(window);
	}
	Game_end();
	return message.wParam;
}