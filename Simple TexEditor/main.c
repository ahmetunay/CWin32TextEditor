#include <windows.h>
#include <commctrl.h>
#include "resource3.h"
const char g_szClassName[] = "Text";
#define IDC_MAIN_EDIT	101
#define IDC_MAIN_TOOL	102
#define IDC_MAIN_STATUS	103

BOOL LoadTextFileToEdit(HWND Duzenle, LPCTSTR pszFileName){
	HANDLE dosya;
	BOOL bSuccess = FALSE;

	dosya = CreateFile(pszFileName, GENERIC_READ, FILE_SHARE_READ, NULL,OPEN_EXISTING, 0, NULL);
	if(dosya != INVALID_HANDLE_VALUE)
	{
		DWORD dwFileSize;

		dwFileSize = GetFileSize(dosya, NULL);
		if(dwFileSize != 0xFFFFFFFF)
		{
			LPSTR pszFileText;

			pszFileText = (LPSTR)GlobalAlloc(GPTR, dwFileSize + 1);
			if(pszFileText != NULL)
			{
				DWORD dwRead;

				if(ReadFile(dosya, pszFileText, dwFileSize, &dwRead, NULL))
				{
					pszFileText[dwFileSize] = 0;
					if(SetWindowText(Duzenle, pszFileText))
						bSuccess = TRUE;
				}
				GlobalFree(pszFileText);
			}
		}
		CloseHandle(dosya);
	}
	return bSuccess;
}

