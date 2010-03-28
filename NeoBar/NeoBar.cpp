#include "NeoBar.h"
#include <strsafe.h>
#include <string>
#include <Windows.h>
#include "AntTweakBar.h"



void NeoBar::m_CreateGUI()
{
	Neo = TwNewBar( name.c_str());

	TwDefine( "GLOBAL help='DxHax is presented to you by Hunter.' " );
	TwDefine( "DxHax 1.0 text=dark");

	TwAddButton( Neo, "Information", NULL,NULL, "label='Hunter Presents'");
	TwAddButton( Neo, "Information2", NULL,NULL, "label='DxHax InGame Hack!'");

	TwAddButton( Neo, "button1",NULL,NULL, "label='Button Example' group='Memory Hacks' ");
	TwAddButton( Neo, "button2", NULL,NULL, "label='Run Speed'  group='Memory Hacks' ");
	TwAddButton( Neo, "button3", NULL,NULL, "label='GM/DEV Mode' group='Memory Hacks' ");
	
	TwAddSeparator( Neo, "Seperate", NULL);


}

int TW_CALL NeoBar::DrawNeo()
{
	return TwDraw();
}


int TW_CALL NeoBar::WndProcCallBack( void *wnd, unsigned int msg, unsigned int _W64 wParam, int _W64 lParam )
{
	return TwEventWin(wnd, msg, wParam,lParam);
}


bool NeoBar::InitiateNeo( TwGraphAPI mode, void* device )
{
	if(TwInit( mode, device))
	{
        return true;
	}
	return false;
}
