【実行に必要なファイル】

	iwmclipboard.exe

【このプログラムについて】

	Win32Apiのクリップボード(SET,GET)を実装。

	(例１) -set2 引数渡し
		iwmclipboard.exe -set2 "c:" "d:"

	(例２) -set2 パイプ渡し
		dir | iwmclipboard.exe -set2
