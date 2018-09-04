//////////////////////////////////////////////////////////////////////////
// Name:	WinMain.h
// Date:	April 2nd, 2010
// Author:	Kyle Lauing [klauing@devry.edu] or [kylelauing@gmail.com]
// Purpose: This file is used to create a simple window application for
//			use with the DirectXFramework developed for the GSP 381 course
//			for DeVry University.
// Disclaimer:	
//			Copyright © 2010 by DeVry Educational Development Corporation.
//			All rights reserved.  No part of this work may be reproduced 
//			or used in any form or by any means – graphic, electronic, or 
//			mechanical, including photocopying, recording, Web distribution 
//			or information storage and retrieval systems – without the 
//			prior consent of DeVry Educational Development Corporation.
//////////////////////////////////////////////////////////////////////////

#define VC_EXTRALEAN
#include <windows.h>

#include "DirectXFramework.h"

//////////////////////////////////////////////////////////////////////////
// Global Variables
//////////////////////////////////////////////////////////////////////////
// 1440 x 900 = 1,296,000 pixels to render each frame
#define SCREEN_WIDTH 1440
#define SCREEN_HEIGHT 900
#define WINDOW_TITLE L"GSP 381 - DirectX Framework"

HWND				g_hWnd;					// Handle to the window
HINSTANCE			g_hInstance;			// Handle to the application instance
bool				g_bWindowed = true;		// Boolean for windowed or full-screen
DXGame	g_DX;					// Instance of the DirectXFramework



// Entry point for the game or application.
int WINAPI wWinMain(_In_ HINSTANCE hInstance,	// Handle to the application
				    _In_opt_ HINSTANCE hPrevInstance,	// Handle to the previous app
				    _In_ LPTSTR lpCmdLine,		// Command line string
				    _In_ int lpCmdShow);			// Show window value

LRESULT CALLBACK WndProc(HWND hWnd,			// Handle to the window
						 UINT message,		// Incoming Message
						 WPARAM wparam,		// Message Info
						 LPARAM lparam);	// Message Info

void InitWindow(void)
{
	WNDCLASSEX wndClass;  
	ZeroMemory(&wndClass, sizeof(wndClass));

	// set up the window
	wndClass.cbSize			= sizeof(WNDCLASSEX);			// size of window structure
	wndClass.lpfnWndProc	= (WNDPROC)WndProc;				// message callback
	wndClass.hInstance		= g_hInstance;					// handle to the application
	wndClass.style			= CS_HREDRAW | CS_VREDRAW;		// style
	wndClass.lpszClassName	= WINDOW_TITLE;					// class name
	wndClass.hbrBackground	= (HBRUSH)(COLOR_WINDOWFRAME);	// background brush
	wndClass.hCursor		= LoadCursor(NULL, IDC_ARROW);	// default cursor
	wndClass.hIcon			= NULL;							// default icon
	wndClass.hIconSm		= NULL;							// default small icon

	// register a new type of window
	RegisterClassEx(&wndClass);

	g_hWnd = CreateWindow(
		WINDOW_TITLE, WINDOW_TITLE, 							// window class name and title
		g_bWindowed ? WS_OVERLAPPEDWINDOW : (WS_EX_TOPMOST | WS_POPUP),// window style
		CW_USEDEFAULT, CW_USEDEFAULT,							// x and y coordinates
		g_bWindowed ? SCREEN_WIDTH : GetSystemMetrics(SM_CXSCREEN), // window width
		g_bWindowed ? SCREEN_HEIGHT : GetSystemMetrics(SM_CYSCREEN), // window height
		NULL, NULL,												// parent window and menu
		g_hInstance,											// handle to application
		NULL);

	// Display the window
	ShowWindow(g_hWnd, SW_SHOW);
	UpdateWindow(g_hWnd);
}
bool finn;
GameStates ts;
__int64 currTimeStamp;
float dt;

