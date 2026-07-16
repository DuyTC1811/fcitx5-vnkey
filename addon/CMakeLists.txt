# addon/CMakeLists.txt
# Only built when BUILD_ADDON=ON (guarded from the root CMakeLists).

find_package(Fcitx5Core REQUIRED)
find_package(Fcitx5Utils REQUIRED)

# The addon shared library. MODULE = a plugin loaded at runtime via dlopen,
# which is exactly what Fcitx5 does with addons.
add_library(vnkey MODULE vnkey.cpp)

# Force the output name to "libvnkey.so" to match `Library=libvnkey` in the
# addon .conf. (CMake MODULE targets don't get a "lib" prefix by default.)
set_target_properties(vnkey PROPERTIES PREFIX "lib")

target_link_libraries(vnkey
        PRIVATE
        Fcitx5::Core
        engine          # <-- your core static lib target name; adjust if different
)

target_compile_features(vnkey PRIVATE cxx_std_23)

# Let vnkey.cpp include "core/..." headers with the repo root on the include path.
target_include_directories(vnkey PRIVATE "${CMAKE_SOURCE_DIR}")

# ---- Install rules ----

# 1. The plugin .so -> /usr/lib/<arch>/fcitx5/
install(TARGETS vnkey
        DESTINATION "${FCITX_INSTALL_ADDONDIR}")

# 2. Addon descriptor -> /usr/share/fcitx5/addon/vnkey.conf
install(FILES vnkey-addon.conf
        RENAME vnkey.conf
        DESTINATION "${FCITX_INSTALL_PKGDATADIR}/addon")

# 3. Input method descriptor -> /usr/share/fcitx5/inputmethod/vnkey.conf
install(FILES vnkey-im.conf
        RENAME vnkey.conf
        DESTINATION "${FCITX_INSTALL_PKGDATADIR}/inputmethod")