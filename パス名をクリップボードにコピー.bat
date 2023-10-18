@echo off

if "%~1" == "" (
	echo フォルダ／ファイルをドラッグしてください。
	echo.
	pause
	exit
)

iwmclipboard.exe -set2 %*

::iwmclipboard.exe -get
::pause
