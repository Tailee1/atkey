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
#include "MainControlDialog.h"
#include "AppDelegate.h"
#include <Shlobj.h>
#include <Uxtheme.h>

#pragma comment(lib, "UxTheme.lib")

static Uint16 _lastKeyCode;

MainControlDialog::MainControlDialog(const HINSTANCE& hInstance, const int& resourceId)
    : BaseDialog(hInstance, resourceId) {
}

MainControlDialog::~MainControlDialog() {
}

void MainControlDialog::initDialog() {
    HINSTANCE hIns = GetModuleHandleW(NULL);
    //dialog icon
    SET_DIALOG_ICON(IDI_APP_ICON);

    //set title version
    TCHAR title[256];
    TCHAR titleBuffer[256];
    LoadString(hIns, IDS_MAIN_DIALOG_TITLE, title, 256);
    wsprintfW(titleBuffer, title, OpenKeyHelper::getVersionString().c_str());
    SetWindowText(hDlg, titleBuffer);

    //input type
    comboBoxInputType = GetDlgItem(hDlg, IDC_COMBO_INPUT_TYPE);
    vector<LPCTSTR>& inputType = OpenKeyManager::getInputType();
    for (int i = 0; i < inputType.size(); i++) {
        SendMessage(comboBoxInputType, CB_ADDSTRING, i, reinterpret_cast<LPARAM>(inputType[i]));
    }
    createToolTip(comboBoxInputType, IDS_STRING_INPUT);

    //code table
    comboBoxTableCode = GetDlgItem(hDlg, IDC_COMBO_TABLE_CODE);
    vector<LPCTSTR>& tableCode = OpenKeyManager::getTableCode();
    for (int i = 0; i < tableCode.size(); i++) {
        SendMessage(comboBoxTableCode, CB_ADDSTRING, i, reinterpret_cast<LPARAM>(tableCode[i]));
    }
    createToolTip(comboBoxTableCode, IDS_STRING_CODE);

    //init tabview
    hTab = GetDlgItem(hDlg, IDC_TAB_CONTROL);
    TCITEM tci = { 0 };
    tci.mask = TCIF_TEXT;
    tci.pszText = (LPWSTR)_T("Bộ gõ");
    TabCtrl_InsertItem(hTab, 0, &tci);
    tci.pszText = (LPWSTR)_T("Gõ tắt");
    TabCtrl_InsertItem(hTab, 1, &tci);
    tci.pszText = (LPWSTR)_T("Hệ thống");
    TabCtrl_InsertItem(hTab, 2, &tci);
    tci.pszText = (LPWSTR)_T("Thông tin");
    TabCtrl_InsertItem(hTab, 3, &tci);
    RECT r;
    TabCtrl_GetItemRect(hTab, 0, &r);
    TabCtrl_SetItemSize(hTab, r.right - r.left, (r.bottom - r.top) * 1.428f);

    //create tab page
    hTabPage1 = CreateDialogParam(hIns, MAKEINTRESOURCE(IDD_DIALOG_TAB_GENERAL), hDlg, tabPageEventProc, (LPARAM)this);
    hTabPage2 = CreateDialogParam(hIns, MAKEINTRESOURCE(IDD_DIALOG_TAB_MACRO), hDlg, tabPageEventProc, (LPARAM)this);
    hTabPage3 = CreateDialogParam(hIns, MAKEINTRESOURCE(IDD_DIALOG_TAB_SYSTEM), hDlg, tabPageEventProc, (LPARAM)this);
    hTabPage4 = CreateDialogParam(hIns, MAKEINTRESOURCE(IDD_DIALOG_TAB_INFO), hDlg, tabPageEventProc, (LPARAM)this);
    RECT rc;//find tab control's rectangle
    GetWindowRect(hTab, &rc);
    POINT offset = { 0 };
    ScreenToClient(hDlg, &offset);
    OffsetRect(&rc, offset.x, offset.y); //convert to client coordinates
    rc.top += (LONG)((r.bottom - r.top) * 1.428f);
    SetWindowPos(hTabPage1, 0, rc.left + 1, rc.top + 3, rc.right - rc.left - 5, rc.bottom - rc.top - 5, SWP_HIDEWINDOW);
    SetWindowPos(hTabPage2, 0, rc.left + 1, rc.top + 3, rc.right - rc.left - 5, rc.bottom - rc.top - 6, SWP_HIDEWINDOW);
    SetWindowPos(hTabPage3, 0, rc.left + 1, rc.top + 3, rc.right - rc.left - 5, rc.bottom - rc.top - 6, SWP_HIDEWINDOW);
    SetWindowPos(hTabPage4, 0, rc.left + 1, rc.top + 3, rc.right - rc.left - 5, rc.bottom - rc.top - 6, SWP_HIDEWINDOW);
    onTabIndexChanged();

    checkCtrl = GetDlgItem(hDlg, IDC_CHECK_SWITCH_KEY_CTRL);
    createToolTip(checkCtrl, IDS_STRING_CTRL);

    checkAlt = GetDlgItem(hDlg, IDC_CHECK_SWITCH_KEY_ALT);
    createToolTip(checkAlt, IDS_STRING_ALT);

    checkWin = GetDlgItem(hDlg, IDC_CHECK_SWITCH_KEY_WIN);
    createToolTip(checkWin, IDS_STRING_WIN);

    checkShift = GetDlgItem(hDlg, IDC_CHECK_SWITCH_KEY_SHIFT);
    createToolTip(checkShift, IDS_STRING_SHIFT);

    checkBeep = GetDlgItem(hDlg, IDC_CHECK_SWITCH_KEY_BEEP);
    createToolTip(checkBeep, IDS_STRING_BEEP);

    checkVietnamese = GetDlgItem(hDlg, IDC_RADIO_METHOD_VIETNAMESE);
    createToolTip(checkVietnamese, IDS_STRING_VIET);

    checkEnglish = GetDlgItem(hDlg, IDC_RADIO_METHOD_ENGLISH);
    createToolTip(checkEnglish, IDS_STRING_ENG);

    /*--------end common---------*/

    checkModernOrthorgraphy = GetDlgItem(hTabPage1, IDC_CHECK_MODERN_ORTHORGRAPHY);
    createToolTip(checkModernOrthorgraphy, IDS_STRING_MORDEN_ORTHORGRAPHY);

    checkFixRecommendBrowser = GetDlgItem(hTabPage1, IDC_CHECK_FIX_RECOMMEND_BROWSER);
    createToolTip(checkFixRecommendBrowser, IDS_STRING_FIX_BROWSER);

    checkSpelling = GetDlgItem(hTabPage1, IDC_CHECK_SPELLING);
    createToolTip(checkSpelling, IDS_STRING_SPELLING_CHECK);

    checkRestoreIfWrongSpelling = GetDlgItem(hTabPage1, IDC_CHECK_RESTORE_IF_WRONG_SPELLING);
    createToolTip(checkRestoreIfWrongSpelling, IDS_STRING_RESTORE_IF_WRONG_SPELLING);

    checkAllowZWJF = GetDlgItem(hTabPage1, IDC_CHECK_ALLOW_ZJWF);
    createToolTip(checkAllowZWJF, IDS_STRING_ALLOW_ZWFJ);

    checkTempOffSpelling = GetDlgItem(hTabPage1, IDC_CHECK_TEMP_OFF_SPELLING);
    createToolTip(checkTempOffSpelling, IDS_STRING_TEMP_OFF_SPELLING);

    checkSmartSwitchKey = GetDlgItem(hTabPage1, IDC_CHECK_SMART_SWITCH_KEY);
    createToolTip(checkSmartSwitchKey, IDS_STRING_SMART_SWITCH_KEY);

    checkCapsFirstChar = GetDlgItem(hTabPage1, IDC_CHECK_CAPS_FIRST_CHAR);
    createToolTip(checkCapsFirstChar, IDS_STRING_CAPS_FIRST_CHAR);

    checkRememberTableCode = GetDlgItem(hTabPage1, IDC_CHECK_SMART_SWITCH_CODE);
    createToolTip(checkRememberTableCode, IDS_STRING_REMEMBER_TABLE_CODE);

    checkAllowOtherLanguages = GetDlgItem(hTabPage1, IDC_CHECK_OTHER_LANGUAGES);
    createToolTip(checkAllowOtherLanguages, IDS_STRING_OTHER_LANGUAGES);

    checkTempOffOpenKey = GetDlgItem(hTabPage1, IDC_CHECK_TEMP_OFF_OPEN_KEY);
    createToolTip(checkTempOffOpenKey, IDS_STRING_TEMP_OFF_OPENKEY);

    /*------------end tab 1----------------*/

    checkQuickStartConsonant = GetDlgItem(hTabPage2, IDC_CHECK_QUICK_START_CONSONANT);
    createToolTip(checkQuickStartConsonant, IDS_STRING_START_CONSONANT);

    checkQuickEndConsonant = GetDlgItem(hTabPage2, IDC_CHECK_QUICK_END_CONSONANT);
    createToolTip(checkQuickEndConsonant, IDS_STRING_END_CONSONANT);

    checkQuickTelex = GetDlgItem(hTabPage2, IDC_CHECK_QUICK_TELEX);
    createToolTip(checkQuickTelex, IDS_STRING_QUICK_TELEX);

    checkUseMacro = GetDlgItem(hTabPage2, IDC_CHECK_USE_MACRO);
    createToolTip(checkUseMacro, IDS_STRING_MACRO);

    checkUseMacroInEnglish = GetDlgItem(hTabPage2, IDC_CHECK_USE_MACRO_IN_ENGLISH);
    createToolTip(checkUseMacroInEnglish, IDS_STRING_MACRO_IN_ENG);

    checkMacroAutoCaps = GetDlgItem(hTabPage2, IDC_CHECK_AUTO_CAPS);
    createToolTip(checkMacroAutoCaps, IDS_STRING_MACRO_AUTO_CAP);

    hUpdateButton = GetDlgItem(hDlg, IDC_BUTTON_CHECK_UPDATE);

    /*------------end tab 2----------------*/

    checkModernIcon = GetDlgItem(hTabPage3, IDC_CHECK_MODERN_ICON);
    createToolTip(checkModernIcon, IDS_STRING_MODERN_ICON);

    checkShowOnStartup = GetDlgItem(hTabPage3, IDC_CHECK_SHOW_ON_STARTUP);
    createToolTip(checkShowOnStartup, IDS_STRING_SHOW_ON_STARTUP);

    checkRunWithWindows = GetDlgItem(hTabPage3, IDC_CHECK_RUN_WITH_WINDOWS);
    createToolTip(checkRunWithWindows, IDS_STRING_RUN_ON_STARTUP);

    checkCreateDesktopShortcut = GetDlgItem(hTabPage3, IDC_CHECK_CHECK_CREATE_SHORTCUT);
    createToolTip(checkCreateDesktopShortcut, IDS_STRING_CREATE_DESKTOP);

    checkRunAsAdmin = GetDlgItem(hTabPage3, IDC_CHECK_RUN_AS_ADMIN);
    createToolTip(checkRunAsAdmin, IDS_STRING_RUN_AS_ADMIN);

    checkCheckNewVersion = GetDlgItem(hTabPage3, IDC_CHECK_CHECK_UPDATE);
    createToolTip(checkCheckNewVersion, IDS_STRING_CHECK_UPDATE);

    checkSupportMetroApp = GetDlgItem(hTabPage3, IDC_CHECK_SUPPORT_METRO_APP);
    createToolTip(checkSupportMetroApp, IDS_STRING_SUPPORT_METRO);

    checkUseClipboard = GetDlgItem(hTabPage3, IDC_CHECK_USE_CLIPBOARD);
    createToolTip(checkUseClipboard, IDS_STRING_USE_CLIPBOARD);

    checkFixChromium = GetDlgItem(hTabPage3, IDC_CHECK_FIX_CHROMIUM);
    createToolTip(checkFixChromium, IDS_STRING_FIX_CHROMIUM);

    /*------------end tab 3----------------*/

    SendDlgItemMessage(hDlg, IDBUTTON_OK, BM_SETIMAGE, IMAGE_ICON, (LPARAM)LoadIcon(hIns, MAKEINTRESOURCEW(IDI_ICON_OK_BUTTON)));
    SendDlgItemMessage(hDlg, ID_BTN_DEFAULT, BM_SETIMAGE, IMAGE_ICON, (LPARAM)LoadIcon(hIns, MAKEINTRESOURCEW(IDI_ICON_DEFAULT_BUTTON)));
    SendDlgItemMessage(hDlg, IDBUTTON_EXIT, BM_SETIMAGE, IMAGE_ICON, (LPARAM)LoadIcon(hIns, MAKEINTRESOURCEW(IDI_ICON_EXIT_BUTTON)));
    fillData();
}

