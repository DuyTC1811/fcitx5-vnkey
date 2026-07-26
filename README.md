# fcitx5-vnkey

Bộ gõ tiếng Việt (Vietnamese Input Method) cho [Fcitx5](https://github.com/fcitx/fcitx5), viết bằng C++23, hỗ trợ kiểu gõ **Telex**.

## Tính năng

- Gõ tiếng Việt kiểu Telex trên Linux, chạy trên nền Fcitx5
- Kiến trúc 2 lớp tách biệt rõ ràng:
    - **`libengine`** (static library): lõi xử lý thuần C++23, độc lập hoàn toàn với Fcitx5 — xử lý `Syllable`, `Transform`, `KeyInput`, logic Telex...
    - **`libvnkey.so`** (addon): lớp bọc mỏng tích hợp `libengine` vào Fcitx5
- Tương thích nhiều bản phân phối Linux: Debian/Ubuntu, Fedora, Arch
- Đóng gói sẵn `.deb` cho Debian/Ubuntu

## Yêu cầu hệ thống

- Linux có cài Fcitx5
- CMake ≥ 3.28
- Ninja
- Trình biên dịch hỗ trợ C++23 (GCC ≥ 13 hoặc Clang ≥ 16)
- Thư viện phát triển Fcitx5: `libfcitx5core-dev` (Debian/Ubuntu)

## Cài đặt

### Debian/Ubuntu (khuyến nghị: dùng gói `.deb`)

```bash
sudo apt install --no-install-recommends libfcitx5core-dev
./build-deb.sh
sudo dpkg -i ../fcitx5-vnkey_0.0.1_*.deb
```

### Build thủ công từ mã nguồn

```bash
# Cài dependency (Debian/Ubuntu)
sudo apt install --no-install-recommends cmake ninja-build libfcitx5core-dev

# Clone và build
git clone https://github.com/<your-username>/fcitx5-vnkey.git
cd fcitx5-vnkey
cmake -B build -G Ninja -DBUILD_ADDON=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
sudo cmake --install build
```

Hoặc dùng Makefile có sẵn ở thư mục gốc:

```bash
make build   # build project
make test    # chạy test cho core engine
make clean   # dọn build directory
make rebuild # clean + build lại từ đầu
```

### Sau khi cài đặt

Khởi động lại Fcitx5 để nhận addon mới:

```bash
fcitx5 -rd
```

Sau đó mở `fcitx5-configtool`, thêm **VNKey** vào danh sách input method và chọn nó khi cần gõ tiếng Việt.

## Kiến trúc

```
fcitx5-vnkey/
├── core/           # libengine — xử lý logic Telex, độc lập Fcitx5
│   ├── include/
│   └── src/
├── addon/          # libvnkey.so — lớp tích hợp Fcitx5
│   ├── include/
│   └── src/
├── debian/         # Packaging cho Debian/Ubuntu
├── CMakeLists.txt
└── Makefile
```

Nguyên tắc quan trọng: phụ thuộc **một chiều** `addon → core`, không bao giờ ngược lại. `core` phải build độc lập được với `-DBUILD_ADDON=OFF`.

## Trạng thái phát triển

- [x] Core engine xử lý Telex (TDD, có test)
- [x] Addon Fcitx5 chạy được end-to-end trên Debian
- [x] Đóng gói `.deb`
- [ ] Xử lý preedit underline / tương thích đa ứng dụng (đang nghiên cứu)
- [ ] Hỗ trợ thêm kiểu gõ VNI, VIQR
- [ ] Test trên Fedora, Arch

## Giấy phép

GPL-3.0-or-later. Xem [LICENSE](LICENSE) để biết chi tiết.

## Đóng góp

Issue và pull request luôn được chào đón. Vui lòng tuân theo quy ước code style trong `CODE_STYLE.md` (PascalCase cho type, camelCase_ cho private member, snake_case cho tên file) và format code bằng `clang-format` (≥ 15) trước khi gửi PR.