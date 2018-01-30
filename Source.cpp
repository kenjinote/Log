#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#pragma comment(lib, "shlwapi")

#include <windows.h>
#include <shlwapi.h>

TCHAR szClassName[] = TEXT("Window");

VOID Log(LPCWSTR lpszText)
{
	WCHAR szFilePath[MAX_PATH];
	GetModuleFileNameW(0, szFilePath, _countof(szFilePath));
	PathRenameExtensionW(szFilePath, L".log");
	HANDLE hFile = CreateFileW(szFilePath, GENERIC_WRITE, 0, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwWriteSize;
		if (GetLastError() == ERROR_ALREADY_EXISTS)
		{
			SetFilePointer(hFile, 0, 0, FILE_END);
		}
		else
		{
			BYTE bom[2] = { 0xFF, 0xFE };
			WriteFile(hFile, bom, 2, &dwWriteSize, NULL);
		}
		{
			SYSTEMTIME st;
			GetLocalTime(&st);
			WCHAR szText[256];
			wsprintfW(szText, L"[%04d-%02d-%02d %02d:%02d:%02d:%03d] ", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, lpszText);
			WriteFile(hFile, szText, lstrlenW(szText) * sizeof(WCHAR), &dwWriteSize, 0);
		}
		WriteFile(hFile, lpszText, lstrlenW(lpszText) * sizeof(WCHAR), &dwWriteSize, 0);
		WriteFile(hFile, L"\r\n", 2 * sizeof(WCHAR), &dwWriteSize, 0);
		CloseHandle(hFile);
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HWND hButton;
	switch (msg)
	{
	case WM_CREATE:
		hButton = CreateWindow(TEXT("BUTTON"), TEXT("ログを出力"), WS_VISIBLE | WS_CHILD | WS_TABSTOP, 0, 0, 0, 0, hWnd, (HMENU)IDOK, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		break;
	case WM_SIZE:
		MoveWindow(hButton, 10, 10, 256, 32, TRUE);
		break;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			Log(L"ログを出力してみる");
		}
		break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefDlgProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPSTR pCmdLine, int nCmdShow)
{
	MSG msg;
	WNDCLASS wndclass = {
		CS_HREDRAW | CS_VREDRAW,
		WndProc,
		0,
		DLGWINDOWEXTRA,
		hInstance,
		0,
		LoadCursor(0,IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		0,
		szClassName
	};
	RegisterClass(&wndclass);
	HWND hWnd = CreateWindow(
		szClassName,
		TEXT("ログを出力する"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		0,
		CW_USEDEFAULT,
		0,
		0,
		0,
		hInstance,
		0
	);
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, 0, 0, 0))
	{
		if (!IsDialogMessage(hWnd, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return (int)msg.wParam;
}
