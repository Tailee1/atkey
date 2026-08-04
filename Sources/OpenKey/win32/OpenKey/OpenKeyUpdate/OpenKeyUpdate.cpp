/*----------------------------------------------------------
OpenKey - The Cross platform Open source Vietnamese Keyboard application.

Copyright (C) 2019 Mai Vu Tuyen
Contact: maivutuyen.91@gmail.com
Github: https://github.com/tuyenvm/OpenKey
Fanpage: https://www.facebook.com/OpenKeyVN

This file is belong to the OpenKey project, Win32 version
which is released under GPL license.
You can fork, modify, improve this program. If you
redistribute your new version, it MUST be open source.
-----------------------------------------------------------*/

#include "framework.h"
#include "OpenKeyUpdate.h"
#include <Urlmon.h>
#include <shellapi.h>
#include <fstream>
#include <sstream>
#include <string>
#pragma comment(lib, "Urlmon.lib")

using namespace std;

INT_PTR CALLBACK MainDialogProcess(HWND, UINT, WPARAM, LPARAM);
void StartUpdate();
HWND hDlg;

//Duong dan day du cua file exe can ghi de, do ATKey truyen sang qua dong lenh.
//Nho vay updater ghi de dung file dang chay du no ten ATKey64.exe hay atkey.exe.
wstring gTargetExe;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);

	gTargetExe = lpCmdLine ? lpCmdLine : L"";
	//ShellExecute boc duong dan trong dau nhay kep, bo di truoc khi dung
	if (gTargetExe.size() >= 2 && gTargetExe.front() == L'"' && gTargetExe.back() == L'"') {
		gTargetExe = gTargetExe.substr(1, gTargetExe.size() - 2);
	}
	if (gTargetExe.empty()) {
		//Ban cu khong truyen tham so: doan theo thu muc chua updater.
		WCHAR self[MAX_PATH];
		GetModuleFileName(NULL, self, MAX_PATH);
		wstring dir(self);
		gTargetExe = dir.substr(0, dir.find_last_of(L'\\')) + L"\\ATKey64.exe";
	}

	hDlg = CreateDialogParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG_UPDATER), 0, MainDialogProcess, 0);
	ShowWindow(hDlg, SW_SHOWNORMAL);
 
	MSG msg;
	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0)) {
		if (!IsDialogMessage(hDlg, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK MainDialogProcess(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
	switch (message) {
	case WM_INITDIALOG:{
		HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_OPENKEYUPDATE));
		if (hIcon) {
			SendMessage(hDlg, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
		}
		StartUpdate();
		return (INT_PTR)TRUE;
	}
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

DWORD WINAPI UpdateThreadFunction(LPVOID lpParam) {
	//Tai file tam vao CUNG thu muc voi exe dich - MoveFileEx chi ghi de duoc
	//trong cung o dia, va GetCurrentDirectory khong dung duoc o day: khi ATKey
	//chay luc khoi dong Windows thi thu muc hien hanh thuong la System32.
	wstring targetDir = gTargetExe.substr(0, gTargetExe.find_last_of(L'\\'));
	wstring tmp = targetDir + L"\\_ATKeyUpdate.tmp";

	//ATKey phat hanh thang file exe tai mot duong dan co dinh, khong dong goi zip.
	//Vi vay khong can tai version.json ve de ghep ten file nua: ung dung chinh da
	//so phien ban truoc khi goi toi day roi.
	DeleteFile(tmp.c_str());
	HRESULT res = URLDownloadToFile(NULL, L"https://atkey.org/atkey.exe", tmp.c_str(), 0, NULL);

	if (res != S_OK) {
		MessageBox(hDlg, _T("Có lỗi trong quá trình cập nhật, vui lòng thử lại sau!"), _T("ATKey Update"), MB_OK);
		ExitProcess(0);
		return 0;
	}

	//Ung dung chinh vua thoat nhung file exe cua no co the con bi khoa vai giay.
	//Ghi de bang MoveFileEx thay vi xoa truoc roi chep sau: neu xoa duoc ma buoc
	//sau that bai thi nguoi dung mat han ung dung. Ban goc mac dung loi nay.
	bool replaced = false;
	for (int i = 0; i < 20; i++) {
		if (MoveFileEx(tmp.c_str(), gTargetExe.c_str(), MOVEFILE_REPLACE_EXISTING)) {
			replaced = true;
			break;
		}
		Sleep(500);
	}

	if (replaced) {
		MessageBox(hDlg, _T("Bạn đã cập nhật ATKey bản mới nhất thành công!"), _T("ATKey Update"), MB_OK);
		ShellExecute(0, L"", gTargetExe.c_str(), 0, targetDir.c_str(), SW_SHOWNORMAL);
	} else {
		DeleteFile(tmp.c_str());
		MessageBox(hDlg, _T("Không ghi đè được ATKey. Hãy đóng ATKey rồi thử lại!"), _T("ATKey Update"), MB_OK);
	}
	ExitProcess(0);
	return 0;
}

void StartUpdate() {
	DWORD hThread;
	HANDLE t = CreateThread(
							NULL,                   // default security attributes
							0,                      // use default stack size  
							UpdateThreadFunction,       // thread function name
							0,          // argument to thread function 
							0,                      // use default creation flags 
							&hThread);   // returns the thread identifier 
}