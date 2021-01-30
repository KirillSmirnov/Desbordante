mkdir lib
cd lib
git clone https://github.com/google/googletest/ --branch release-1.10.0
cd ..
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=RELEASE
# To perform various checks use one of the following cmds:
# cmake "-DCMAKE_CXX_CLANG_TIDY=/usr/bin/clang-tidy;-checks=*,-llvmlibc*,-llvm-header-guard,-llvm-include-order,-modernize-use-trailing-return-type;-header-filter=.*;-extra-arg=-std=gnu++17" ..
# cmake "-DCMAKE_CXX_CPPCHECK=/usr/bin/cppcheck;--std=c++11" ..
# cmake "-DCMAKE_CXX_CPPLINT=/usr/local/bin/cpplint;--linelength=79" ..
make
