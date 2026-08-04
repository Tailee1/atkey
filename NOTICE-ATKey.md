# Thông báo sửa đổi — ATKey

ATKey là **bản sửa đổi (modified version)** của OpenKey.

| | |
|---|---|
| Tác phẩm gốc | OpenKey — https://github.com/tuyenvm/OpenKey |
| Tác giả gốc | Mai Vũ Tuyên, Copyright (C) 2019 |
| Giấy phép | GNU General Public License v3 — xem `LICENSE` |
| Bên sửa đổi | ATPOS — Công ty Cổ phần MIKI, https://atkey.org |
| Mã nguồn ATKey | https://github.com/Tailee1/atkey |
| Ngày sửa đổi | Tháng 8 năm 2026 |

Tệp này tồn tại để đáp ứng điều 5(a) của GPL v3: bản sửa đổi phải mang thông báo
rõ ràng cho biết đã bị sửa và thời điểm sửa.

ATKey được phát hành theo cùng giấy phép GPL v3 như tác phẩm gốc. Bạn có toàn
quyền sao chép, sửa đổi và phân phối lại, kèm điều kiện phải công bố mã nguồn.

## Những thay đổi so với OpenKey

### Tính năng chính được thêm

- **Tự vô hiệu hoá khi IME tiếng Nhật đang ở chế độ ひらがな/カタカナ**
  (`Sources/OpenKey/win32/OpenKey/OpenKey/JapaneseIME.h`, `JapaneseIME.cpp` — tệp mới).
  Trạng thái IME được đọc trên luồng nền và lưu vào bộ đệm; hook bàn phím chỉ đọc
  bộ đệm đó.

### Lỗi của bản gốc được sửa

- Hook bàn phím cấp thấp trong `OpenKey.cpp` gọi `SendMessage` **đồng bộ, không
  timeout, trên mỗi phím** để hỏi trạng thái IME. Ứng dụng đích bận sẽ khiến
  Windows vượt `LowLevelHooksTimeout` và gỡ hook, làm bộ gõ chết im lặng. Đã thay
  bằng cơ chế đọc theo sự kiện, không gọi gì trong hook.
- `BaseDialog::bringOnTop()` không gọi `ShowWindow`, nên hộp thoại đã bị ẩn bằng
  nút X thì không bao giờ mở lại được cho tới khi khởi động lại ứng dụng.
- `registerRunOnStartup()` ghi và xoá hai tên khoá registry khác nhau, khiến việc
  bỏ chọn "tự chạy cùng Windows" không có tác dụng.
- Ứng dụng và trình cập nhật dùng `GetCurrentDirectory()` thay vì thư mục chứa
  tệp thực thi, dẫn tới ghi tệp sai chỗ khi chạy lúc khởi động Windows.
- Trình cập nhật xoá tệp exe **trước** khi giải nén gói mới và chỉ `Sleep(5000)`
  thay vì chờ giải nén xong; giải nén lỗi là mất luôn ứng dụng. Đã thay bằng
  `MoveFileEx(..., MOVEFILE_REPLACE_EXISTING)` có thử lại.
- Ứng dụng thoát ngay sau khi gọi trình cập nhật mà không kiểm tra đã chạy được
  hay chưa.

### Thay đổi về giao diện và hành vi

- Bỏ ô nhập phím phụ của tổ hợp chuyển Việt/Anh; bắt buộc dùng đúng 2 trong 4
  phím bổ trợ Ctrl/Alt/Win/Shift. Mặc định là Ctrl + Shift.
- Đổi tên thương hiệu thành ATKey, bộ biểu tượng mới.
- Khoá registry chuyển sang `HKCU\SOFTWARE\MIKI\ATKey`.
- Lớp cửa sổ đổi thành `ATKeyVietnameseInputMethod` để chạy song song được với
  OpenKey mà không nhận nhầm nhau.
- Cơ chế cập nhật trỏ về https://atkey.org, tải thẳng tệp exe thay vì gói zip.
- Đặt lại số phiên bản về 1.0.0.

## Phần giữ nguyên

Toàn bộ thông báo bản quyền trong mã nguồn của tác giả gốc được giữ nguyên vẹn.
Engine xử lý tiếng Việt (`Sources/OpenKey/engine/`) không bị sửa đổi.
