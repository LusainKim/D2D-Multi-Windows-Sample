#pragma once

class CSceneDirect2D;



class CPopupWindowBase
{

public:

	CPopupWindowBase();
	virtual ~CPopupWindowBase();

	bool Initialize(HWND hParentWnd, HINSTANCE hInstance);

	void FrameAdvance(float fTimeElapsed);


	HWND GetHwnd() const { return m_hWnd; }
	void SetRenderTarget(ComPtr<ID2D1HwndRenderTarget>&& rt) { m_hwndRenderTarget = move(rt); }

	void Update(float fTimeELapsed);
	void Render();

protected:

	ATOM MyRegisterClass(HINSTANCE hInstance);
	bool CreatePopupWindow(HWND hParentWnd, HINSTANCE hInstance);

	HWND			m_hWnd{ nullptr };
	LPCTSTR			m_szWindowClass{ TEXT("popupBase") };
	LPCTSTR			m_szTitle{ TEXT("") };

	POINT			m_ptStartPosition{ 20,20 };

	RECT			m_rcClient;

	HFONT			m_hFont{ NULL };

protected:

	list<unique_ptr<CSceneDirect2D>>				m_lstScenes;
	CSceneDirect2D								*	m_pCurrentScene{ nullptr };
	ComPtr<ID2D1HwndRenderTarget>					m_hwndRenderTarget;

public:

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	static void CALLBACK TimerProc(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime);

};