@echo on

rem ===========================================

rem .dll�t�@�C���̃C���X�g�[����p�X���w��
set DLL_DIR=D:\skyrim\Data\SKSE\Plugins

rem ===========================================

if %1 == "" goto END
if not exist "%DLL_DIR%" goto END

copy %1 "%DLL_DIR%"

:END
