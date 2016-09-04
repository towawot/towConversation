@echo on

rem ===========================================

rem .dllファイルのインストール先パスを指定
set DLL_DIR=D:\skyrim\Data\SKSE\Plugins

rem ===========================================

if %1 == "" goto END
if not exist "%DLL_DIR%" goto END

copy %1 "%DLL_DIR%"

:END
