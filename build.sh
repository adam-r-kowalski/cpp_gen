if [[ $# -eq 0 ]]; then
    mkdir -p build
    cd build
    cmake -G Ninja ..
else
    mkdir -p build_$1
    cd build_$1
    cmake -G Ninja -D CMAKE_BUILD_TYPE=$1 ..
fi
