//------------------------------------------------------------------------------
#define   IWM_VERSION         "iwmClipPathname_20230811"
#define   IWM_COPYRIGHT       "Copyright (C)2023 iwm-iwama"
//------------------------------------------------------------------------------
#include "lib_iwmutil2.h"

INT       main();
UINT64    iClipboard_setText(WS *str);
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
	// -get
	else if(iCLI_getOptMatch(0, L"-get", NULL))
	{
		iClipboard_print();
	}
	// -set
	else if(iCLI_getOptMatch(0, L"-set", NULL))
	{
		WS *rtn = 0;

		if($ARGC < 2)
		{
			rtn = iCLI_GetStdin();
		}
		else
		{
			WS *pJoin = L"\n";
			UINT64 u1 = iwan_strlen($ARGV) + (iwan_size($ARGV) * wcslen(pJoin));
			rtn = icalloc_WS(u1);
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
		}

		UINT64 uCnt = iClipboard_setText(rtn);
		UINT64 uNL = (uCnt ? iwn_searchCnt(rtn, L"\n") : 0);

		P("\033[96mクリップボードにコピーしました。（%llu行／%llu文字）\033[0m\n", uNL, uCnt);
		P1("\033[94m");
		iClipboard_print();
		P1("\033[0m");

		Sleep(3000);
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
	P(ICLR_STR2);
	LN(80);
	P(" %s\n", IWM_COPYRIGHT);
	P("    Ver.%s+%s\n", IWM_VERSION, LIB_IWMUTIL_VERSION);
	LN(80);
	P(ICLR_RESET);
}

VOID
print_help()
{
	MS *_cmd = W2M($CMD);

	print_version();
	P("%s サンプル %s\n", ICLR_TITLE1, ICLR_RESET);
	P("%s    %s %s[Option] %s[STR ...]\n", ICLR_STR1, _cmd, ICLR_OPT2, ICLR_OPT1);
	P("\n");
	P("%s (例１) %s-set 引数渡し\n", ICLR_LBL1, ICLR_STR1);
	P("%s    %s %s-set %s\"c:\" %s\"d:\"\n", ICLR_STR1, _cmd, ICLR_OPT2, ICLR_OPT1, ICLR_OPT1);
	P("\n");
	P("%s (例２) %s-set パイプ渡し\n", ICLR_LBL1, ICLR_STR1);
	P("%s    dir | %s%s %s-set\n", ICLR_OPT1, ICLR_STR1, _cmd, ICLR_OPT2);
	P("\n");
	P("%s (例３) %s-get\n", ICLR_LBL1, ICLR_STR1);
	P("%s    %s %s-get\n", ICLR_STR1, _cmd, ICLR_OPT2);
	P("\n");
	P("%s [Option]\n", ICLR_OPT2);
	P("%s    -set STR ...\n", ICLR_OPT21);
	P("%s        クリップボードにコピー\n", ICLR_STR1);
	P("\n");
	P("%s    -get\n", ICLR_OPT21);
	P("%s        クリップボードの内容を表示\n", ICLR_STR1);
	P("\n");
	P(ICLR_STR2);
	LN(80);
	P(ICLR_RESET);

	ifree(_cmd);
}
