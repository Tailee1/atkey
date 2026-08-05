# ATKey

Bộ gõ tiếng Việt mã nguồn mở cho Windows, dành cho người Việt dùng bàn phím Nhật.

*Open-source Vietnamese input method editor (IME) for Windows, for Vietnamese
users working on Japanese keyboards.*

---

## Vấn đề mà ATKey giải quyết

Khi bạn dùng bàn phím Nhật và bật IME tiếng Nhật ở chế độ ひらがな hoặc カタカナ,
bộ gõ tiếng Việt vẫn tiếp tục chen vào giữa chuỗi romaji bạn đang gõ. Kết quả là
`ka` không ra `か`, chữ bị mất, dấu bị nhảy.

**ATKey tự động ngừng hoạt động khi IME tiếng Nhật đang ở chế độ native**, rồi tự
bật lại khi bạn quay về 直接入力. Bạn không phải bấm thêm gì.

Cơ chế: ATKey đọc trạng thái IME qua `WM_IME_CONTROL` trên một luồng nền và lưu
vào bộ đệm. Hook bàn phím chỉ đọc bộ đệm đó, **không bao giờ gọi `SendMessage`
bên trong hook** — gọi đồng bộ trong hook cấp thấp sẽ làm Windows vượt
`LowLevelHooksTimeout` rồi gỡ hook, khiến bộ gõ chết im lặng.

## Tính năng

- Kiểu gõ Telex, VNI, Telex đơn giản
- Bảng mã Unicode, TCVN3, VNI Windows, Unicode tổ hợp
- Kiểm tra chính tả, tự khôi phục phím khi gõ sai
- Gõ tắt (macro)
- Tự nhớ chế độ gõ theo từng ứng dụng
- Công cụ chuyển mã
- **Tự vô hiệu hoá khi IME tiếng Nhật ở chế độ ひらがな/カタカナ**
- Phím chuyển Việt/Anh dùng đúng 2 trong 4 phím bổ trợ Ctrl/Alt/Win/Shift,
  mặc định Ctrl + Shift

## Tải về

Xem mục [Releases](https://github.com/Tailee1/atkey/releases) hoặc
[atkey.org](https://atkey.org).

Yêu cầu: Windows 10 trở lên, 64-bit.

> **Lưu ý về cảnh báo bảo mật.** Mọi bộ gõ tiếng Việt trên Windows đều phải cài
> hook bàn phím cấp thấp (`WH_KEYBOARD_LL`) và dùng `SendInput` để thay ký tự đã
> gõ bằng ký tự có dấu. Với máy quét dựa trên hành vi, việc này không phân biệt
> được với keylogger, nên ATKey có thể bị gắn nhãn nhầm. ATKey **không ghi, không
> lưu, không gửi đi** bất kỳ phím nào bạn gõ — toàn bộ mã nguồn ở ngay trong repo
> này, bạn kiểm chứng được.

## Build từ mã nguồn

Cần Visual Studio 2022 hoặc Build Tools kèm workload *Desktop development with C++*.

```
msbuild Sources\OpenKey\win32\OpenKey\OpenKey.sln ^
        /p:Configuration=Release /p:Platform=x64 /t:Rebuild
```

Kết quả: `Sources\OpenKey\win32\OpenKey\x64\Release\ATKey64.exe`

Mỗi lần đẩy lên nhánh `main`, GitHub Actions build lại cả x86 và x64, kèm chứng
thực nguồn gốc build (`actions/attest-build-provenance`) để đối chiếu binary với
đúng commit sinh ra nó.

## Quyền riêng tư

ATKey không thu thập gì. Kết nối mạng duy nhất là tải
`https://atkey.org/version.json` để kiểm tra phiên bản mới, và chỉ khi bạn bật
tuỳ chọn đó hoặc bấm nút kiểm tra. Cấu hình lưu cục bộ trong
`HKCU\SOFTWARE\MIKI\ATKey`.

## Giấy phép và ghi công

ATKey là **bản sửa đổi của [OpenKey](https://github.com/tuyenvm/OpenKey)** do Mai
Vũ Tuyên viết, phát hành theo **GNU General Public License v3**.

Xem [`NOTICE-ATKey.md`](NOTICE-ATKey.md) để biết đầy đủ những gì đã thay đổi so
với bản gốc, và [`LICENSE`](LICENSE) cho toàn văn giấy phép.

Phần engine xử lý tiếng Việt (`Sources/OpenKey/engine/`) giữ nguyên không sửa.

Phát triển bởi ATPOS — Công ty Cổ phần MIKI, [atkey.org](https://atkey.org).
