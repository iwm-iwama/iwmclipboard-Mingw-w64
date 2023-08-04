@echo off

if "%1" == "" (
	echo フォルダ／ファイルをドラッグしてください。
	echo.
	pause
) else (
	iwmclipboard.exe -set %*
)

''iwmclipboard.exe -get
''pause
