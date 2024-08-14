//------------------------------------------------------------------------------
#define   IWM_COPYRIGHT       "(C)2024 iwm-iwama"
#define   IWM_FILENAME        "iwmclipboard+FLTK"
#define   IWM_UPDATE          "20240519"
//------------------------------------------------------------------------------
extern "C"
{
	#include "lib_iwmutil2.h"
}
#include <FL/Fl.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Window.H>

INT       main();
VOID      subClipboard_drop();
VOID      subClipboard_getARGV();

//--------------------------------------------------------------------------------
// TEXT_BUF
//--------------------------------------------------------------------------------
class
TEXT_BUF : public Fl_Text_Buffer
{};

//--------------------------------------------------------------------------------
// TEXT_BOX
//--------------------------------------------------------------------------------
class
TEXT_BOX : public Fl_Text_Display
{
	Fl_Text_Buffer *TextBuf;

	public:
		TEXT_BOX(INT, INT, INT, INT, CONST MS*, Fl_Text_Buffer*);

	private:
		auto handle(INT) -> INT override;
};

TEXT_BOX::TEXT_BOX(
	INT x,
	INT y,
	INT w,
	INT h,
	CONST MS *lbl,
	Fl_Text_Buffer *TextBuf
) : Fl_Text_Display(x, y, w, h, lbl)
{
	this->TextBuf = TextBuf;
	textfont(FL_FREE_FONT);
	textcolor(FL_BLACK);
	textsize(14);
	linenumber_width(48);
}

INT
TEXT_BOX::handle(
	INT event
)
{
	switch(event)
	{
		case FL_DND_DRAG:
		case FL_DND_ENTER:
		case FL_DND_RELEASE:
		{
			return 1;
		}
		case FL_PASTE:
		{
			subClipboard_drop();
			// TEXT_BOX の表示更新は ChildThread() が行う
			return 1;
		}
		default:
		{
			return Fl_Text_Display::handle(event);
		}
	}
}

//--------------------------------------------------------------------------------
// WINDOW
//--------------------------------------------------------------------------------
TEXT_BUF *TextBuf;
TEXT_BOX *TextBox1;

DWORD WINAPI
ChildThread(
	VOID*
)
{
	while(TRUE)
	{
		Fl::lock();
			MS *mp1 = W2M(iClipboard_getText());
				TextBuf->text(mp1);
				TextBox1->buffer(TextBuf);
			ifree(mp1);
		Fl::unlock();
		Fl::awake();
		Sleep(2000);
	}
}

class
WINDOW : Fl_Window
{
		HANDLE Thread_Handle;
		DWORD Thread_Id;
	public:
		WINDOW(INT, INT, INT, INT, CONST MS*);
		~WINDOW();
};

WINDOW::WINDOW(
	INT x,
	INT y,
	INT w,
	INT h,
	CONST MS *title
) : Fl_Window(x, y, w, h, title)
{
	TextBuf = new TEXT_BUF();
	TextBox1 = new TEXT_BOX(0, 0, w, h, "", TextBuf);

	// WINDOW->
	///set_modal(); // 最小化しない
	resizable(*TextBox1);
	end();
	show();

	// Main Thread
	if($ARGC)
	{
		subClipboard_getARGV();
		// TEXT_BOX の表示更新は ChildThread() が行う
	}

	// Child Thread
	Fl::lock();
	Thread_Handle = CreateThread(0, 0, ChildThread, 0, 0, &Thread_Id);
	SetThreadPriority(Thread_Handle, THREAD_PRIORITY_IDLE);
}

WINDOW::~WINDOW()
{
	WaitForSingleObject(Thread_Handle, INFINITE);
	CloseHandle(Thread_Handle);

	if(TextBox1 != nullptr)
	{
		delete TextBox1;
	}
	if(TextBuf != nullptr)
	{
		delete TextBuf;
	}
}

//--------------------------------------------------------------------------------
// main()
//--------------------------------------------------------------------------------
INT
main()
{
	// lib_iwmutil2 初期化
	imain_begin();

	CONST INT sizeW = 480;
	CONST INT sizeH = 160;

	new WINDOW(
		((Fl::w() - sizeW) / 2),
		((Fl::h() - sizeH) / 2),
		sizeW,
		sizeH,
		"クリップボードの内容"
	);

	Fl::run();

	// 最終処理
	imain_end();
}

VOID
subClipboard_drop()
{
	WS *wp1 = M2W(Fl::event_text());
		WS **wa1 = iwaa_split(wp1, L"\n", FALSE);
			$struct_iVBW *IVBW = iVBW_alloc();
				for(UINT _u1 = 0; _u1 < iwan_size(wa1); _u1++)
				{
					iVBW_add(IVBW, wa1[_u1]);
					if(iFchk_DirName(wa1[_u1]))
					{
						if(wa1[_u1][wcslen(wa1[_u1]) - 1] != '\\')
						{
							iVBW_add(IVBW, L"\\");
						}
					}
					iVBW_add(IVBW, L"\n");
				}
				iClipboard_setText(iVBW_getStr(IVBW));
			iVBW_free(IVBW);
		ifree(wa1);
	ifree(wp1);
}

VOID
subClipboard_getARGV()
{
	$struct_iVBW *IVBW = iVBW_alloc();
		for(UINT _u1 = 0; _u1 < $ARGC; _u1++)
		{
			iVBW_add(IVBW, $ARGV[_u1]);
			if(iFchk_DirName($ARGV[_u1]))
			{
				iVBW_add(IVBW, L"\\");
			}
			iVBW_add(IVBW, L"\n");
		}
		iClipboard_setText(iVBW_getStr(IVBW));
	iVBW_free(IVBW);
}
