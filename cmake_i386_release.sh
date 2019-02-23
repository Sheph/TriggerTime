DIR_NAME=`basename ${PWD}`
mkdir -p ../${DIR_NAME}-i386-release
cd ../${DIR_NAME}-i386-release
cmake -G "Eclipse CDT4 - Unix Makefiles" -DCMAKE_BUILD_TYPE="Release" -DUSE_LUAJIT=1 -DUSE_LUAJIT_VALGRIND=0 -DBUILD_DOC=1 -DUSE_STEAM_API=1 -D_ECLIPSE_VERSION=4.3 \
-DCMAKE_INSTALL_PREFIX=${PWD}/../${DIR_NAME}-i386-release-install \
-DCMAKE_C_FLAGS="-m32 -march=i686" \
-DCMAKE_CXX_FLAGS="-m32 -march=i686" \
-DCMAKE_ASM_FLAGS="-m32 -march=i686" \
../${DIR_NAME}