INT_PTR MainControlDialog::eventProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_INITDIALOG:
        this->hDlg = hDlg;
        initDialog();
        return TRUE;
    case WM_COMMAND: {
        int wmId = LOWORD(wParam);
        switch (wmId) {
        case IDBUTTON_OK:
            AppDelegate::getInstance()->closeDialog(this);
            break;
        case IDBUTTON_EXIT:
            AppDelegate::getInstance()->onOpenKeyExit();
            break;
        case ID_BTN_DEFAULT: {
            int msgboxID = MessageBox(
                hDlg,
                _T("Bạn có chắc chắn muốn thiết lập lại cài đặt gốc?"),
                _T("ATKey"),
                MB_ICONEXCLAMATION | MB_YESNO
            );
            if (msgboxID == IDYES) {
                AppDelegate::getInstance()->onDefaultConfig();
            }
            break;
        }
        case IDC_BUTTON_MACRO_TABLE:
            AppDelegate::getInstance()->onMacroTable();
            break;
        case IDC_BUTTON_CHECK_UPDATE:
            onUpdateButton();
            break;
        case IDC_BUTTON_GO_SOURCE_CODE:
            //GPL yeu cau cung cap ma nguon cua CHINH ban da sua, khong phai ma
            //nguon cua ban goc. Day la repo cua ATKey; atkey.org/source.zip la
            //ban chup du phong cho ai khong dung git.
            ShellExecute(NULL, _T("open"), _T("https://github.com/Tailee1/atkey"), NULL, NULL, SW_SHOWNORMAL);
            break;
        default:
            if (HIWORD(wParam) == CBN_SELCHANGE) {
                this->onComboBoxSelected((HWND)lParam, LOWORD(wParam));
            }
            else if (HIWORD(wParam) == BN_CLICKED) {
                this->onCheckboxClicked((HWND)lParam);
            }
            break;
        }
        break;
    }
    case WM_NOTIFY: {
        switch (((LPNMHDR)lParam)->code) {
        case TCN_SELCHANGE:
            onTabIndexChanged();
            break;
        case NM_CLICK:
        case NM_RETURN: {
            PNMLINK link = (PNMLINK)lParam;
            if (link->hdr.idFrom == IDC_SYSLINK_HOME_PAGE)
                ShellExecute(NULL, _T("open"), _T("https://atkey.org"), NULL, NULL, SW_SHOWNORMAL);
            else if (link->hdr.idFrom == IDC_SYSLINK_FANPAGE)
                ShellExecute(NULL, _T("open"), _T("https://github.com/tuyenvm/OpenKey"), NULL, NULL, SW_SHOWNORMAL);
            break;
        }
        }
        break;
    }
    }

    return FALSE;
}

