#include <d3d9.h>
#pragma warning( disable : 4996 ) // disable deprecated warning 
#include <strsafe.h>
#pragma warning( default : 4995 ) //deprecation warnings
#include "NeoBar.h"
#include <Windows.h>
#include <AntTweakBar.h>

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
LPDIRECT3D9         g_pD3D = NULL; // Used to create the D3DDevice
LPDIRECT3DDEVICE9   g_pd3dDevice = NULL; // Our rendering device
D3DPRESENT_PARAMETERS   g_D3Dpp;
HWND hwnd; //Main window

//Instance of our hack
NeoBar *neo = new NeoBar();




HRESULT InitD3D()
{
	// Create the D3D object, which is needed to create the D3DDevice.
	if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
		return E_FAIL;

	// Set up the structure used to create the D3DDevice. Most parameters are
	// zeroed out. We set Windowed to TRUE, since we want to do D3D in a
	// window, and then set the SwapEffect to "discard", which is the most
	// efficient method of presenting the back buffer to the display.  And 
	// we request a back buffer format that matches the current desktop display 
	// format.
	//D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory( &g_D3Dpp, sizeof( g_D3Dpp ) );
	g_D3Dpp.Windowed = TRUE;
	g_D3Dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	g_D3Dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	g_D3Dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	g_D3Dpp.AutoDepthStencilFormat = D3DFMT_D16;
	g_D3Dpp.hDeviceWindow = hwnd;
	g_D3Dpp.EnableAutoDepthStencil = TRUE;

	// Create the Direct3D device. Here we are using the default adapter (most
	// systems only have one, unless they have multiple graphics hardware cards
	// installed) and requesting the HAL (which is saying we want the hardware
	// device rather than a software one). Software vertex processing is 
	// specified since we know it will work on all cards. On cards that support 
	// hardware vertex processing, though, we would see a big performance gain 
	// by specifying hardware vertex processing.
	if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&g_D3Dpp, &g_pd3dDevice ) ) )
	{
		return E_FAIL;
	}

	// Device state would normally be set here

	return S_OK;
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

	// Clear the backbuffer to a blue color
	g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB( 0, 0, 255 ), 1.0f, 0 );

	// Begin the scene
	if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
	{
		// Rendering of scene objects can happen here
		neo->DrawNeo();

		// End the scene
		g_pd3dDevice->EndScene();
	}

	// Present the backbuffer contents to the display
	g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}



//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: The window's message handler
//-----------------------------------------------------------------------------
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	if( neo->WndProcCallBack( hWnd, msg, wParam, lParam ) )
		return 0;

	switch( msg )
	{
	case WM_DESTROY:
		Cleanup();
		PostQuitMessage( 0 );
		break;

	case WM_PAINT:
		Render();
		ValidateRect( hWnd, NULL );
		break;

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

	return DefWindowProc( hWnd, msg, wParam, lParam );
}


INT WINAPI wWinMain( HINSTANCE hInst, HINSTANCE, LPWSTR, INT )
{

	// Register the window class
	WNDCLASSEX wc =
	{
		sizeof( WNDCLASSEX ), CS_CLASSDC, MsgProc, 0L, 0L,
		GetModuleHandle( NULL ), NULL, NULL, NULL, NULL,
		L"NEO_TEST", NULL
	};
	RegisterClassEx( &wc );

	// Create the application's window
	hwnd = CreateWindow( L"NEO_TEST", L"NEOHAX",
		WS_OVERLAPPEDWINDOW, 100, 100, 400, 400,
		NULL, NULL, wc.hInstance, NULL );


	if( SUCCEEDED(InitD3D() ) )
	{

		if( !neo->InitiateNeo( TW_DIRECT3D9, g_pd3dDevice ) )
		{
			g_pd3dDevice->Release();
			g_pd3dDevice = NULL;
			g_pD3D->Release();
			g_pD3D = NULL;
			return FALSE;
		}

		neo->m_CreateGUI();
		// Show the window
		ShowWindow( hwnd, SW_SHOWDEFAULT );
		UpdateWindow( hwnd );
	}

	bool quit = false;

	while( !quit)
	{
		Render();
		MSG msg;
		while( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) )
		{
			if( msg.message==WM_QUIT )
				quit = true;
			else if( !TranslateAccelerator(msg.hwnd, NULL, &msg) ) 
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

	}
	delete neo;

	UnregisterClass(L"NEO_TEST", hInst );

	return 0;
}