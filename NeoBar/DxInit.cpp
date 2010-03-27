#include <d3d9.h>
#pragma warning( disable : 4996 ) // disable deprecated warning 
#include <strsafe.h>
#pragma warning( default : 4995 ) //deprecation warnings
#include "NeoBar.h"
#include <Windows.h>
#include "detours.h"
#include "AntTweakBar.h"
#pragma comment(lib, "detours.lib")



LPDIRECT3D9         g_pD3D = NULL; 
LPDIRECT3DDEVICE9   g_pd3dDevice = NULL; 
D3DPRESENT_PARAMETERS   g_D3Dpp;

HWND g_hWnd; //Main window
LONG_PTR g_origWndProc; //the original wndproc (msgloop)

//Instance of our hack
NeoBar neo;


LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

// a table of hooked functions
enum D3DHOOKS 
{
    FUNC_DIRECT_3D_CREATE_9,
    // FUNC_DIRECT_3D_CREATE_9_EX, //unused, see below
    NUMBER_OF_D3D_HOOKS
};


// prototype of hook functions and pointer of the original version
typedef LPDIRECT3D9 (WINAPI *TDirect3DCreate9)(UINT version);
LPDIRECT3D9 WINAPI Direct3DCreate9_Hook(UINT version);


HookFunction d3dHooks[NUMBER_OF_D3D_HOOKS] = 
{
    { Direct3DCreate9, Direct3DCreate9_Hook }
    //{ Direct3DCreate9Ex, Direct3DCreate9Ex_Hook }, //see below

};


// our hooked functions
LPDIRECT3D9 WINAPI Direct3DCreate9_Hook(UINT version)
{
    g_pD3D = ((TDirect3DCreate9)d3dHooks[FUNC_DIRECT_3D_CREATE_9].original)(version);
    return g_pD3D;
}

/* Direct3DCreate9Ex is only available for vista and later
    Since all games I've played runs on xp, it isn't necessary to hook this function
HRESULT WINAPI Direct3DCreate9Ex_Hook(UINT SDKVersion, IDirect3D9Ex**)
{

}*/



static void HookFunctionArray(HookFunction *functions, int size)
{
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    for (int i = 0;i < size;i++)
    {
        DetourAttach(&(PVOID&)functions[i].original, functions[i].hook);  
    }
    DetourTransactionCommit();
}


bool InitD3DHook()
{

	/*if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
		return E_FAIL;

	ZeroMemory( &g_D3Dpp, sizeof( g_D3Dpp ) );
	g_D3Dpp.Windowed = TRUE;
	g_D3Dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	g_D3Dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	g_D3Dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	g_D3Dpp.AutoDepthStencilFormat = D3DFMT_D16;
	g_D3Dpp.hDeviceWindow = hwnd;
	g_D3Dpp.EnableAutoDepthStencil = TRUE;


	if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&g_D3Dpp, &g_pd3dDevice ) ) )
	{
		return E_FAIL;
	}*/

    HookFunctionArray(d3dHooks, NUMBER_OF_D3D_HOOKS);


	return true;
}

bool InitWindowHook(HWND hWnd)
{
    g_origWndProc = GetWindowLongPtr(hWnd, GWL_WNDPROC);
    SetWindowLongPtr(hWnd, GWL_WNDPROC, (LONG_PTR)MsgProc);
    return true;
}


VOID Cleanup()
{
	if( g_pd3dDevice != NULL )
		g_pd3dDevice->Release();

	if( g_pD3D != NULL )
		g_pD3D->Release();
}


VOID Render()
{
	if( NULL == g_pd3dDevice )
		return;

	g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB( 0, 0, 255 ), 1.0f, 0 );

	if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
	{
		neo.DrawNeo();
		g_pd3dDevice->EndScene();
	}
	g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}



LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	if( neo.WndProcCallBack( hWnd, msg, wParam, lParam ) )
		return 0;

	switch( msg )
	{
    case WM_CREATE:
        g_hWnd = hWnd;
        InitWindowHook(hWnd);
        break;

        // in WM_PAINT we let the game render first before rendering our stuff
	case WM_PAINT:
        CallWindowProc((WNDPROC)g_origWndProc, hWnd, msg, wParam, lParam);
		Render();
		ValidateRect( hWnd, NULL );
		return 0;

	case WM_SIZE:

		g_D3Dpp.BackBufferWidth = LOWORD(lParam);
		g_D3Dpp.BackBufferHeight = HIWORD(lParam);
		if( g_D3Dpp.BackBufferWidth > 0 && g_D3Dpp.BackBufferHeight > 0 )
		{
			g_pd3dDevice->Reset( &g_D3Dpp );
			TwWindowSize( g_D3Dpp.BackBufferWidth, g_D3Dpp.BackBufferHeight);
			break;
		}
		break;
	}

	return CallWindowProc((WNDPROC)g_origWndProc, hWnd, msg, wParam, lParam);
}


INT DoHook( HINSTANCE hInst )
{

	if( InitD3DHook() )
	{

		if( !neo.InitiateNeo( TW_DIRECT3D9, g_pd3dDevice ) )
		{
			g_pd3dDevice->Release();
			g_pd3dDevice = NULL;
			g_pD3D->Release();
			g_pD3D = NULL;
			return FALSE;
		}

		neo.m_CreateGUI();
	}

	return 0;
}

BOOL WINAPI DllMain(HINSTANCE hInst, _In_ unsigned _Reason, _In_opt_ void * _Reserved)
{
    if(_Reason == DLL_PROCESS_ATTACH)
    {
        DoHook(hInst);
    }
    
    return TRUE;
}
