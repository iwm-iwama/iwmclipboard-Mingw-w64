//------------------------------------------------------------------------------
#define   IWM_VERSION         "iwmclipboard_20231018"
#define   IWM_COPYRIGHT       "Copyright (C)2023 iwm-iwama"
//------------------------------------------------------------------------------
#include "lib_iwmutil2.h"

INT       main();
VOID      iClipboard_setText(INT argc, BOOL bGetLenRow);
UINT      SetText_Len = 0;
UINT      SetText_Row = 0;
VOID      iClipboard_print();
VOID      print_version();
VOID      print_help();

INT
main()
{
	// lib_iwmutil2 初期化
	imain_begin();

	// -h | -help
	if(! $ARGC || iCLI_getOptMatch(0, L"-h", L"--help"))
	{
		print_help();
	}
	// -v | -version
	else if(iCLI_getOptMatch(0, L"-v", L"--version"))
	{
		print_version();
	}

	BOOL bSet  = iCLI_getOptMatch(0, L"-set",  NULL);
	BOOL bSet2 = iCLI_getOptMatch(0, L"-set2", NULL);
	BOOL bGet  = iCLI_getOptMatch(0, L"-get",  NULL);
	BOOL bSget = iCLI_getOptMatch(0, L"-sget", NULL);

	// -set
	if(bSet)
	{
		iClipboard_setText($ARGC, FALSE);
	}
	// -set2
	else if(bSet2)
	{
		iClipboard_setText($ARGC, TRUE);
		P("\033[96mクリップボードにコピーしました。（%lu行／%lu文字）\033[0m\n", SetText_Row, SetText_Len);
		P1("\033[94m");
		iClipboard_print();
		P2("\033[0m");

		Sleep(3000);
	}
	// -get
	else if(bGet)
	{
		iClipboard_print();
	}
	// -sget
	else if(bSget)
	{
		iClipboard_setText($ARGC, FALSE);
		iClipboard_print();
	}

	// Debug
	///icalloc_mapPrint(); ifree_all(); icalloc_mapPrint();

	// 最終処理
	imain_end();
}

VOID
iClipboard_setText(
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
			if(iFchk_DirNameW(_wp1))
			{
				pEnd += iwn_cpy(pEnd, L"\\");
			}
			pEnd += iwn_cpy(pEnd, pJoin);
		}
	}

	u1 = wcslen(str);
	if(bGetLenRow)
	{
		SetText_Len = u1;
		SetText_Row = iwn_searchCnt(str, L"\n");
	}

	// 文字長=0 のとき クリップボードをクリア
	HGLOBAL hg = GlobalAlloc(GMEM_DDESHARE | GMEM_MOVEABLE, ((u1 + 1) * sizeof(WS)));
	if(! hg)
	{
		return;
	}

	WS *p1 = GlobalLock(hg);
	iwn_cpy(p1, str);
	GlobalUnlock(hg);
	ifree(str);

	if(! OpenClipboard(NULL))
	{
		return;
	}

	EmptyClipboard();
	SetClipboardData(CF_UNICODETEXT, hg);
	CloseClipboard();
}

VOID
iClipboard_print()
{
	if(! OpenClipboard(NULL))
	{
		return;
	}

	HANDLE hg = GetClipboardData(CF_UNICODETEXT);
	if(! hg)
	{
		return;
	}

	WS *p1 = GlobalLock(hg);
	P1W(p1);
	GlobalUnlock(hg);
	CloseClipboard();
}

VOID
print_version()
{
	P1(IESC_STR2);
	LN(80);
	P(
		" %s\n"
		"    Ver.%s+%s\n"
		, IWM_COPYRIGHT, IWM_VERSION, LIB_IWMUTIL_VERSION
	);
	LN(80);
	P1(IESC_RESET);
}

VOID
print_help()
{
	MS *_cmd = W2M($CMD);

	print_version();
	P(
		IESC_TITLE1	" クリップボード "
		IESC_RESET	"\n"
		IESC_STR1	"    %s"
		IESC_OPT2	" [Option]"
		IESC_OPT1	" [STR ...]\n\n"
		IESC_LBL1	" (例１)"
		IESC_STR1	" -set  引数渡し\n"
					"    %s"
		IESC_OPT2	" -set"
		IESC_OPT1	" \"c:\" \"d:\"\n\n"
		IESC_LBL1	" (例２)"
		IESC_STR1	" -set2 パイプ渡し\n"
		IESC_OPT1	"    dir |"
		IESC_STR1	" %s"
		IESC_OPT2	" -set2\n\n"
		, _cmd, _cmd, _cmd, _cmd
	);
	P1(
		IESC_OPT2	" [Option]\n"
		IESC_OPT21	"    -set STR ...\n"
		IESC_STR1	"        クリップボードにコピー\n\n"
		IESC_OPT21	"    -set2 STR ...\n"
		IESC_STR1	"        クリップボードにコピー／情報表示\n\n"
		IESC_OPT21	"    -get\n"
		IESC_STR1	"        クリップボードの内容を表示\n\n"
		IESC_OPT21	"    -sget\n"
		IESC_STR1	"        クリップボードにコピー／内容を表示\n\n"
	);
	P1(IESC_STR2);
	LN(80);
	P1(IESC_RESET);

	ifree(_cmd);
}