int WINAPI wWinMain(_In_ HINSTANCE hInstance,
					_In_opt_ HINSTANCE hPrevInstance,
					_In_ LPTSTR lpCmdLine,
					_In_ int lpCmdShow)
{

	finn=0;
	ts=GameStates::INTRO;
	//////////////////////////////////////////////////////////////////////////
	// Run-time memory leak detection for non-D3D dynamic memory allocations
	//////////////////////////////////////////////////////////////////////////
	/*#if defined(DEBUG) | defined(_DEBUG)
		_
		
		
		SetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _
		_LEAK_CHECK_DF );
	#endif*/

	g_hInstance = hInstance;	// Store application handle

	// Init the window
	InitWindow();

	// Use this msg structure to catch window messages
	MSG msg; 
	ZeroMemory(&msg, sizeof(msg));

	// Initialize DirectX/Game here
	g_DX.Init(g_hWnd, g_hInstance, g_bWindowed);

	//////////////////////////////////////////////////////////////////////////
	// Initialize timer to compute elapsed time between frames
	//////////////////////////////////////////////////////////////////////////
	__int64 cntsPerSec = 0;
	QueryPerformanceFrequency((LARGE_INTEGER*)&cntsPerSec);
	float secsPerCnt = 1.0f / (float)cntsPerSec;

	__int64 prevTimeStamp = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&prevTimeStamp);

	//////////////////////////////////////////////////////////////////////////
	// Main Windows/Game Loop
	//////////////////////////////////////////////////////////////////////////
	while ( g_DX.gamestate != GameStates::QUIT  &&
		msg.message != WM_QUIT)
	{
		switch  ( g_DX.gamestate) 
		{
			// Get the current time stamp and find the difference from the prev.
			case GameStates::LOAD:
			{
				if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			currTimeStamp = 0;
			QueryPerformanceCounter((LARGE_INTEGER*)&currTimeStamp);
			dt = (currTimeStamp - prevTimeStamp) * secsPerCnt;

		// This is where you call your DirectX/Game render/update calls
				if(!g_DX.isDeviceLost())
				{
			
						//finn=g_DX.PlayIntro(dt);
					//g_DX.RenderGUI(dt);

			// Update the prev time stamp to current
					prevTimeStamp = currTimeStamp;
	}
				//if (finn)
				//{
 
					g_DX.gamestate = GameStates::INTRO;
					
				//}
			break;
			}
			case GameStates::INTRO:
			{
				if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}

				if(!g_DX.isDeviceLost())
				{
			 //Get the current time stamp and find the difference from the prev.
					currTimeStamp = 0;
					QueryPerformanceCounter((LARGE_INTEGER*)&currTimeStamp);
					 dt = (currTimeStamp - prevTimeStamp) * secsPerCnt;
			
					ts=g_DX.UpdateGUI(dt);

			// Update the prev time stamp to current
					prevTimeStamp = currTimeStamp;
				}
				if (ts!=GameStates::INTRO)
				{
					g_DX.gamestate = ts;
				}
				break;
			}
			case GameStates::GAME:
			{
				if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}

		// This is where you call your DirectX/Game render/update calls
				if(!g_DX.isDeviceLost())
				{
			// Get /the current time stamp and find the difference from the prev.
					 currTimeStamp = 0;
					QueryPerformanceCounter((LARGE_INTEGER*)&currTimeStamp);
					 dt = (currTimeStamp - prevTimeStamp) * secsPerCnt;
					g_DX.Update(dt);
					g_DX.Render(dt);

			// Update the prev time stamp to current
					prevTimeStamp = currTimeStamp;
				}
			break;
			}

				
		}
	}

	//Shutdown DirectX/Game here
	g_DX.Shutdown();
	//_CrtDumpMemoryLeaks();

	// Unregister window
	UnregisterClass(WINDOW_TITLE, g_hInstance);

	// Return successful
	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	// attempt to handle your messages
	switch(message)
	{
		case(WM_EXITSIZEMOVE):
		{
			RECT clientRect;
			GetClientRect(hWnd, &clientRect);
			g_DX.setResolution(clientRect.right, clientRect.bottom);
			return 0;
		}
		case(WM_PAINT):
		{
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		}		
		case(WM_CLOSE):
		{
			DestroyWindow(hWnd);
			return 0;
		}
		case(WM_DESTROY):
		{
			PostQuitMessage(0); 
			break;
		}
		case(WM_KEYDOWN):
		{
			switch(wparam)
			{
				// Escape Key will close the application
				// Remove/Modify this call when creating a real game with menus, etc.
				case VK_ESCAPE:
				{
					PostQuitMessage(0);
					break;
				}
			}
		}
	}

	// pass to default handler
	return DefWindowProc(hWnd, message, wparam, lparam);
}
