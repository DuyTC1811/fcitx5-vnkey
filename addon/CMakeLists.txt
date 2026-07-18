# addon/CMakeLists.txt
# Chỉ build khi BUILD_ADDON=ON (đã guard từ root CMakeLists).
find_package(Fcitx5Core REQUIRED)
find_package(Fcitx5Utils REQUIRED)

# Thư viện addon. MODULE = plugin được dlopen lúc chạy, đúng cách Fcitx5 nạp addon.
add_library(vnkey MODULE
        vnkey.cpp
        keymap.cpp
)

# Ép tên output thành "libvnkey.so" khớp với Library=libvnkey trong .conf.
set_target_properties(vnkey PROPERTIES PREFIX "lib")

target_link_libraries(vnkey
        PRIVATE
        Fcitx5::Core
        engine            # core static lib (libengine.a)
)

target_compile_features(vnkey PRIVATE cxx_std_23)

target_include_directories(vnkey PRIVATE "${CMAKE_SOURCE_DIR}/core/include")

# ---- Install ----
# 1. Plugin .so -> /usr/lib/<arch>/fcitx5/
install(TARGETS vnkey
        DESTINATION "${FCITX_INSTALL_ADDONDIR}")

# 2. Mô tả addon -> /usr/share/fcitx5/addon/vnkey.conf
install(FILES vnkey-addon.conf
        RENAME vnkey.conf
        DESTINATION "${FCITX_INSTALL_PKGDATADIR}/addon")

# 3. Mô tả input method -> /usr/share/fcitx5/inputmethod/vnkey.conf
install(FILES vnkey.conf DESTINATION "${FCITX_INSTALL_PKGDATADIR}/inputmethod")