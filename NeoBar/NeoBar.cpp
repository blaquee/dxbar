/*

Copyright (c) 2009, Hunter and genuine (http://mp.reversing.us)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the MPReversing nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY Hunter and genuine (http://mp.reversing.us) ''AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL Hunter and genuine (http://mp.reversing.us) BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/


#include "NeoBar.h"
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

	TwDefine( "GLOBAL help='DxHax is presented to you by genuine/Hunter.' " );
	TwDefine( "DxHax 1.0 text=dark");

	TwType myHack = TwDefineStruct( "MyHack", myStruct, 3, sizeof( myHax ), NULL, NULL  );

	TwAddButton( Neo, "Information", NULL,NULL, "label='genuine/Hunter Presents'");
	TwAddButton( Neo, "Information2", NULL,NULL, "label='DxHax InGame Hack!'");

	TwAddButton( Neo, "button1",NULL,NULL, "label='Button Example' group='Memory Hacks' ");
	TwAddButton( Neo, "button2", NULL,NULL, "label='Run Speed'  group='Memory Hacks' ");
	TwAddButton( Neo, "button3", NULL,NULL, "label='GM/DEV Mode' group='Memory Hacks' ");
	
	TwAddSeparator( Neo, "Seperate", NULL);

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
