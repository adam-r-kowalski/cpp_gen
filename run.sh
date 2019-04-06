if [[ $# -eq 0 ]]; then
    cd build
else
    cd build_$1
fi

cmake --build . -j 8
mv compile_commands.json ..
./cpp_gen
