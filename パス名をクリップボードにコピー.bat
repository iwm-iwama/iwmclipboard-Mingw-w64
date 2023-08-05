@echo off

:: "%~1" => 「.. の使い方が誤っています。」対策
if "%~1" == "" (
	echo フォルダ／ファイルをドラッグしてください。
	echo.
	pause
) else (
	iwmclipboard.exe -set %*
)

''iwmclipboard.exe -get
''pause
