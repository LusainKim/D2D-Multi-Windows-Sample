#pragma once

class CCamera;
class CScene;
class CServerFramework;

#include "MakeFullscreen.h"

///	<remarks> 
///	Create 함수는 객체의 할당 여부를 검사하지 않습니다. 할당될 객체들은 모두 해제되는 등 할당되지 않은 상태여야 합니다.
/// 모든 할당되지 않은 객체는 nullptr를 유지해야 합니다.
///	</remarks>
class CDirectXFramework {

private:

	ComPtr<ID3D11Device2>			m_pd3dDevice				{ nullptr }	;
	ComPtr<IDXGISwapChain2>			m_pdxgiSwapChain			{ nullptr }	;

	ComPtr<ID3D11RenderTargetView>	m_pd3dRenderTargetView		{ nullptr }	;
	ComPtr<ID3D11DeviceContext2>	m_pd3dDeviceContext			{ nullptr }	;

	HINSTANCE						m_hInstance					{ NULL }	;
	HWND							m_hWnd						{ NULL }	;
	RECT							m_rcClient;

	CFullScreen						m_FullScreen;

	// Rendering 추가 부분 : Depth-Stencil 을 하지 않으면 정상적인 큐브가 출력되지 않는다.
	ComPtr<ID3D11Texture2D>			m_pd3dDepthStencilBuffer	{ nullptr }	;
	ComPtr<ID3D11DepthStencilView>	m_pd3dDepthStencilView		{ nullptr }	;

	DXGI_FORMAT						m_dxgiFormatCurrentSelect { DXGI_FORMAT_R16G16B16A16_FLOAT };

public:

	CDirectXFramework();

	~CDirectXFramework();

	bool Initialize(HINSTANCE hInstance, HWND hWnd);

	void CreateChildWindow();

	void ReleaseObjects();

	bool CreateD3D11Deivce();

	bool CreateRenderTargetDepthStencilView();

	void ReleaseD3D11Devices();



	void Render();

	void Update(float fTimeElapsed);



	void SetMainRenderTargetView();

	template<UINT N>
	void SetRenderTargetViews(ID3D11RenderTargetView* const (&pd3dRTVs)[N]) { m_pd3dDeviceContext->OMSetRenderTargets(N, pd3dRTVs, m_pd3dDepthStencilView.Get()); }

	void ClearDepthStencilView() { m_pd3dDeviceContext->ClearDepthStencilView(m_pd3dDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0); }

	void ClearRenderTargetView() { m_pd3dDeviceContext->ClearRenderTargetView(m_pd3dRenderTargetView.Get(), FLOAT4 { 0.8f, 0.8f, 0.8f, 1.0f }); }


	// Getter Method입니다.
public:

	auto GetD3D11Device() const { return m_pd3dDevice.Get(); }
	void SetCameraViewport(CCamera* pCamera);
	RECT GetClientRect() const { return m_rcClient; }

	DXGI_FORMAT GetDXGIFormat() const { return m_dxgiFormatCurrentSelect; }

	auto GetD2D1Factory() const { return m_pd2dFactory.Get(); }
	template<typename InterfaceType>
	HRESULT CreateAsD2D1Factory(ComPtr<InterfaceType>& in) const { return m_pd2dFactory.As(&in); }
	auto GetDWriteFactory() const { return m_pdwFactory.Get(); }
	auto GetDWICImagingFactory() const { return m_wicFactory.Get(); }

	// 2D code
private:

	ComPtr<ID2D1Device2>				m_pd2dDevice		{ nullptr }	;
	ComPtr<ID2D1Factory3>				m_pd2dFactory		{ nullptr }	;
	ComPtr<ID2D1DeviceContext2>			m_pd2dDeviceContext	{ nullptr }	;
	ComPtr<IDWriteFactory3>				m_pdwFactory		{ nullptr }	;
	ComPtr<IWICImagingFactory2>			m_wicFactory		{ nullptr }	;
	
	// 3D SwapChain에서 RenderTarget을 얻기 위한 BackBuffer 입니다.
	ComPtr<ID2D1Bitmap1>				m_pd2dBmpBackBuffer { nullptr }	;
	
	// 이전까지의 Drawing 상태를 저장합니다.
	ComPtr<ID2D1DrawingStateBlock1>		m_pd2dStateBlock	{ nullptr }	;

	ComPtr<ID2D1Multithread>			m_pd2dMultithread	{ nullptr }	;

	void ReleaseD2DResources();

public:

	bool CreateD2D1Device(IDXGIDevice3* pdxgiDevice);

	void CreateHwndRenderTarget(HWND hWnd, ComPtr<ID2D1HwndRenderTarget>& phwndRenderTarget);

	bool CreateD2DBackBuffer();

	void Render2D();



// 게임 전체의 로직을 담당합니다.
private:

	TCHAR		m_CaptionTitle[TITLE_MAX_LENGTH];
	int			m_TitleLength;

	using FLOAT4 = float[4];

	std::list<std::unique_ptr<CScene>>		m_lstScenes;
	CScene								*	m_pCurrentScene	{ nullptr }	;

public:

	// 후 처리가 없을 경우 true 반환
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	// 후 처리가 없을 경우 true 반환
	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	// 후 처리가 없을 경우 true 반환
	virtual LRESULT OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void BuildScene();
	void FrameAdvance(float fTimeElapsed);

	// Framework 에서 호출하는 윈도우 프로시저입니다.
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	// 새로운 윈도우입니다.
private:
	HWND								m_hWndChild			{ NULL }	;
	ComPtr<ID2D1HwndRenderTarget>		m_hWndRenderTarget	{ nullptr }	;

public:
	static LRESULT CALLBACK ChildProc(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
};
