configure:
    cmake -B build -G "Ninja Multi-Config"

build:
    cmake --build build --config Debug

run +command:
    build/Debug/rudolph {{command}}

debug +command:
    gdb build/Debug/rudolph

format:
    clang-format -i src/*
