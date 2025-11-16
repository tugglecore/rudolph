build:
    cmake --build build

run +command:
    build/rudolph {{command}}

debug +command:
    gdb build/rudolph

format:
    clang-format -i src/*
