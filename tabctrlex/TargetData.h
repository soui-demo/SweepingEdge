#pragma once
#include "SBrowserTab.h"
#include "wtl.mini\souimisc.h"
#include "control\Stabctrl.h"

struct TargetData
{
	SBrowserTab* tabWnd;//tab ±Í«©
	//SBrowserTab* tabPage;// µº “≥
	//HWND parenthWnd;
	CPoint Pos;
	SPrintWindow * printwnd;
	STabPage *page;
	SHostWindowFactory *hostfactory;
};