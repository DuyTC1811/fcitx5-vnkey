add_library(engine STATIC
        src/engine.cpp
        src/syllable.cpp
        src/methods/telex.cpp
        src/methods/vni.cpp
        src/methods/viqr.cpp
)
target_include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(engine PUBLIC cxx_std_23)
set_target_properties(engine PROPERTIES POSITION_INDEPENDENT_CODE ON)

# QUY TAC VANG: file nao trong core/ ma #include <fcitx/...> la sai kien truc.
