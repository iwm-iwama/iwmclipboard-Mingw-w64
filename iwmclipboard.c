//------------------------------------------------------------------------------
#define   IWM_VERSION         "iwmClipPathname_20230802"
#define   IWM_COPYRIGHT       "Copyright (C)2023 iwm-iwama"
//------------------------------------------------------------------------------
#include "lib_iwmutil2.h"

INT       main();
UINT64    iClipboard_setText(WS *str);
VOID      iClipboard_print();
VOID      print_version();
VOID      print_help();

#define   CLR_RESET           "\033[0m"
#define   CLR_TITLE1          "\033[38;2;250;250;250m\033[104m" // 白／青
#define   CLR_OPT1            "\033[38;2;250;150;150m"          // 赤
#define   CLR_OPT2            "\033[38;2;150;150;250m"          // 青
#define   CLR_OPT21           "\033[38;2;80;250;250m"           // 水
#define   CLR_OPT22           "\033[38;2;250;100;250m"          // 紅紫
#define   CLR_LBL1            "\033[38;2;250;250;100m"          // 黄
#define   CLR_LBL2            "\033[38;2;100;100;250m"          // 青
#define   CLR_STR1            "\033[38;2;225;225;225m"          // 白
#define   CLR_STR2            "\033[38;2;175;175;175m"          // 銀

INT
main()
{
	// lib_iwmutil 初期化
	iCLI_getCommandLine(); //=> $CMD, $ARGC, $ARGV
	iConsole_EscOn();

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
	// -get
	else if(iCLI_getOptMatch(0, L"-get", NULL))
	{
		iClipboard_print();
	}
	// -set
	else if(iCLI_getOptMatch(0, L"-set", NULL))
	{
		WS *pJoin = L"\n";
		UINT64 u1 = iwan_strlen($ARGV) + (iwan_size($ARGV) * wcslen(pJoin));
		WS *rtn = icalloc_WS(u1);
		WS *pEnd = rtn;

		for(UINT64 _u1 = 1; _u1 < $ARGC; _u1++)
		{
			WS *_wp1 = $ARGV[_u1];
			pEnd += iwn_cpy(pEnd, _wp1);
			if(iFchk_typePathW(_wp1) == 1)
			{
				pEnd += iwn_cpy(pEnd, L"\\");
			}
			pEnd += iwn_cpy(pEnd, pJoin);
		}

		UINT64 uCnt = iClipboard_setText(rtn);
		UINT64 uNL = (uCnt ? iwn_searchCnt(rtn, L"\n") : 0);

		P("\033[96mクリップボードにコピーしました。（%llu行／%llu文字）\033[0m\n", uNL, uCnt);
		P1("\033[94m");
		iClipboard_print();
		P1("\033[0m");

		Sleep(2000);
	}

	// Debug
	/// icalloc_mapPrint(); ifree_all(); icalloc_mapPrint();

	// 最終処理
	imain_end();
}

UINT64
iClipboard_setText(
	WS *str
)
{
	// 文字長=0 のとき クリップボードをクリア
	UINT64 rtn = iwn_len(str);
	HGLOBAL hg = GlobalAlloc(GMEM_DDESHARE | GMEM_MOVEABLE, ((rtn + 1) * sizeof(WS)));
	if(! hg)
	{
		return 0;
	}
	WS *p1 = GlobalLock(hg);
	iwn_cpy(p1, str);
	GlobalUnlock(hg);
	if(! OpenClipboard(NULL))
	{
		return 0;
	}
	EmptyClipboard();
	SetClipboardData(CF_UNICODETEXT, hg);
	CloseClipboard();
	return rtn;
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
	P(CLR_STR2);
	LN(80);
	P(" %s\n", IWM_COPYRIGHT);
	P("    Ver.%s+%s\n", IWM_VERSION, LIB_IWMUTIL_VERSION);
	LN(80);
	P(CLR_RESET);
}

VOID
print_help()
{
	MS *_cmd = W2M($CMD);

	print_version();
	P("%s サンプル %s\n", CLR_TITLE1, CLR_RESET);
	P("%s    %s %s[Option] %s[STR ...]\n", CLR_STR1, _cmd, CLR_OPT2, CLR_OPT1);
	P("\n");
	P("%s (例１)\n", CLR_LBL1);
	P("%s    %s %s-set %s\"c:\" %s\"d:\"\n", CLR_STR1, _cmd, CLR_OPT2, CLR_OPT1, CLR_OPT1);
	P("\n");
	P("%s (例２)\n", CLR_LBL1);
	P("%s    %s %s-get\n", CLR_STR1, _cmd, CLR_OPT2);
	P("\n");
	P("%s [Option]\n", CLR_OPT2);
	P("%s    -set STR ...\n", CLR_OPT21);
	P("%s        クリップボードにコピー\n", CLR_STR1);
	P("\n");
	P("%s    -get\n", CLR_OPT21);
	P("%s        クリップボードの内容を表示\n", CLR_STR1);
	P("\n");
	P(CLR_STR2);
	LN(80);
	P(CLR_RESET);

	ifree(_cmd);
}
