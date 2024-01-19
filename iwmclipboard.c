//------------------------------------------------------------------------------
#define   IWM_COPYRIGHT       "(C)2023-2024 iwm-iwama"
#define   IWM_VERSION         "iwmclipboard_20240118"
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
		iClipboard_setText($ARGC, FALSE);
	}
	// -set2
	else if(iCLI_getOptMatch(0, L"-set2",  L"-s2"))
	{
		iClipboard_setText($ARGC, TRUE);
		SetConsoleOutputCP(65001);
		P(
			IESC_TRUE1
			"クリップボードにコピーしました。（%lu行／%lu文字）\n"
			IESC_RESET
			,
			SetText_Row,
			SetText_Len
		);
		iClipboard_print();
		Sleep(2000);
	}
	// -get
	else if(iCLI_getOptMatch(0, L"-get",   L"-g"))
	{
		iClipboard_print();
	}
	// -sget
	else if(iCLI_getOptMatch(0, L"-sget",  L"-sg"))
	{
		iClipboard_setText($ARGC, FALSE);
		iClipboard_print();
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
			if(iFchk_existPath(_wp1) && iFchk_DirName(_wp1))
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
		SetText_Row = iwn_search(str, L"\n");
	}

	HGLOBAL hg = GlobalAlloc(GMEM_DDESHARE | GMEM_MOVEABLE, ((u1 + 1) * sizeof(WS)));
	if(hg)
	{
		WS *p1 = GlobalLock(hg);
		iwn_cpy(p1, str);
		GlobalUnlock(hg);
		if(OpenClipboard(NULL))
		{
			EmptyClipboard();
			SetClipboardData(CF_UNICODETEXT, hg);
			CloseClipboard();
		}
	}
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

	WS *wp1 = GlobalLock(hg);
	MS *mp1 = W2M(wp1);
		P1(mp1);
	ifree(mp1);
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
