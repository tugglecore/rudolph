is_configured := path_exists('build')

configure:
    {{is_configured}} || \
        cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -B build -G "Ninja Multi-Config"

build: configure
    cmake --build build --config Debug

test: build
    ctest --output-on-failure --test-dir build -C Debug

dev:
    watchexec 'clear && just test'

run +command:
    build/bin/Debug/rudolph {{command}}

debug +command:
    gdb build/bin/Debug/rudolph

format:
    clang-format -i --sort-includes src/*.[c,h] tests/*.[c,h]

cppcheck:
    cppcheck --cppcheck-build-dir=build --check-level=exhaustive --language=c --enable=warning,style,performance,information src/*.c

clang-tidy:
    run-clang-tidy -p=build src/*.[c,h]

vale:
    vale $(rg --files | rg '.md[x]?$')

analyze: cppcheck clang-tidy vale
