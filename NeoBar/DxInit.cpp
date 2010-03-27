// enable C interface
#include <d3d9.h>
#include <io.h>
#include <fcntl.h> 
#include <iostream>
#pragma warning( disable : 4996 ) // disable deprecated warning 

// need to include this file before strsafe
#include "regkey.h"

#pragma warning( default : 4995 ) //deprecation warnings
#include "NeoBar.h"
#include <Windows.h>
#include "detours.h"
#include "AntTweakBar.h"


#pragma comment(lib, "detours.lib")


static void OpenConsole()
{
    int outHandle, errHandle, inHandle;
    FILE *outFile, *errFile, *inFile;
    AllocConsole();
    CONSOLE_SCREEN_BUFFER_INFO coninfo;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
    coninfo.dwSize.Y = 9999;
    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);

    outHandle = _open_osfhandle((long)GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT);
    errHandle = _open_osfhandle((long)GetStdHandle(STD_ERROR_HANDLE),_O_TEXT);
    inHandle = _open_osfhandle((long)GetStdHandle(STD_INPUT_HANDLE),_O_TEXT );

    outFile = _fdopen(outHandle, "w" );
    errFile = _fdopen(errHandle, "w");
    inFile =  _fdopen(inHandle, "r");

    *stdout = *outFile;
    *stderr = *errFile;
    *stdin = *inFile;

    setvbuf( stdout, NULL, _IONBF, 0 );
    setvbuf( stderr, NULL, _IONBF, 0 );
    setvbuf( stdin, NULL, _IONBF, 0 );

    std::ios::sync_with_stdio();

}



typedef HRESULT(WINAPI *TCreateDevice)(LPDIRECT3D9 d3d, UINT Adapter,D3DDEVTYPE DeviceType,HWND hFocusWindow,DWORD BehaviorFlags,D3DPRESENT_PARAMETERS* pPresentationParameters,IDirect3DDevice9** ppReturnedDeviceInterface);
typedef HRESULT(WINAPI *TEndScene)(LPDIRECT3DDEVICE9);

LPDIRECT3D9         g_pD3D = NULL; 
LPDIRECT3DDEVICE9   g_pd3dDevice = NULL; 
D3DPRESENT_PARAMETERS   g_D3Dpp;

HWND g_hWnd; //Main window
LONG_PTR g_origWndProc; //the original wndproc (msgloop)

// original d3d methods
TCreateDevice g_origCreateDevice;
TEndScene g_origEndScene;

//Instance of our hack
NeoBar neo;

// the location of the injector.exe directory
static char pluginAppDir[2000];

// some forward declares
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
VOID Render();

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
bool InitWindowHook(HWND);

HookFunction d3dHooks[NUMBER_OF_D3D_HOOKS] = 
{
    { Direct3DCreate9, Direct3DCreate9_Hook }
    //{ Direct3DCreate9Ex, Direct3DCreate9Ex_Hook }, //see below

};

HRESULT WINAPI EndScene_Hook(LPDIRECT3DDEVICE9 device)
{
    Render();
    return g_origEndScene(device);
}

HRESULT WINAPI CreateDevice_Hook(LPDIRECT3D9 d3d, UINT Adapter,D3DDEVTYPE DeviceType,HWND hFocusWindow,DWORD BehaviorFlags,D3DPRESENT_PARAMETERS* pPresentationParameters,IDirect3DDevice9** ppReturnedDeviceInterface)
{
    
    HRESULT res = g_origCreateDevice(d3d, Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);

    if(SUCCEEDED(res))
    {
        g_pd3dDevice = *ppReturnedDeviceInterface;
        

        if(g_pD3D && g_pd3dDevice)
        {
            g_hWnd = hFocusWindow;
            InitWindowHook(hFocusWindow);

            if( neo.InitiateNeo( TW_DIRECT3D9, g_pd3dDevice ) )
            {
                RECT rect;
                if(GetClientRect(hFocusWindow, &rect))
                {
                    int cx = rect.right, cy = rect.bottom;
                    if( cx > 0 && cy > 0 )
                        TwWindowSize( cx, cy);
                }

                neo.m_CreateGUI();
            }
            else
            {
                IDirect3DDevice9_Release(g_pd3dDevice);
                g_pd3dDevice = NULL;
                IDirect3D9_Release(g_pD3D);
                g_pD3D = NULL;
                
            }

            DWORD oldProtect;
            VirtualProtect(&(PVOID&)(g_pd3dDevice->lpVtbl->EndScene), sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProtect);
            g_origEndScene = g_pd3dDevice->lpVtbl->EndScene;
            g_pd3dDevice->lpVtbl->EndScene = EndScene_Hook;
            VirtualProtect(&(PVOID&)(g_pd3dDevice->lpVtbl->EndScene), sizeof(void*), oldProtect, &oldProtect);
        }
    }

    return res;
}