INT_PTR MainControlDialog::tabPageEventProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_INITDIALOG) {
#ifdef _WIN64
        SetWindowLongPtr(hDlg, GWLP_USERDATA, lParam);
#else
        SetWindowLong(hDlg, GWL_USERDATA, lParam);
#endif
        return TRUE;
    }
    else if (uMsg == WM_ERASEBKGND) {
        return TRUE;
    }
    else if ((uMsg == WM_CTLCOLORSTATIC || uMsg == WM_CTLCOLORBTN) && IsThemeActive()) {
        SetBkMode((HDC)wParam, TRANSPARENT);
        return (LRESULT)GetStockObject(COLOR_WINDOW + 1);
    }
    else if (uMsg == WM_PAINT && IsThemeActive()) {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hDlg, &ps);

        // All painting occurs here, between BeginPaint and EndPaint.
        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

        EndPaint(hDlg, &ps);

        return 0;
    }

#ifdef _WIN64
    LONG_PTR attr = GetWindowLongPtr(hDlg, GWLP_USERDATA);
#else
    long attr = GetWindowLong(hDlg, GWL_USERDATA);
#endif
    if (attr != 0) {
        return ((MainControlDialog*)attr)->eventProc(hDlg, uMsg, wParam, lParam);
    }
    return FALSE;
}

