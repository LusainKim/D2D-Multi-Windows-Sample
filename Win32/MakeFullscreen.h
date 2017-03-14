#pragma once

#include <Windows.h>

class CFullScreen
{

public:

	void SetFullscreen_WindowMode(HWND hWnd, RECT& rcClient, DWORD DeleteStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZE | WS_THICKFRAME)
	{
		if (m_bClientSetFullScreenMode)
		{
			SetWindowLong(hWnd, GWL_STYLE, m_dwWindowStyleRememberLast);
			SetWindowLong(hWnd, GWL_EXSTYLE, m_dwWindowStyleExRememberLast);

			AdjustWindowRectEx(	  &m_rcClientRememberLast
								, m_dwWindowStyleRememberLast
								, GetMenu(hWnd) != NULL
								, m_dwWindowStyleExRememberLast
			);
		
			SetWindowPos(	  hWnd, NULL
							, m_ptClientPosRememberLast.x, m_ptClientPosRememberLast.y
							, m_rcClientRememberLast.right - m_rcClientRememberLast.left
							, m_rcClientRememberLast.bottom - m_rcClientRememberLast.top
							, SWP_NOZORDER
			);

			m_bClientSetFullScreenMode = false;
			return;
		}
	


		RECT rcWindow;
		RECT rcStartPos;
	
		::GetClientRect(GetDesktopWindow(), &rcWindow);
		::GetWindowRect(hWnd, &rcStartPos);
	
		m_rcClientRememberLast = rcClient;
		m_ptClientPosRememberLast = POINT { rcStartPos.left, rcStartPos.top };

		RECT crt;
		DWORD Style, ExStyle;

		int width = rcWindow.right - rcWindow.left;
		int height = rcWindow.bottom - rcWindow.top;

		SetRect(&crt, 0, 0, width, height);

		Style = GetWindowLong(hWnd, GWL_STYLE);
		ExStyle = GetWindowLong(hWnd, GWL_EXSTYLE);
		m_dwWindowStyleRememberLast = Style;
		m_dwWindowStyleExRememberLast = ExStyle;
	
		ExStyle &= ~DeleteStyle;
		Style &= ~DeleteStyle;
		SetWindowLong(hWnd, GWL_STYLE, Style);
		SetWindowLong(hWnd, GWL_EXSTYLE, ExStyle);

		AdjustWindowRectEx(&crt, Style, GetMenu(hWnd) != NULL, ExStyle);
	
		if (Style & WS_VSCROLL) crt.right += GetSystemMetrics(SM_CXVSCROLL);
		if (Style & WS_HSCROLL) crt.bottom += GetSystemMetrics(SM_CYVSCROLL);

		SetWindowPos(	hWnd, NULL, 0, 0, crt.right - crt.left, crt.bottom - crt.top,
						SWP_NOZORDER);


		m_bClientSetFullScreenMode = true;
	}

private:

	// for fullscreen(window mode)
	RECT							m_rcClientRememberLast					;
	POINT							m_ptClientPosRememberLast				;
	bool							m_bClientSetFullScreenMode	{ false }	;
	DWORD							m_dwWindowStyleRememberLast				;
	DWORD							m_dwWindowStyleExRememberLast			;

};
