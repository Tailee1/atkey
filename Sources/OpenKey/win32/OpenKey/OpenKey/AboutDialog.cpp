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
#include "stdafx.h"
#include "AboutDialog.h"
#include "AppDelegate.h"

AboutDialog::AboutDialog(const HINSTANCE & hInstance, const int & resourceId)
	: BaseDialog(hInstance, resourceId) {
}

AboutDialog::~AboutDialog() {
}

void AboutDialog::fillData() {
}

INT_PTR AboutDialog::eventProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_INITDIALOG:
		this->hDlg = hDlg;
		initDialog();
		return TRUE;
	case WM_COMMAND: {
		int wmId = LOWORD(wParam);
		switch (wmId) {
		case IDM_EXIT:
		case IDBUTTON_OK:
			AppDelegate::getInstance()->closeDialog(this);
			break;
		case IDC_BUTTON_CHECK_VERSION:
			onUpdateButton();
			break;
		}
		break;
	}
	case WM_NOTIFY:
		switch (((LPNMHDR)lParam)->code) {
		case NM_CLICK:
		case NM_RETURN: {
			PNMLINK link = (PNMLINK)lParam;
			if (link->hdr.idFrom == IDC_SYSLINK_HOME_PAGE)
				ShellExecute(NULL, _T("open"), _T("https://atkey.org"), NULL, NULL, SW_SHOWNORMAL);
			else if (link->hdr.idFrom == IDC_SYSLINK_NEW_VERSION)
				ShellExecute(NULL, _T("open"), _T("https://atkey.org"), NULL, NULL, SW_SHOWNORMAL);
			else if (link->hdr.idFrom == IDC_SYSLINK_FANPAGE)
				ShellExecute(NULL, _T("open"), _T("https://github.com/tuyenvm/OpenKey"), NULL, NULL, SW_SHOWNORMAL);
		}
		break;
		}
	}
	return FALSE;
}

void AboutDialog::initDialog() {
	//dialog icon
	SET_DIALOG_ICON(IDI_APP_ICON);
	
	hUpdateButton = GetDlgItem(hDlg, IDC_BUTTON_CHECK_VERSION);

	HFONT hFont = CreateFont(48, 0, 0, 0, FW_THIN, FALSE, FALSE, FALSE,
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 
		DEFAULT_PITCH | FF_SWISS | FF_MODERN, _T("Segoe UI"));
	SendDlgItemMessage(hDlg, IDC_STATIC_APP_TITLE, WM_SETFONT, WPARAM(hFont), TRUE);

	wchar_t buffer[256];
	wsprintfW(buffer, _T("Phiên bản %s cho Windows - Ngày cập nhật: %s"), OpenKeyHelper::getVersionString().c_str(), _T(__DATE__));
	SendDlgItemMessage(hDlg, IDC_STATIC_APP_VERSION, WM_SETTEXT, 0, LPARAM(buffer));

	hFont = CreateFont(20, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE,
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS | FF_MODERN, _T("Arial"));
	SendDlgItemMessage(hDlg, IDC_STATIC_APP_SUB_TITLE, WM_SETFONT, WPARAM(hFont), TRUE);
}

void AboutDialog::onUpdateButton() {
	EnableWindow(hUpdateButton, false);
	string newVersion;
	if (OpenKeyManager::checkUpdate(newVersion)) {
		WCHAR msg[256];
		wsprintf(msg, 
			TEXT("ATKey Có phiên bản mới (%s), bạn có muốn cập nhật không?"),
			utf8ToWideString(newVersion).c_str());

		int msgboxID = MessageBox(
			hDlg,
			msg,
			_T("ATKey Update"),
			MB_ICONEXCLAMATION | MB_YESNO
		);
		if (msgboxID == IDYES) {
			//Call OpenKeyUpdate
			//Dung thu muc chua exe chu khong phai GetCurrentDirectory: khi ATKey chay
			//luc khoi dong Windows thi thu muc hien hanh thuong la System32, updater
			//se ghi file ra nham cho. Truyen duong dan exe hien tai sang de updater
			//ghi de dung file dang chay, bat ke ten la ATKey64.exe hay atkey.exe.
			wstring selfPath = OpenKeyHelper::getFullPath();
			wstring appDir = selfPath.substr(0, selfPath.find_last_of(L'\\'));
			wstring updaterPath = appDir + L"\\OpenKeyUpdate.exe";
			wstring updaterArgs = L"\"" + selfPath + L"\"";
			//Chi thoat khi da chay duoc updater. Neu nguoi dung chi tai moi file exe
			//ma khong co OpenKeyUpdate.exe ben canh thi khong duoc thoat, neu khong
			//ho bam "Kiem tra ban moi" xong thay ung dung bien mat ma chua cap nhat gi.
			HINSTANCE started = ShellExecute(0, L"", updaterPath.c_str(), updaterArgs.c_str(), appDir.c_str(), SW_SHOWNORMAL);
			if ((INT_PTR)started <= 32) {
			    MessageBox(0, _T("Thiếu tập tin OpenKeyUpdate.exe nên không thể tự cập nhật.\nVui lòng tải bản mới thủ công tại https://atkey.org"), _T("ATKey Update"), MB_ICONEXCLAMATION | MB_OK);
			    return;
			}

			AppDelegate::getInstance()->onOpenKeyExit();
		}
		
	} else {
		MessageBox(hDlg, _T("Bạn đang dùng phiên bản mới nhất!"), _T("ATKey Update"), MB_OK);
	}
	EnableWindow(hUpdateButton, true);
}