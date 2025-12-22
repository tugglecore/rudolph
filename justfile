setup:
    meson setup build

build:
    meson compile -C build

test:
    meson test -C build -i rudolph:

dev:
    watchexec 'clear && just test'

run +command:
    build/src/rudolph {{command}}

format:
    clang-format --style=file -i --sort-includes src/*.[c,h] tests/*.[c,h]

cppcheck:
    cppcheck --error-exitcode=1 --cppcheck-build-dir=build --check-level=exhaustive --language=c --enable=warning,style,performance,information src/*.c

clang-tidy:
    run-clang-tidy -p=build src/*.[c,h]

vale:
    vale $(rg --files | rg '.md[x]?$')

check: 
    watchexec 'clear && just vale cppcheck clang-tidy'

infer:
    infer \
        --compilation-database build/compile_commands.json \
        --skip-analysis-in-path subprojects \
        --headers \
        --bufferoverrun \
        --loop-hoisting

valgrind *command:
    valgrind \
        --error-exitcode=1 \
        --track-fds=bad \
        --leak-check=full \
        --show-leak-kinds=all \
        --track-origins=yes \
        build/src/rudolph {{command}}
    valgrind \
        --tool=drd \
        --error-exitcode=1 \
        --track-fds=bad \
        --trace-fork-join=yes \
        --trace-mutex=yes \
        build/src/rudolph {{command}}
    valgrind \
        --tool=helgrind \
        --error-exitcode=1 \
        --track-fds=bad \
        --free-is-write=yes \
        build/src/rudolph {{command}}

