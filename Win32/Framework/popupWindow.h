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

	// 후 처리가 없을 경우 true 반환
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	// 후 처리가 없을 경우 true 반환
	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	// 후 처리가 없을 경우 true 반환
	virtual LRESULT OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	virtual void CreateMultithread(ComPtr<ID2D1Multithread>&& mul) { m_pd2dMultithread = mul; }

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
	ComPtr<ID2D1Multithread>						m_pd2dMultithread { nullptr };

public:

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

};