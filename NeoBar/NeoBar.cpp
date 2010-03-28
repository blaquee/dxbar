#include "NeoBar.h"
#include "NeoHax.h"
#include <strsafe.h>
#include <string>
#include <Windows.h>
#include "AntTweakBar.h"

typedef struct POSXYZ
{
	float X;
	float Y;
	float Z;
}myHax;

myHax elem;


//Set up the ATB Structure.
TwStructMember myStruct[] = {
	{ "X", TW_TYPE_FLOAT, offsetof(myHax, X), "" },
	{ "Y", TW_TYPE_FLOAT, offsetof(myHax, Y), "" },
	{ "Z", TW_TYPE_FLOAT, offsetof(myHax, Z), "" }
};




void NeoBar::m_CreateGUI()
{
	Neo = TwNewBar( name.c_str());

	TwDefine( "GLOBAL help='NeoHax is presented to you by CUG.' " );
	TwDefine( "NeoHax 1.0 text=dark");

	TwType myHack = TwDefineStruct( "MyHack", myStruct, 3, sizeof( myHax ), NULL, NULL  );

	TwAddButton( Neo, "Information", NULL,NULL, "label='CUG Presents' ");
	TwAddButton( Neo, "Information2", NULL,NULL, "label='NEOHAX InGameOMG Hack!' ");

	TwAddButton( Neo, "button1",NULL,NULL, "label='Button Example' group='Memory Hacks' ");
	TwAddButton( Neo, "button2", NULL,NULL, "label='Run Speed'  group='Memory Hacks' ");
	TwAddButton( Neo, "button3", NULL,NULL, "label='GM/DEV Mode' group='Memory Hacks' ");
	
	TwAddSeparator( Neo, NULL, NULL);

	TwAddVarRW( Neo, "Player Position", myHack, &elem, "group='Memory Hacks' ");


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
