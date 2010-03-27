#include "NeoBar.h"
#include <strsafe.h>
#include <string>
#include <Windows.h>
#include "AntTweakBar.h"



void NeoBar::m_CreateGUI()
{
	Neo = TwNewBar( name.c_str());

	TwDefine( "GLOBAL help='NeoHax is presented to you by CUG.' " );
	TwDefine( "NeoHax 1.0 text=dark");

	TwAddButton( Neo, "Information", NULL,NULL, "label='CUG Presents'");
	TwAddButton( Neo, "Information2", NULL,NULL, "label='NEOHAX InGameOMG Hack!'");

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
	if( mode == TW_DIRECT3D9)
	{
		if( !TwInit( mode, device) )
		{
			return false;
		}
		return true;
	}
	return false;
}