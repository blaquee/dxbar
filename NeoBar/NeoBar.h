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
	NeoBar(): name("NeoHax")
	{};

	virtual ~NeoBar()
	{
		TwTerminate();
	};

private:

	//bool isActivated; 
	TwBar *Neo;
	LPDIRECT3DDEVICE9 p_D3D9; //DirectX9 Device. Needed to Initialize ATB
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