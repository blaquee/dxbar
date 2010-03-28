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



#ifndef NEOBAR
#define NEOBAR

#include <d3d9.h>
#pragma warning( disable : 4995 ) 
#include <strsafe.h>
#pragma warning( default : 4996 )
#include <string>
#include <Windows.h>
#include "AntTweakBar.h"

//This header file defines the class for the NeoBar Tool


namespace SizeOfTemplate
{

    template<int n>
    struct char_array_wrapper{
        char result[n];
    };

    template<typename T, int s>
    char_array_wrapper<s> the_type_of_the_variable_is_not_an_array(const T (&array)[s]){
    }

}

#define SIZEOF_ARRAY(v) sizeof(SizeOfTemplate::the_type_of_the_variable_is_not_an_array(v).result)


struct HookFunction
{
    PVOID original;
    PVOID hook;
};

namespace
{
	void TW_CALL CopyStdStringToClient(std::string& destinationClientString, const std::string& sourceLibraryString)
	{
		destinationClientString = sourceLibraryString;
	}
}


class NeoBar{

public:
	NeoBar(): name("DxHax")
	{};

	virtual ~NeoBar()
	{
		TwTerminate();
	};

private:

	//bool isActivated; 
	TwBar *Neo;
	//LPDIRECT3DDEVICE9 p_D3D9; //DirectX9 Device. Needed to Initialize ATB
	std::string name; //Name of TwBar

public:

	bool InitiateNeo(TwGraphAPI mode, void* device);
	void m_CreateGUI();
	int TW_CALL DrawNeo();
	int TW_CALL WndProcCallBack( void *wnd, unsigned int msg, unsigned int _W64 wParam, int _W64 lParam);

public: 
	//User Define CallBack Functions.
	void TW_CALL button( void *data);
	void TW_CALL button2( void * data);

};
#endif //NEOBAR