//------------------------------------------------------------------------------
#define   IWM_COPYRIGHT       "(C)2023-2024 iwm-iwama"
#define   IWM_VERSION         "iwmclipboard_20240519"
//------------------------------------------------------------------------------
#include "lib_iwmutil2.h"

INT       main();
VOID      subClipboard_set(INT argc, BOOL bGetLenRow);
VOID      subClipboard_print();
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
		print_help();
		imain_end();
	}

	// -v | --version
	if(iCLI_getOptMatch(0, L"-v", L"--version"))
	{
		print_version();
		imain_end();
	}

	// -set
	if(iCLI_getOptMatch(0, L"-set",   L"-s"))
	{
		subClipboard_set($ARGC, FALSE);
	}
	// -set2
	else if(iCLI_getOptMatch(0, L"-set2",  L"-s2"))
	{
		subClipboard_set($ARGC, TRUE);
		subClipboard_print();
		NL();
		P(
			IESC_TRUE1
			"クリップボードにコピーしました。（%lu文字／%lu行）\n"
			IESC_RESET
			,
			subClipboard.len,
			subClipboard.row
		);
		Sleep(2000);
	}
	// -get
	else if(iCLI_getOptMatch(0, L"-get",   L"-g"))
	{
		subClipboard_print();
	}
	// -sget
	else if(iCLI_getOptMatch(0, L"-sget",  L"-sg"))
	{
		subClipboard_set($ARGC, FALSE);
		subClipboard_print();
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

	///idebug_map(); ifree_all(); idebug_map();

	// 最終処理
	imain_end();
}

VOID
subClipboard_set(
	INT argc,
	BOOL bGetLenRow
)
{
	WS *str = 0;
	UINT64 u1 = 0;

	if(argc < 2)
	{
		str = iCLI_GetStdin();
	}
	else
	{
		WS *pJoin = L"\n";
		u1 = iwan_strlen($ARGV) + (iwan_size($ARGV) * wcslen(pJoin));
		str = icalloc_WS(u1);
		WS *pEnd = str;

		for(UINT64 _u1 = 1; _u1 < $ARGC; _u1++)
		{
			WS *_wp1 = $ARGV[_u1];
			pEnd += iwn_cpy(pEnd, _wp1);
			if(iFchk_existPath(_wp1) && iFchk_DirName(_wp1))
			{
				pEnd += iwn_cpy(pEnd, L"\\");
			}
			pEnd += iwn_cpy(pEnd, pJoin);
		}
	}

	if(bGetLenRow)
	{
		subClipboard.len = wcslen(str);
		subClipboard.row = iwn_searchCnt(str, L"\n", FALSE);
	}

	iClipboard_setText(str);
}

VOID
subClipboard_print()
{
	WS *wp1 = iClipboard_getText();
		P1W(wp1);
	ifree(wp1);
}

VOID
print_version()
{
	P1(IESC_STR2);
	LN(80);
	P(
		" %s\n"
		"    %s+%s\n"
		,
		IWM_COPYRIGHT,
		IWM_VERSION,
		LIB_IWMUTIL_VERSION
	);
	LN(80);
	P1(IESC_RESET);
}

VOID
print_help()
{
	MS *_cmd = "iwmclipboard.exe";

	print_version();
	P(
		IESC_TITLE1	" クリップボード "	IESC_RESET	"\n\n"
		IESC_STR1	"    %s"
		IESC_OPT2	" [Option]"
		IESC_OPT1	" [PATH ...]\n\n\n"
		IESC_LBL1	" (例１)"
		IESC_STR1	" -set2 引数渡し／パス名のみ\n"
					"    %s"
		IESC_OPT2	" -set2"
		IESC_OPT1	" \"c:\" \"d:\"\n\n"
		IESC_LBL1	" (例２)"
		IESC_STR1	" -set2 パイプ渡し／文字列\n"
		IESC_OPT1	"    tasklist /v /fo csv |"
		IESC_STR1	" %s"
		IESC_OPT2	" -set2\n\n\n"
		,
		_cmd,
		_cmd,
		_cmd
	);
	P1(
		IESC_OPT2	" [Option]\n"
		IESC_OPT21	"    -set PATH ... | -s PATH ...\n"
		IESC_STR1	"        クリップボードにコピー\n\n"
		IESC_OPT21	"    -set2 PATH ... | -s2 PATH ...\n"
		IESC_STR1	"        クリップボードにコピー／情報表示\n\n"
		IESC_OPT21	"    -get | -g\n"
		IESC_STR1	"        クリップボードの内容を表示\n\n"
		IESC_OPT21	"    -sget | -sg\n"
		IESC_STR1	"        クリップボードにコピー／内容を表示\n\n"
		IESC_OPT21	"    -clear | -c\n"
		IESC_STR1	"        クリップボードをクリア\n\n"
	);
	P1(IESC_STR2);
	LN(80);
	P1(IESC_RESET);
}
