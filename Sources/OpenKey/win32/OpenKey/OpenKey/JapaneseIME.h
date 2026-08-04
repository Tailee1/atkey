/*----------------------------------------------------------
OpenKey - The Cross platform Open source Vietnamese Keyboard application.

This file is belong to the OpenKey project, Win32 version
which is released under GPL license.
You can fork, modify, improve this program. If you
redistribute your new version, it MUST be open source.
-----------------------------------------------------------*/

//
// Tu dong vo hieu hoa engine tieng Viet khi IME tieng Nhat
// dang o che do native (ひらがな / カタカナ).
//
// Trang thai duoc doc bang WM_IME_CONTROL gui toi IME window cua
// cua so dang focus. Loi goi do la SendMessage dong bo lien tien
// trinh: NEU goi truc tiep trong keyboard hook thi no chen vao giua
// chuoi backspace+ky tu ma engine dang bom ra, gay mat dau / nhan
// doi dau. Vi vay query luon chay tren thread rieng, con hook chi
// doc bien cache.
//
#pragma once

// Khoi tao / dung thread query. Goi trong OnInit / OnDestroy.
void jpImeInit();
void jpImeShutdown();

// Doc cache. An toan de goi tu keyboard hook (khong he thong goi gi).
bool jpImeIsNativeMode();

// Yeu cau cap nhat cache. An toan de goi tu keyboard hook.
// Chi bao hieu cho thread nen, tra ve ngay lap tuc.
void jpImeRequestRefresh();
