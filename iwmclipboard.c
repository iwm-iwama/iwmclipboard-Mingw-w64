//------------------------------------------------------------------------------
#define   IWM_COPYRIGHT       "(C)2023-2025 iwm-iwama"
#define   IWM_FILENAME        "iwmclipboard"
#define   IWM_UPDATE          "20250203"
//------------------------------------------------------------------------------
#include "lib_iwmutil2.h"

INT       main();
VOID      subClipboard_set(INT argc, BOOL bGetLenRow);
VOID      subClipboard_print(BOOL bEscOn);
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
		subClipboard_print(TRUE);
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
		subClipboard_print(TRUE);
	}
	// -get2
	else if(iCLI_getOptMatch(0, L"-get2",   L"-g2"))
	{
		subClipboard_print(FALSE);
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
		str = iCLI_GetStdin(FALSE);
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
subClipboard_print(
	BOOL bEscOn // TRUE=ソノママ印字／FALSE=ESC(\033[...)を消去して印字
)
{
	WS *wp1 = iClipboard_getText();
		if(bEscOn)
		{
			P1W(wp1);
		}
		else
		{
			WS *wp2 = iws_withoutESC(wp1);
				P1W(wp2);
			ifree(wp2);
		}
	ifree(wp1);
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
	print_version();
	P1(
		"\033[1G"	IESC_TITLE1	" クリップボード "	IESC_RESET	"\n"
		"\n"
		"\033[5G"	IESC_STR1	IWM_FILENAME	IESC_OPT2	" [Option]"	IESC_OPT1	" [PATH ...]"	"\n"
		"\n"
		"\033[2G"	IESC_LBL1	"(例１)"	IESC_STR1	" -set2 引数渡し／パス名のみ"	"\n"
		"\033[5G"	IESC_STR1	IWM_FILENAME	IESC_OPT2	" -set2"	IESC_OPT1	" \"c:\" \"d:\""	"\n"
		"\n"
		"\033[2G"	IESC_LBL1	"(例２)"	IESC_STR1	" -set2 パイプ渡し／文字列"	"\n"
		"\033[5G"	IESC_OPT1	"tasklist /v /fo csv | "	IESC_STR1	IWM_FILENAME	IESC_OPT2	" -set2"	"\n"
		"\n"
		"\033[2G"	IESC_OPT2	"[Option]"	"\n"
		"\033[5G"	IESC_OPT21	"-set PATH ... | -s PATH ..."	"\n"
		"\033[9G"	IESC_STR1	"クリップボードにコピー"	"\n"
		"\n"
		"\033[5G"	IESC_OPT21	"-set2 PATH ... | -s2 PATH ..."	"\n"
		"\033[9G"	IESC_STR1	"クリップボードにコピー／情報表示"	"\n"
		"\n"
		"\033[5G"	IESC_OPT21	"-get | -g"	"\n"
		"\033[9G"	IESC_STR1	"クリップボードを表示"	"\n"
		"\n"
		"\033[5G"	IESC_OPT21	"-get2 | -g2"	"\n"
		"\033[9G"	IESC_STR1	"クリップボードを表示（ESC文字除去）"	"\n"
		"\n"
		"\033[5G"	IESC_OPT21	"-clear | -c"	"\n"
		"\033[9G"	IESC_STR1	"クリップボードをクリア"	"\n"
		"\n"
	);
	P1(IESC_STR2);
	LN(80);
	P1(IESC_RESET);
}
