// bumper.cpp : Defines the entry point for the application.
//


#include "stdafx.h"
#include <objbase.h>
#include <stdio.H>
#import "shdocvw.dll"

#define COMPILE_MULTIMON_STUBS
#include <multimon.h>

HWND t;
char cname[1024];

void GetTop()
{
	t = GetForegroundWindow();
	if (t)
		GetClassName(t, cname, 1024);
}

// unused
/*
int FindIE(HWND top)
{
	t = NULL;
	HWND loct = GetWindow(top, GW_CHILD);

	while (loct) {
		if (loct && FindIE(loct)) return 1;
		if (GetClassName(loct, cname, 1024) > 0) {
			if (strcmp(cname, "Internet Explorer_Server") == 0) {
				t = loct;
				return 1;
			}
		}
		loct = GetWindow(loct, GW_HWNDNEXT);
	}

	return 0;
}
*/

typedef SHDocVw::IWebBrowser2Ptr wb2;


wb2 GetIE(HWND w)
{
	SHDocVw::IShellWindowsPtr shp; 
	shp.CreateInstance(__uuidof(SHDocVw::ShellWindows)); 

	int count = shp->GetCount();

	IDispatchPtr spDisp;

	// ---- get the browsers 
	for (long i = 0; i < count; i++) {
		_variant_t va(i, VT_I4);
		spDisp = shp->Item(va);

		SHDocVw::IWebBrowser2Ptr spBrowser(spDisp);
		if (spBrowser != NULL)
		{
			HWND hw;

			try {
				hw = (HWND)spBrowser->GetHWND();
			} catch ( ... ) {
				hw = NULL;
			}
			if (hw == w) return spBrowser;

		}
	}

	return NULL;
}

void Back()
{
	GetTop();
	if (t == NULL) return;

	if (!strcmp(cname, "IEFrame") ||
		!strcmp(cname, "CabinetWClass")) {
		//FindIE(t);
		if (t) {
			wb2 wb = GetIE(t);
			if (wb) {
				wb->raw_GoBack();
			}
		}
	}
}

void Forward()
{
	GetTop();
	if (t == NULL) return;

	if (!strcmp(cname, "IEFrame") ||
		!strcmp(cname, "CabinetWClass")) {
		//FindIE(t);
		if (t) {
			wb2 wb = GetIE(t);
			if (wb) {
				wb->raw_GoForward();
			}
		}
	}
}

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{

	// try to register our window class, and exit if it's there
	CoInitialize(NULL);

	int notask = false;

	if (strncmp(lpCmdLine, "-notask", 7)) {
		notask = true;
	}

	POINT loc;

	// poll the mouse at 100hz
	int ctr = 0;
	int reset = true;
	int maxx, maxy;
	int minx, miny;
	const int thresh = 10;	// reset size

	while (1) {

		MSG msg;
		while (::PeekMessage(&msg, NULL, 0, 0, TRUE)) {
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}

		RECT sa;
		if ((ctr & 127) == 0) {
			maxx = GetSystemMetrics(SM_CXVIRTUALSCREEN);
			maxy = GetSystemMetrics(SM_CYVIRTUALSCREEN);

			minx = GetSystemMetrics(SM_XVIRTUALSCREEN);
			miny = GetSystemMetrics(SM_YVIRTUALSCREEN);

			maxx -= minx;
			maxy -= miny;

			sa.left = minx;
			sa.top = miny;
			sa.right = maxx;
			sa.bottom = maxy;
			sa.bottom -= 5;
		}

		ctr ++;

		::GetCursorPos(&loc);

		if (notask) {
			::ClipCursor(&sa);
		}

		sa.left -= 24;

		if (loc.x == minx && reset) {
			reset = false;
			// back!
			Back();
		}

		if (loc.x == (maxx-1) && reset) {
			reset = false;
			// forward!
			Forward();
		}

		if (loc.x > thresh && ((maxx - loc.x) > thresh)) {
			reset = true;
		}

		if (loc.y > sa.bottom - 40) {
			if (loc.x < 2) {
				loc.x = 2;
				::SetCursorPos(loc.x, loc.y);
			}
		}

		Sleep(10);
	}
	
	return 0;
}



