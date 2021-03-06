//-----------------------------------------------------------------------------
// File: Scene.h
//-----------------------------------------------------------------------------

#pragma once

class CDirectXFramework;
class CPopupWindowBase;

// Deferred Rendering 에 필요한 객체입니다.
struct DeferredRenderObject
{
	ComPtr<ID3D11Texture2D>				texture;
	ComPtr<ID3D11ShaderResourceView>	srv;
	ComPtr<ID3D11RenderTargetView>		rtv;

	void BuildObject(	  ID3D11Device								*pd3dDevice
						, const D3D11_SHADER_RESOURCE_VIEW_DESC		*DescSRV
						, const D3D11_TEXTURE2D_DESC				*Desc2D
						, const D3D11_RENDER_TARGET_VIEW_DESC		*DescRTV
	)
	{
		pd3dDevice->CreateTexture2D(Desc2D, nullptr, &texture);
		pd3dDevice->CreateShaderResourceView(texture.Get(), DescSRV, &srv);
		pd3dDevice->CreateRenderTargetView(texture.Get(), DescRTV, &rtv);
	}

	operator ID3D11RenderTargetView*() { return rtv.Get(); }
	operator ID3D11ShaderResourceView*() { return srv.Get(); }
	DeferredRenderObject& operator=(decltype(__nullptr)) noexcept
	{
		texture = nullptr; srv = nullptr; rtv = nullptr;
		return *this;
	}
};

struct GBufferObject
{
	ComPtr<ID3D11Texture2D>				texture;
	ComPtr<ID3D11ShaderResourceView>	srv;
	ComPtr<ID3D11UnorderedAccessView>	uav;

	void BuildObject(	  ID3D11Device								*pd3dDevice
						, const D3D11_SHADER_RESOURCE_VIEW_DESC		*DescSRV
						, const D3D11_TEXTURE2D_DESC				*Desc2D
						, const D3D11_UNORDERED_ACCESS_VIEW_DESC	*DescUOV
	)
	{
		pd3dDevice->CreateTexture2D(Desc2D, nullptr, &texture);
		pd3dDevice->CreateShaderResourceView(texture.Get(), DescSRV, &srv);
		pd3dDevice->CreateUnorderedAccessView(texture.Get(), DescUOV, &uav);
	}

	operator ID3D11UnorderedAccessView*() { return uav.Get(); }
	operator ID3D11ShaderResourceView*() { return srv.Get(); }
	GBufferObject& operator=(decltype(__nullptr)) noexcept
	{
		texture = nullptr; srv = nullptr; uav = nullptr;
		return *this;
	}
};



class CSceneBase
{
public:

	CSceneBase() = default;
	virtual ~CSceneBase();

	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	virtual void ReleaseObjects() {}

	virtual void AnimateObjects(float fTimeElapsed) {}

	bool FindByTag(std::wstring Tag);
	std::wstring GetTag() { return m_wsTag; }

protected:
	std::wstring						m_wsTag				;
	HWND								m_hWnd				;
};

class CScene
	: public CSceneBase
{
public:

	CScene() = default;
	virtual ~CScene() = default;

	virtual void BuildObjects(std::wstring Tag, HWND hWnd, CDirectXFramework *pMasterFramework);
	virtual void BuildObjecsFromD3D11Devices(ID3D11Device2* pd3dDevice, ID3D11DeviceContext* pd3dDeviceContext) {}
	virtual void BuildObjecsFromD2D1Devices(ID2D1Device2* pd2dDevice, ID2D1DeviceContext2* pd2dDeviceContext, IDWriteFactory3* pdwFactory, IWICImagingFactory2* pwicFactory) {}
	virtual void BuildContainer(ID3D11Device2* pd3dDevice, ID2D1Factory3* pd2dFactory, ComPtr<IDWriteFactory3> pdwFactory, ComPtr<IWICImagingFactory2> pwicFactory) {}

	virtual void Update2D(float fTimeElapsed) {}
	virtual void Update3D(float fTimeElapsed) {}
	virtual void Render2D(ID2D1DeviceContext2 *pd2dDeviceContext) {}
	virtual void Render3D(ID3D11DeviceContext *pd3dDeviceContext) {}

protected:
	CDirectXFramework				*	m_pMasterFramework	;

};

class CSceneDirect2D
	: public CSceneBase
{
public:

	CSceneDirect2D() = default;
	virtual ~CSceneDirect2D() = default;

	virtual void BuildObjects(std::wstring Tag, HWND hWnd, CPopupWindowBase *pMasterWindow);
	virtual void BuildObjecsFromD2D1Devices(ID2D1Device2* pd2dDevice, ID2D1DeviceContext2* pd2dDeviceContext, IDWriteFactory3* pdwFactory, IWICImagingFactory2* pwicFactory) {}

	virtual void Update2D(float fTimeElapsed) {}
	virtual void Render2D(ID2D1HwndRenderTarget*pd2dDeviceContext) {}
	
protected:
	CPopupWindowBase				*	m_pMasterWindow	;

};

