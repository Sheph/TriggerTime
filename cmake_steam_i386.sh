DIR_NAME=`basename ${PWD}`
mkdir -p ../${DIR_NAME}-steam-i386
cd ../${DIR_NAME}-steam-i386
cmake -G "Eclipse CDT4 - Unix Makefiles" -DCMAKE_BUILD_TYPE="Release" -DUSE_LUAJIT=1 -DUSE_LUAJIT_VALGRIND=0 -DBUILD_DOC=1 -DUSE_STEAM_API=1 -D_ECLIPSE_VERSION=4.3 \
-DCROSSCOMPILING=1 \
-DCMAKE_FIND_ROOT_PATH=${HOME}/steam_runtime/runtime/i386 \
-DCMAKE_INSTALL_PREFIX=${HOME}/steam_ContentBuilder/content/linux_i386 \
-DCMAKE_C_FLAGS="-m32 -march=i686" \
-DCMAKE_CXX_FLAGS="-m32 -march=i686" \
-DCMAKE_ASM_FLAGS="-m32 -march=i686" \
-DCMAKE_C_COMPILER="${HOME}/steam_runtime/bin/gcc" \
-DCMAKE_CXX_COMPILER="${HOME}/steam_runtime/bin/g++" \
../${DIR_NAME}