void MainControlDialog::fillData() {
    SendMessage(comboBoxInputType, CB_SETCURSEL, vInputType, 0);
    SendMessage(comboBoxTableCode, CB_SETCURSEL, vCodeTable, 0);

    SendMessage(checkCtrl, BM_SETCHECK, HAS_CONTROL(vSwitchKeyStatus) ? 1 : 0, 0);
    SendMessage(checkAlt, BM_SETCHECK, HAS_OPTION(vSwitchKeyStatus) ? 1 : 0, 0);
    SendMessage(checkWin, BM_SETCHECK, HAS_COMMAND(vSwitchKeyStatus) ? 1 : 0, 0);
    SendMessage(checkShift, BM_SETCHECK, HAS_SHIFT(vSwitchKeyStatus) ? 1 : 0, 0);
    SendMessage(checkBeep, BM_SETCHECK, HAS_BEEP(vSwitchKeyStatus) ? 1 : 0, 0);

    //Dung lai thu tu tick tu cau hinh da luu. OpenKeyInit da dam bao dung 2 co.
    _switchModOrder.clear();
    const int switchBits[4] = { 0x100, 0x200, 0x400, 0x800 };
    for (int i = 0; i < 4; i++) {
        if (vSwitchKeyStatus & switchBits[i])
            _switchModOrder.push_back(i);
    }

    SendMessage(checkVietnamese, BM_SETCHECK, vLanguage, 0);
    SendMessage(checkEnglish, BM_SETCHECK, !vLanguage, 0);

    SendMessage(checkModernOrthorgraphy, BM_SETCHECK, vUseModernOrthography ? 1 : 0, 0);
    SendMessage(checkFixRecommendBrowser, BM_SETCHECK, vFixRecommendBrowser ? 1 : 0, 0);
    SendMessage(checkShowOnStartup, BM_SETCHECK, vShowOnStartUp ? 1 : 0, 0);
    SendMessage(checkRunWithWindows, BM_SETCHECK, vRunWithWindows ? 1 : 0, 0);
    SendMessage(checkSpelling, BM_SETCHECK, vCheckSpelling ? 1 : 0, 0);
    SendMessage(checkRestoreIfWrongSpelling, BM_SETCHECK, vRestoreIfWrongSpelling ? 1 : 0, 0);
    SendMessage(checkModernIcon, BM_SETCHECK, vUseGrayIcon ? 1 : 0, 0);
    SendMessage(checkAllowZWJF, BM_SETCHECK, vAllowConsonantZFWJ ? 1 : 0, 0);
    SendMessage(checkTempOffSpelling, BM_SETCHECK, vTempOffSpelling ? 1 : 0, 0);
    SendMessage(checkQuickStartConsonant, BM_SETCHECK, vQuickStartConsonant ? 1 : 0, 0);
    SendMessage(checkQuickEndConsonant, BM_SETCHECK, vQuickEndConsonant ? 1 : 0, 0);
    SendMessage(checkRememberTableCode, BM_SETCHECK, vRememberCode ? 1 : 0, 0);
    SendMessage(checkAllowOtherLanguages, BM_SETCHECK, vOtherLanguage ? 1 : 0, 0);
    SendMessage(checkTempOffOpenKey, BM_SETCHECK, vTempOffOpenKey ? 1 : 0, 0);

    SendMessage(checkSmartSwitchKey, BM_SETCHECK, vUseSmartSwitchKey ? 1 : 0, 0);
    SendMessage(checkCapsFirstChar, BM_SETCHECK, vUpperCaseFirstChar ? 1 : 0, 0);
    SendMessage(checkQuickTelex, BM_SETCHECK, vQuickTelex ? 1 : 0, 0);
    SendMessage(checkUseMacro, BM_SETCHECK, vUseMacro ? 1 : 0, 0);
    SendMessage(checkUseMacroInEnglish, BM_SETCHECK, vUseMacroInEnglishMode ? 1 : 0, 0);

    SendMessage(checkMacroAutoCaps, BM_SETCHECK, vAutoCapsMacro ? 1 : 0, 0);
    SendMessage(checkSupportMetroApp, BM_SETCHECK, vSupportMetroApp ? 1 : 0, 0);
    SendMessage(checkCreateDesktopShortcut, BM_SETCHECK, vCreateDesktopShortcut ? 1 : 0, 0);
    SendMessage(checkRunAsAdmin, BM_SETCHECK, vRunAsAdmin ? 1 : 0, 0);
    SendMessage(checkCheckNewVersion, BM_SETCHECK, vCheckNewVersion ? 1 : 0, 0);
    SendMessage(checkUseClipboard, BM_SETCHECK, vSendKeyStepByStep ? 0 : 1, 0);
    SendMessage(checkFixChromium, BM_SETCHECK, vFixChromiumBrowser ? 1 : 0, 0);

    EnableWindow(checkRestoreIfWrongSpelling, vCheckSpelling);
    EnableWindow(checkAllowZWJF, vCheckSpelling);
    EnableWindow(checkTempOffSpelling, vCheckSpelling);
    EnableWindow(checkFixChromium, vFixRecommendBrowser);

    //tab info
    wchar_t buffer[256];
    wsprintfW(buffer, _T("Phiên bản %s cho Windows - Ngày cập nhật: %s"), OpenKeyHelper::getVersionString().c_str(), _T(__DATE__));
    SendDlgItemMessage(hTabPage4, IDC_STATIC_APP_VERSION_INFO, WM_SETTEXT, 0, LPARAM(buffer));
}

