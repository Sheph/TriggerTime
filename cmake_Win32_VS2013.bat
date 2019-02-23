@mkdir build_Win32_VS2013 2>nul
@cd build_Win32_VS2013
@cmake -DUSE_LUAJIT=1 -DUSE_STEAM_API=1 -DCMAKE_INSTALL_PREFIX="%CD%\..\install_Win32_VS2013" -G "Visual Studio 12 2013" ..
