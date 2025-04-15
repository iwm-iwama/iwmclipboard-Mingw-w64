//------------------------------------------------------------------------------
#define   IWM_COPYRIGHT       "(C)2023-2025 iwm-iwama"
#define   IWM_FILENAME        "iwmclipboard"
#define   IWM_UPDATE          "20250413"
//------------------------------------------------------------------------------
#include "lib_iwmutil2.h"

INT       main();
VOID      subClipboard_set(UINT uArgc, BOOL bGetLenRow);
WS        *rtnClipboard_get(BOOL bGetLenRow);
VOID      print_version();
VOID      print_help();

struct Struct_subClipboard
{
	UINT len; // 文字数／'\r' '\n' 含む
	UINT row; // 行数
}
subClipboard = {
	.len = 0,
	.row = 0
};

INT
main()
{
	// lib_iwmutil2 初期化
	imain_begin();

	///iCLI_VarList();

	// -h | --help
	if(! $ARGC || iCLI_getOptMatch(0, L"-h", L"--help"))
	{
		print_version();
		print_help();
	}
	// -v | --version
	else if(iCLI_getOptMatch(0, L"-v", L"--version"))
	{
		print_version();
	}
	// -set
	else if(iCLI_getOptMatch(0, L"-set",   L"-s"))
	{
		subClipboard_set($ARGC, FALSE);
	}
	// -set2
	else if(iCLI_getOptMatch(0, L"-set2",  L"-s2"))
	{
		subClipboard_set($ARGC, TRUE);
		QP2W(iClipboard_getText());
		P(
			IESC_LBL3
			"クリップボードにコピーしました。（%u文字／%u行）\n"
			IESC_RESET
			, subClipboard.len
			, subClipboard.row
		);
		Sleep(2000);
	}
	// -get
	else if(iCLI_getOptMatch(0, L"-get",   L"-g"))
	{
		QP2W(rtnClipboard_get(FALSE));
	}
	// -get2
	else if(iCLI_getOptMatch(0, L"-get2",   L"-g2"))
	{
		QP2W(rtnClipboard_get(TRUE));
		P(
			IESC_LBL2
			"クリップボードの内容（%u文字／%u行）\n"
			IESC_RESET
			, subClipboard.len
			, subClipboard.row
		);
		Sleep(2000);
	}
	// -clear
	else if(iCLI_getOptMatch(0, L"-clear", L"-c"))
	{
		if(OpenClipboard(NULL))
		{
			EmptyClipboard();
			CloseClipboard();
		}
	}

	// Debug
	///idebug_map();
	///ifree_all();
	///idebug_map();

	// 最終処理
	imain_end();
}

VOID
subClipboard_set(
	UINT uArgc,
	BOOL bGetLenRow
)
{
	WS *rtn = 0;

	if(uArgc < 2)
	{
		rtn = iCLI_getStdin(FALSE);
	}
	else
	{
		$struct_iVBW *iVBW = iVBW_alloc();
			UINT u1 = 1;
			while($ARGV[u1])
			{
				iVBW_push2(iVBW, $ARGV[u1]);
				if(iF_chkExistPath($ARGV[u1]) && iF_chkDirName($ARGV[u1]))
				{
					iVBW_push2(iVBW, L"\\");
				}
				iVBW_push2(iVBW, L"\n");
				++u1;
			}
		rtn = iVBW_free(iVBW);
	}

	if(bGetLenRow)
	{
		subClipboard.len = wcslen(rtn);
		subClipboard.row = iwn_searchCnt(rtn, L"\n");
	}

	iClipboard_setText(rtn);
	ifree(rtn);
}

WS
*rtnClipboard_get(
	BOOL bGetLenRow
)
{
	WS *rtn = iClipboard_getText();
	if(bGetLenRow)
	{
		subClipboard.len = wcslen(rtn);
		subClipboard.row = iwn_searchCnt(rtn, L"\n");
	}
	return rtn;
}

VOID
print_version()
{
	P1(IESC_STR2);
	LN(80);
	P1(
		"\033[2G"	IWM_COPYRIGHT	"\n"
		"\033[5G"	IWM_FILENAME	"_"	IWM_UPDATE	" + "	LIB_IWMUTIL_FILENAME	"\n"
	);
	LN(80);
	P1(IESC_RESET);
}

VOID
print_help()
{
	P1(
		"\033[1G"	IESC_TITLE1	" クリップボード "	IESC_RESET	"\n"
		"\n"
		"\033[5G"	IESC_STR1	IWM_FILENAME	IESC_OPT2	" [Option]"	IESC_OPT1	" [PATH ...]"	"\n"
		"\n"
		"\033[2G"	IESC_LBL1	"(例１) "	IESC_STR1	"-set2 引数渡し／パス名取得"	"\n"
		"\033[5G"	IESC_STR1	IWM_FILENAME	IESC_OPT2	" -set2"	IESC_OPT1	" \"c:\" \"d:\""	"\n"
		"\n"
		"\033[2G"	IESC_LBL1	"(例２) "	IESC_STR1	"-set2 パイプ渡し"	"\n"
		"\033[5G"	IESC_OPT1	"tasklist /v /fo csv | "	IESC_STR1	IWM_FILENAME	IESC_OPT2	" -set2"	"\n"
		"\n"
		"\033[2G"	IESC_OPT2	"[Option]"	"\n"
		"\033[5G"	IESC_OPT21	"-set | -s"	"\n"
		"\033[9G"	IESC_STR1	"クリップボードにコピー"	"\n"
		"\n"
		"\033[5G"	IESC_OPT21	"-set2 | -s2"	"\n"
		"\033[9G"	IESC_STR1	"クリップボードにコピー／情報表示"	"\n"
		"\n"
		"\033[5G"	IESC_OPT21	"-get | -g"	"\n"
		"\033[9G"	IESC_STR1	"クリップボードを表示"	"\n"
		"\n"
		"\033[5G"	IESC_OPT21	"-get2 | -g2"	"\n"
		"\033[9G"	IESC_STR1	"クリップボードを表示／情報表示"	"\n"
		"\n"
		"\033[5G"	IESC_OPT21	"-clear | -c"	"\n"
		"\033[9G"	IESC_STR1	"クリップボードをクリア"	"\n"
		"\n"
		"\033[5G"	IESC_OPT22	"エスケープシーケンス文字を消去"	"\n"
		"\033[9G"	IESC_STR1	"別プログラム"	IESC_LBL3	" iwmesc.exe "	IESC_STR1	"を利用してください"	"\n"
		"\033[9G"	IESC_LBL1	"(例) "	IESC_STR1	IWM_FILENAME	" -h | iwmesc -text | "	IWM_FILENAME	" -s2"	"\n"
		"\n"
	);
	P1(IESC_STR2);
	LN(80);
	P1(IESC_RESET);
}
