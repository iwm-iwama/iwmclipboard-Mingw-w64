@echo off

:: "%~1" => �u.. �̎g����������Ă��܂��B�v�΍�
if "%~1" == "" (
	echo �t�H���_�^�t�@�C�����h���b�O���Ă��������B
	echo.
	pause
) else (
	iwmclipboard.exe -set %*
)

''iwmclipboard.exe -get
''pause