void MainControlDialog::onComboBoxSelected(const HWND& hCombobox, const int& comboboxId) {
    if (hCombobox == comboBoxInputType) {
        APP_SET_DATA(vInputType, (int)SendMessage(hCombobox, CB_GETCURSEL, 0, 0));
    }
    else if (hCombobox == comboBoxTableCode) {
        APP_SET_DATA(vCodeTable, (int)SendMessage(hCombobox, CB_GETCURSEL, 0, 0));
        if (vRememberCode) {
            setAppInputMethodStatus(OpenKeyHelper::getFrontMostAppExecuteName(), vLanguage | (vCodeTable << 1));
            saveSmartSwitchKeyData();
        }
    }
    SystemTrayHelper::updateData();
}

void MainControlDialog::onCheckboxClicked(const HWND& hWnd) {
    int val = 0;
    if (hWnd == checkCtrl || hWnd == checkAlt || hWnd == checkWin || hWnd == checkShift) {
        onSwitchModifierClicked(hWnd);
    }
    else if (hWnd == checkBeep) {
        val = (int)SendMessage(checkBeep, BM_GETCHECK, 0, 0);
        vSwitchKeyStatus &= (~0x8000);
        vSwitchKeyStatus |= val << 15;
        APP_SET_DATA(vSwitchKeyStatus, vSwitchKeyStatus);
    }
    else if (hWnd == checkVietnamese) {
        val = (int)SendMessage(checkVietnamese, BM_GETCHECK, 0, 0);
        APP_SET_DATA(vLanguage, val ? 1 : 0);
        if (vUseSmartSwitchKey) {
            setAppInputMethodStatus(OpenKeyHelper::getFrontMostAppExecuteName(), vLanguage | (vCodeTable << 1));
            saveSmartSwitchKeyData();
        }
    }
    else if (hWnd == checkEnglish) {
        val = (int)SendMessage(checkVietnamese, BM_GETCHECK, 0, 0);
        APP_SET_DATA(vLanguage, val ? 1 : 0);
        if (vUseSmartSwitchKey) {
            setAppInputMethodStatus(OpenKeyHelper::getFrontMostAppExecuteName(), vLanguage | (vCodeTable << 1));
            saveSmartSwitchKeyData();
        }
    }
    else if (hWnd == checkModernOrthorgraphy) {
        val = (int)SendMessage(hWnd, BM_GETCHECK, 0, 0);
        APP_SET_DATA(vUseModernOrthography, val ? 1 : 0);
    }
    else if (hWnd == checkFixRecommendBrowser) {
        val = (int)SendMessage(hWnd, BM_GETCHECK, 0, 0);
        APP_SET_DATA(vFixRecommendBrowser, val ? 1 : 0);
        EnableWindow(checkFixChromium, vFixRecommendBrowser);
    }
    else if (hWnd == checkShowOnStartup) {
        val = (int)SendMessage(hWnd, BM_GETCHECK, 0, 0);
        APP_SET_DATA(vShowOnStartUp, val ? 1 : 0);
    }
    else if (hWnd == checkRunWithWindows) {
        val = (int)SendMessage(hWnd, BM_GETCHECK, 0, 0);
        APP_SET_DATA(vRunWithWindows, val ? 1 : 0);
        OpenKeyHelper::registerRunOnStartup(vRunWithWindows);
    }
    else if (hWnd == checkSpelling) {
        val = (int)SendMessage(hWnd, BM_GETCHECK, 0, 0);
        APP_SET_DATA(vCheckSpelling, val ? 1 : 0);
        vSetCheckSpelling();
        EnableWindow(checkRestoreIfWrongSpelling, vCheckSpelling);
        EnableWindow(checkAllowZWJF, vCheckSpelling);
        EnableWindow(checkTempOffSpelling, vCheckSpelling);
    }
    else if (hWnd == checkRestoreIfWrongSpelling) {
        val = (int)SendMessage(hWnd, BM_GETCHECK, 0, 0);
        APP_SET_DATA(vRestoreIfWrongSpelling, val ? 1 : 0);
    }
    else if (hWnd == checkUseClipboard) {
        val = (int)SendMessage(hWnd, BM_GETCHECK, 0, 0);
        APP_SET_DATA(vSendKeyStepByStep, val ? 0 : 1);
    }
    else if (hWnd == checkSmartSwitchKey) {
        val = (int)SendMessage(hWnd, BM_GETCHECK, 0, 0);
        APP_SET_DATA(vUseSmartSwitchKey, val ? 1 : 0);
    }
    else if (hWnd == checkCapsFirstChar) {
        val = (int)SendMessage(hWnd, BM_GETCHECK, 0, 0);
        APP_SET_DATA(vUpperCaseFirstChar, val ? 1 : 0);
    }
    else if (hWnd == checkQuickTelex) {
        val = (int)SendMessage(hWnd, BM_GETCHECK, 0, 0);
        APP_SET_DATA(vQuickTelex, val ? 1 : 0);
    }
    else if (hWnd == checkUseMacro) {
        val = (int)SendMessage(hWnd, BM_GETCHECK, 0, 0);
        APP_SET_DATA(vUseMacro, val ? 1 : 0);
    }
    else if (hWnd == checkUseMacroInEnglish) {
        val = (int)SendMessage(hWnd, BM_GETCHECK, 0, 0);
        APP_SET_DATA(vUseMacroInEnglishMode, val ? 1 : 0);
    }
    else if (hWnd == checkModernIcon) {
        val = (int)SendMessage(hWnd, BM_GETCHECK, 0, 0);
        APP_SET_DATA(vUseGrayIcon, val ? 1 : 0);
    }
    else if (hWnd == checkAllowZWJF) {
        val = (int)SendMessage(hWnd, BM_GETCHECK, 0, 0);
        APP_SET_DATA(vAllowConsonantZFWJ, val ? 1 : 0);
    }
    else if (hWnd == checkTempOffSpelling) {
        val = (int)SendMessage(hWnd, BM_GETCHECK, 0, 0);
        APP_SET_DATA(vTempOffSpelling, val ? 1 : 0);
    }
    else if (hWnd == checkQuickStartConsonant) {
        val = (int)SendMessage(hWnd, BM_GETCHECK, 0, 0);
        APP_SET_DATA(vQuickStartConsonant, val ? 1 : 0);
    }
    else if (hWnd == checkQuickEndConsonant) {
        val = (int)SendMessage(hWnd, BM_GETCHECK, 0, 0);
        APP_SET_DATA(vQuickEndConsonant, val ? 1 : 0);
    }
    else if (hWnd == checkSupportMetroApp) {
        val = (int)SendMessage(hWnd, BM_GETCHECK, 0, 0);
        APP_SET_DATA(vSupportMetroApp, val ? 1 : 0);
    }
    else if (hWnd == checkMacroAutoCaps) {
        val = (int)SendMessage(hWnd, BM_GETCHECK, 0, 0);
        APP_SET_DATA(vAutoCapsMacro, val ? 1 : 0);
    }
    else if (hWnd == checkCreateDesktopShortcut) {
        val = (int)SendMessage(hWnd, BM_GETCHECK, 0, 0);
        APP_SET_DATA(vCreateDesktopShortcut, val ? 1 : 0);
        //create desktop shortcut
        if (val)
            OpenKeyManager::createDesktopShortcut();
    }
    else if (hWnd == checkRunAsAdmin) {
        val = (int)SendMessage(hWnd, BM_GETCHECK, 0, 0);
        APP_SET_DATA(vRunAsAdmin, val ? 1 : 0);
        requestRestartAsAdmin();
    }
    else if (hWnd == checkCheckNewVersion) {
        val = (int)SendMessage(hWnd, BM_GETCHECK, 0, 0);
        APP_SET_DATA(vCheckNewVersion, val ? 1 : 0);
    }
    else if (hWnd == checkRememberTableCode) {
        val = (int)SendMessage(hWnd, BM_GETCHECK, 0, 0);
        APP_SET_DATA(vRememberCode, val ? 1 : 0);
    }
    else if (hWnd == checkAllowOtherLanguages) {
        val = (int)SendMessage(hWnd, BM_GETCHECK, 0, 0);
        APP_SET_DATA(vOtherLanguage, val ? 1 : 0);
    }
    else if (hWnd == checkTempOffOpenKey) {
        val = (int)SendMessage(hWnd, BM_GETCHECK, 0, 0);
        APP_SET_DATA(vTempOffOpenKey, val ? 1 : 0);
    }
    else if (hWnd == checkFixChromium) {
        val = (int)SendMessage(hWnd, BM_GETCHECK, 0, 0);
        APP_SET_DATA(vFixChromiumBrowser, val ? 1 : 0);
    }
    SystemTrayHelper::updateData();
}

