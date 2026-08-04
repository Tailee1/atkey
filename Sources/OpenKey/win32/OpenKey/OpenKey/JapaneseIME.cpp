/*----------------------------------------------------------
OpenKey - The Cross platform Open source Vietnamese Keyboard application.

This file is belong to the OpenKey project, Win32 version
which is released under GPL license.
You can fork, modify, improve this program. If you
redistribute your new version, it MUST be open source.
-----------------------------------------------------------*/

#include "stdafx.h"
#include "JapaneseIME.h"
#include <imm.h>

// Windows SDK hien tai khong khai bao cac hang so nay du chung van duoc
// tai lieu hoa cho WM_IME_CONTROL, nen phai tu dinh nghia.
#ifndef IMC_GETCONVERSIONMODE
#define IMC_GETCONVERSIONMODE	0x0001
#endif
#ifndef IMC_GETOPENSTATUS
#define IMC_GETOPENSTATUS		0x0005
#endif

// Doi mot chut truoc khi doc, vi ngay sau khi bam 半角/全角 thi
// IME chua kip cap nhat trang thai.
#define REFRESH_DELAY_MS  40

static volatile LONG _isNativeMode = 0;   // cache, hook doc bien nay
static HANDLE _refreshEvent = NULL;
static HANDLE _quitEvent = NULL;
static HANDLE _worker = NULL;

//
// Doc trang thai IME cua cua so dang focus.
// Fail-safe: bat cu khi nao khong doc duoc (app TSF/UWP khong tra loi,
// app treo, khong phai layout Nhat) deu tra ve false = KHONG tat tieng
// Viet. Doan sai theo huong nay chi lam mat tinh nang, con doan sai
// nguoc lai se lam nguoi dung go tieng Viet khong duoc.
//
static bool queryJapaneseNativeMode() {
	HWND fg = GetForegroundWindow();
	if (!fg)
		return false;

	DWORD pid = 0;
	DWORD tid = GetWindowThreadProcessId(fg, &pid);
	if (!tid)
		return false;

	// Chi IME kieu CJK moi co khai niem "che do native". Kiem tra som o day
	// cung giup khoi phai goi SendMessage khi dang dung layout thong thuong
	// (US, VN...). Ban goc cua OpenKey tat tieng Viet voi moi IME dang bat,
	// nen giu ca Trung/Han de khong lam thut lui hanh vi cu.
	HKL hkl = GetKeyboardLayout(tid);
	LANGID lang = PRIMARYLANGID((LANGID)(((DWORD_PTR)hkl) & 0xFFFF));
	if (lang != LANG_JAPANESE && lang != LANG_CHINESE && lang != LANG_KOREAN)
		return false;

	// Cua so focus that su trong thread do, chinh xac hon foreground window.
	HWND target = fg;
	GUITHREADINFO gti;
	ZeroMemory(&gti, sizeof(gti));
	gti.cbSize = sizeof(gti);
	if (GetGUIThreadInfo(tid, &gti) && gti.hwndFocus)
		target = gti.hwndFocus;

	HWND hIme = ImmGetDefaultIMEWnd(target);
	if (!hIme)
		return false;

	DWORD_PTR openStatus = 0;
	if (!SendMessageTimeout(hIme, WM_IME_CONTROL, IMC_GETOPENSTATUS, 0,
							SMTO_ABORTIFHUNG, 30, &openStatus))
		return false;

	// IME tat han = dang go romaji truc tiep (直接入力 / 半角英数).
	// Luu y: conv van giu nguyen che do cu khi IME tat, nen bat buoc
	// phai kiem tra openStatus truoc, khong duoc chi doc conv.
	if (!openStatus)
		return false;

	DWORD_PTR convMode = 0;
	if (!SendMessageTimeout(hIme, WM_IME_CONTROL, IMC_GETCONVERSIONMODE, 0,
							SMTO_ABORTIFHUNG, 30, &convMode))
		return false;

	// IME_CMODE_NATIVE phu ca ひらがな lan moi kieu カタカナ.
	// 全角英数 khong co co nay nen van cho go tieng Viet.
	return (convMode & IME_CMODE_NATIVE) != 0;
}

static DWORD WINAPI workerProc(LPVOID) {
	HANDLE waits[2] = { _quitEvent, _refreshEvent };
	while (true) {
		DWORD r = WaitForMultipleObjects(2, waits, FALSE, INFINITE);
		if (r == WAIT_OBJECT_0)
			break;

		// Cho IME cap nhat xong roi moi doc.
		if (WaitForSingleObject(_quitEvent, REFRESH_DELAY_MS) == WAIT_OBJECT_0)
			break;

		InterlockedExchange(&_isNativeMode, queryJapaneseNativeMode() ? 1 : 0);
	}
	return 0;
}

void jpImeInit() {
	if (_worker)
		return;
	_quitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);      // manual reset
	_refreshEvent = CreateEvent(NULL, FALSE, FALSE, NULL);  // auto reset
	_worker = CreateThread(NULL, 0, workerProc, NULL, 0, NULL);
	jpImeRequestRefresh();
}

void jpImeShutdown() {
	if (!_worker)
		return;
	SetEvent(_quitEvent);
	WaitForSingleObject(_worker, 1000);
	CloseHandle(_worker);
	CloseHandle(_refreshEvent);
	CloseHandle(_quitEvent);
	_worker = NULL;
	_refreshEvent = NULL;
	_quitEvent = NULL;
}

bool jpImeIsNativeMode() {
	return InterlockedCompareExchange(&_isNativeMode, 0, 0) != 0;
}

void jpImeRequestRefresh() {
	if (_refreshEvent)
		SetEvent(_refreshEvent);
}