// our hooked functions
LPDIRECT3D9 WINAPI Direct3DCreate9_Hook(UINT version)
{
    g_pD3D = ((TDirect3DCreate9)d3dHooks[FUNC_DIRECT_3D_CREATE_9].original)(version);


    if(g_pD3D)
    {
        DWORD oldProtect;
        VirtualProtect(&(PVOID&)(g_pD3D->lpVtbl->CreateDevice), sizeof(void*), PAGE_EXECUTE_READWRITE, &oldProtect);
        g_origCreateDevice = g_pD3D->lpVtbl->CreateDevice;
        g_pD3D->lpVtbl->CreateDevice = CreateDevice_Hook;
        VirtualProtect(&(PVOID&)(g_pD3D->lpVtbl->CreateDevice), sizeof(void*), oldProtect, &oldProtect);
    }

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
		IDirect3DDevice9_Release(g_pd3dDevice);

	if( g_pD3D != NULL )
		IDirect3D9_Release(g_pD3D);
}


VOID Render()
{
	if( NULL == g_pd3dDevice )
		return;

	//IDirect3DDevice9_Clear( g_pd3dDevice, 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB( 0, 0, 255 ), 1.0f, 0 );

	neo.DrawNeo();

	//IDirect3DDevice9_Present( g_pd3dDevice, NULL, NULL, NULL, NULL );
}



LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	if( neo.WndProcCallBack( hWnd, msg, wParam, lParam ) )
		return 0;

    RECT rect;
	switch( msg )
	{
        case WM_SIZE:
            if(GetClientRect(hWnd, &rect))
            {
                int cx = rect.right, cy = rect.bottom;
                if( cx > 0 && cy > 0 )
                    TwWindowSize( cx, cy);
            }

            break;

        case WM_WINDOWPOSCHANGED:
            if(GetClientRect(hWnd, &rect))
            {
                int cx = rect.right, cy = rect.bottom;
                if( cx > 0 && cy > 0 )
                    TwWindowSize( cx, cy);
            }

            break;
	}

	return CallWindowProc((WNDPROC)g_origWndProc, hWnd, msg, wParam, lParam);
}


bool DoHook( HINSTANCE hInst )
{
    bool success = true;

    RegKey key(HKEY_CURRENT_USER);
    key.Open(L"Software\\DxBar");
    RegValue regDir = key[L"path"];

    WideCharToMultiByte(CP_UTF8, 0, regDir, wcslen(regDir), pluginAppDir, sizeof(pluginAppDir), 0, 0);

    
	if( InitD3DHook() == false )
        success = false;

    // notify the injector we are ready
    wchar_t readyKeyName[100];
    StringCchPrintfW(readyKeyName, 100, L"ready%d", GetCurrentProcessId());
    RegValue r = key[readyKeyName];
    if(r.Type != REG_NONE)
    {
        key[readyKeyName] = DWORD(1);
        key.Flush();
    }

    key.Close();


	return success;
}

BOOL WINAPI DllMain(HINSTANCE hInst, _In_ unsigned _Reason, _In_opt_ void * _Reserved)
{
    if(_Reason == DLL_PROCESS_ATTACH)
    {
        OpenConsole();
        if(DoHook(hInst) == false)
        {
            MessageBoxA(0, "Failed to initialize hook", "Failed", 0);
        }
    }
    
    return TRUE;
}
