.PHONY: clean build test rebuild

NPROC := $(shell nproc)

clean:
	rm -rf build

build:
	cmake -B build -DBUILD_ADDON=OFF -DCMAKE_BUILD_TYPE=Debug
	cmake --build build -j$(NPROC)

test: build
	ctest --test-dir build --output-on-failure

rebuild: clean build test
#mkdir -p build && cd build
#cmake .. -DCMAKE_BUILD_TYPE=Release
#make -j$(nproc)
#fcitx5 -rd
#fcitx5-configtool