BOOL SaveTextFileFromEdit(HWND Duzenle, LPCTSTR pszFileName){
	HANDLE dosya;
	BOOL bSuccess = FALSE;

	dosya = CreateFile(pszFileName, GENERIC_WRITE, 0, NULL,CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(dosya != INVALID_HANDLE_VALUE)
	{
		DWORD dwTextLength;

		dwTextLength = GetWindowTextLength(Duzenle);

		if(dwTextLength > 0)
		{
			LPSTR pszText;
			DWORD dwBufferSize = dwTextLength + 1;

			pszText = (LPSTR)GlobalAlloc(GPTR, dwBufferSize);
			if(pszText != NULL)
			{
				if(GetWindowText(Duzenle, pszText, dwBufferSize))
				{
					DWORD dwWritten;

					if(WriteFile(dosya, pszText, dwTextLength, &dwWritten, NULL))
						bSuccess = TRUE;
				}
				GlobalFree(pszText);
			}
		}
		CloseHandle(dosya);
	}
	return bSuccess;
}

void Ac(HWND pencere){
	OPENFILENAME dosyadi;
	char szFileName[MAX_PATH] = "";

	ZeroMemory(&dosyadi, sizeof(dosyadi));

	dosyadi.lStructSize = sizeof(dosyadi);
	dosyadi.hwndOwner = pencere;
	dosyadi.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
	dosyadi.lpstrFile = szFileName;
	dosyadi.nMaxFile = MAX_PATH;
	dosyadi.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	dosyadi.lpstrDefExt = "txt";

	if(GetOpenFileName(&dosyadi))
	{
		HWND Duzenle = GetDlgItem(pencere, IDC_MAIN_EDIT);
		if(LoadTextFileToEdit(Duzenle, szFileName))
		{
			SendDlgItemMessage(pencere, IDC_MAIN_STATUS, SB_SETTEXT, 0, (LPARAM)"Açildi");
			SendDlgItemMessage(pencere, IDC_MAIN_STATUS, SB_SETTEXT, 1, (LPARAM)szFileName);
		}
	}
}

void Kaydet(HWND pencere){
	OPENFILENAME dosyadi;
	char szFileName[MAX_PATH] = "";

	ZeroMemory(&dosyadi, sizeof(dosyadi));

	dosyadi.lStructSize = sizeof(dosyadi);
	dosyadi.hwndOwner = pencere;
	dosyadi.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
	dosyadi.lpstrFile = szFileName;
	dosyadi.nMaxFile = MAX_PATH;
	dosyadi.lpstrDefExt = "txt";
	dosyadi.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;

	if(GetSaveFileName(&dosyadi))
	{
		HWND Duzenle = GetDlgItem(pencere, IDC_MAIN_EDIT);
		if(SaveTextFileFromEdit(Duzenle, szFileName))
		{
			SendDlgItemMessage(pencere, IDC_MAIN_STATUS, SB_SETTEXT, 0, (LPARAM)"Saved");
			SendDlgItemMessage(pencere, IDC_MAIN_STATUS, SB_SETTEXT, 1, (LPARAM)szFileName);
		}
	}
}

LRESULT CALLBACK WndProc(HWND pencere, UINT msg, WPARAM wParam, LPARAM lParam){
	switch(msg)
	{
		case WM_CREATE:
		{
			HFONT hfDefault;
			HWND Duzenle;

			HWND toolbar;
			TBBUTTON button[3];
			TBADDBITMAP tbab;

			HWND hStatus;
			int statwidths[] = {100, -1};


			///Text Area

   Duzenle = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "",WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
    0, 0, 100, 100, pencere, (HMENU)IDC_MAIN_EDIT, GetModuleHandle(NULL), NULL);
			if(Duzenle == NULL)
            MessageBox(pencere, "Messagebox can't create", "Error", MB_OK | MB_ICONERROR);

			hfDefault = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
			SendMessage(Duzenle, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(FALSE, 0));


			///Tool Bar olustur

			toolbar = CreateWindowEx(0, TOOLBARCLASSNAME, NULL, WS_CHILD | WS_VISIBLE, 0, 0, 0, 0,pencere, (HMENU)IDC_MAIN_TOOL, GetModuleHandle(NULL), NULL);
			if(toolbar == NULL)
            MessageBox(pencere, "Tool Bar Olusturulamadi.", "Error", MB_OK | MB_ICONERROR);


			SendMessage(toolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);

			tbab.hInst = HINST_COMMCTRL;
			tbab.nID = IDB_STD_SMALL_COLOR;
			SendMessage(toolbar, TB_ADDBITMAP, 0, (LPARAM)&tbab);


            ///Create Button
			ZeroMemory(button, sizeof(button));
			button[0].iBitmap = STD_FILENEW;
			button[0].fsState = TBSTATE_ENABLED;
			button[0].fsStyle = TBSTYLE_BUTTON;
			button[0].idCommand = ID_FILE_NEW;

			button[1].iBitmap = STD_FILEOPEN;
			button[1].fsState = TBSTATE_ENABLED;
			button[1].fsStyle = TBSTYLE_BUTTON;
			button[1].idCommand = ID_FILE_OPEN;

			button[2].iBitmap = STD_FILESAVE;
			button[2].fsState = TBSTATE_ENABLED;
			button[2].fsStyle = TBSTYLE_BUTTON;
			button[2].idCommand = ID_FILE_SAVEAS;

			SendMessage(toolbar, TB_ADDBUTTONS, sizeof(button)/sizeof(TBBUTTON), (LPARAM)&button);


			///Durum Çubugu yaratma
            hStatus = CreateWindowEx(0, STATUSCLASSNAME, NULL,WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP, 0, 0, 0, 0,pencere, (HMENU)IDC_MAIN_STATUS, GetModuleHandle(NULL), NULL);

			SendMessage(hStatus, SB_SETPARTS, sizeof(statwidths)/sizeof(int), (LPARAM)statwidths);
			SendMessage(hStatus, SB_SETTEXT, 0, (LPARAM)"Welcome Text Editor");
		}
		break;
		case WM_SIZE:
		{
			HWND toolbar;
			RECT rcTool;
			int iToolHeight;

			HWND hStatus;
			RECT rcStatus;
			int iStatusHeight;

			HWND Duzenle;
			int iEditHeight;
			RECT rcClient;


			///Tool Box width and height

			toolbar = GetDlgItem(pencere, IDC_MAIN_TOOL);
			SendMessage(toolbar, TB_AUTOSIZE, 0, 0);

			GetWindowRect(toolbar, &rcTool);
			iToolHeight = rcTool.bottom - rcTool.top;

			///Status bar  width and height


			hStatus = GetDlgItem(pencere, IDC_MAIN_STATUS);
			SendMessage(hStatus, WM_SIZE, 0, 0);

			GetWindowRect(hStatus, &rcStatus);
			iStatusHeight = rcStatus.bottom - rcStatus.top;



			GetClientRect(pencere, &rcClient);

			iEditHeight = rcClient.bottom - iToolHeight - iStatusHeight;

			Duzenle = GetDlgItem(pencere, IDC_MAIN_EDIT);
			SetWindowPos(Duzenle, NULL, 0, iToolHeight, rcClient.right, iEditHeight, SWP_NOZORDER);
		}
		break;
		case WM_CLOSE:
			DestroyWindow(pencere);
		break;
		case WM_DESTROY:
			PostQuitMessage(0);
		break;
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case ID_FILE_EXIT:
					PostMessage(pencere, WM_CLOSE, 0, 0);
				break;
				case ID_FILE_NEW:
					SetDlgItemText(pencere, IDC_MAIN_EDIT, "");
				break;
				case ID_FILE_OPEN:
					Ac(pencere);
				break;
				case ID_FILE_SAVEAS:
					Kaydet(pencere);
				break;
			}
		break;
		default:
			return DefWindowProc(pencere, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow){

	WNDCLASSEX wc;
	HWND pencere;
	MSG Msg;


	wc.cbSize		 = sizeof(WNDCLASSEX);
	wc.style		 = 0;
	wc.lpfnWndProc	 = WndProc;
	wc.cbClsExtra	 = 0;
	wc.cbWndExtra	 = 0;
	wc.hInstance	 = hInstance;
	wc.hIcon		 = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor		 = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName  = MAKEINTRESOURCE(IDR_MAINMENU);
	wc.lpszClassName = g_szClassName;
	wc.hIconSm		 = LoadIcon(NULL, IDI_APPLICATION);

	if(!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "Windows Register is Failed", "Eror ! ",MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	pencere = CreateWindowEx(0,g_szClassName,"Text Editor",WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,CW_USEDEFAULT,
                       CW_USEDEFAULT, 500, 500,NULL, NULL, hInstance, NULL);

	if(pencere == NULL)
	{
		MessageBox(NULL, "Window can't create", "Eror!",MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	ShowWindow(pencere, nCmdShow);
	UpdateWindow(pencere);

	while(GetMessage(&Msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	return Msg.wParam;
}