//ATKey bat buoc phim chuyen Viet-Anh gom DUNG 2 trong 4 phim bo tro.
//  - Tick phim thu 3  -> phim duoc tick som nhat tu dong bo ra.
//  - Bo tick khi dang co 2 -> khong cho, tra lai nguyen trang.
//Nho vay khong bao gio roi vao trang thai 1 phim (de bam nham) hay 0 phim
//(EMPTY_HOTKEY, hotkey bi vo hieu ma nguoi dung khong biet vi sao).
void MainControlDialog::onSwitchModifierClicked(const HWND& hWnd) {
    const HWND wnds[4] = { checkCtrl, checkAlt, checkWin, checkShift };
    const int  bits[4] = { 0x100, 0x200, 0x400, 0x800 };

    int idx = -1;
    for (int i = 0; i < 4; i++) {
        if (wnds[i] == hWnd)
            idx = i;
    }
    if (idx < 0)
        return;

    if (SendMessage(hWnd, BM_GETCHECK, 0, 0) == BST_UNCHECKED) {
        SendMessage(hWnd, BM_SETCHECK, BST_CHECKED, 0);
        return;
    }

    for (size_t i = 0; i < _switchModOrder.size(); i++) {
        if (_switchModOrder[i] == idx) {
            _switchModOrder.erase(_switchModOrder.begin() + i);
            break;
        }
    }
    _switchModOrder.push_back(idx);

    while (_switchModOrder.size() > 2) {
        int oldest = _switchModOrder.front();
        _switchModOrder.erase(_switchModOrder.begin());
        SendMessage(wnds[oldest], BM_SETCHECK, BST_UNCHECKED, 0);
    }

    vSwitchKeyStatus &= ~0xF00;
    for (size_t i = 0; i < _switchModOrder.size(); i++) {
        vSwitchKeyStatus |= bits[_switchModOrder[i]];
    }
    APP_SET_DATA(vSwitchKeyStatus, vSwitchKeyStatus);
    SystemTrayHelper::updateData();
}

