#include "NeoBar.h"
#include "NeoHax.h"
#include <strsafe.h>
#include <string>
#include <Windows.h>
#include "AntTweakBar.h"

//Setting up a Hack Structure for ATB
typedef struct Hacks
{
	DWORD_PTR addr;
	const char* name;
}myHax;

myHax elem;


//Set up the ATB Structure.
TwStructMember myStruct[] = {
	{ "address", TW_TYPE_INT32, offsetof(myHax, addr), "label=Address group=Memhacks " },
	{ "name", TW_TYPE_CDSTRING, offsetof(myHax, name ), "label='Name of Hack' group=MemHacks"}
};

//Defines the type for the ATB struct.
TwType myHack = TwDefineStruct( "MemHacks", myStruct, 2, sizeof( myHax ), NULL, NULL  );

void NeoBar::m_CreateGUI()
{
	Neo = TwNewBar( name.c_str());

	TwDefine( "GLOBAL help='NeoHax is presented to you by CUG.' " );
	TwDefine( "NeoHax 1.0 text=dark");

	TwAddButton( Neo, "Information", NULL,NULL, "label='CUG Presents' ");
	TwAddButton( Neo, "Information2", NULL,NULL, "label='NEOHAX InGameOMG Hack!' ");

	TwAddButton( Neo, "button1",NULL,NULL, "label='Button Example' group='Memory Hacks' ");
	TwAddButton( Neo, "button2", NULL,NULL, "label='Run Speed'  group='Memory Hacks' ");
	TwAddButton( Neo, "button3", NULL,NULL, "label='GM/DEV Mode' group='Memory Hacks' ");
	
	TwAddSeparator( Neo, NULL, NULL);

	TwAddVarRW( Neo, "Grouped Hacks", myHack, &elem, "group='Memory Hacks' " );


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