void MainControlDialog::onTabIndexChanged() {
    int index = TabCtrl_GetCurSel(hTab);
    ShowWindow(hTabPage1, (index == 0) ? SW_SHOW : SW_HIDE);
    ShowWindow(hTabPage2, (index == 1) ? SW_SHOW : SW_HIDE);
    ShowWindow(hTabPage3, (index == 2) ? SW_SHOW : SW_HIDE);
    ShowWindow(hTabPage4, (index == 3) ? SW_SHOW : SW_HIDE);
}

void MainControlDialog::onUpdateButton() {
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

    }
    else {
        MessageBox(hDlg, _T("Bạn đang dùng phiên bản mới nhất!"), _T("ATKey Update"), MB_OK);
    }
    EnableWindow(hUpdateButton, true);
}

void MainControlDialog::requestRestartAsAdmin() {
    OpenKeyHelper::registerRunOnStartup(false);
    if (vRunAsAdmin && !IsUserAnAdmin()) {
        int msgboxID = MessageBox(
            hDlg,
            _T("Bạn cần phải khởi động lại ATKey để kích hoạt chế độ Admin!\nBạn có muốn khởi động lại ATKey không?"),
            _T("ATKey"),
            MB_ICONEXCLAMATION | MB_YESNO
        );
        if (msgboxID == IDYES) {
            PostQuitMessage(0);
            ShellExecute(0, L"runas", OpenKeyHelper::getFullPath().c_str(), 0, 0, SW_SHOWNORMAL);
        }
    }
    else {
        OpenKeyHelper::registerRunOnStartup(vRunWithWindows);
    }
}
