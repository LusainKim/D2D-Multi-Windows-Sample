//-----------------------------------------------------------------------------
// File: Scene.h
//-----------------------------------------------------------------------------

#pragma once

#include "Scene/Scene.h"
class CDirectXFramework;



class CTitleScene
	: public CScene
{
public:

	CTitleScene();
	virtual ~CTitleScene();

	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	virtual bool OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	virtual void BuildObjects(std::wstring Tag, HWND hWnd, CDirectXFramework *pMasterFramework);
	virtual void BuildObjecsFromD3D11Devices(ID3D11Device2* pd3dDevice, ID3D11DeviceContext* pd3dDeviceContext) {}
	virtual void BuildObjecsFromD2D1Devices(ID2D1Device2* pd2dDevice, ID2D1DeviceContext2* pd2dDeviceContext, IDWriteFactory3* pdwFactory, IWICImagingFactory2* pwicFactory);
	virtual void BuildContainer(ID3D11Device2* pd3dDevice, ID2D1Factory3* pd2dFactory, ComPtr<IDWriteFactory3> pdwFactory, ComPtr<IWICImagingFactory2> pwicFactory) {}
	virtual void ReleaseObjects();

	virtual void Update2D(float fTimeElapsed);
	virtual void Update3D(float fTimeElapsed);
	virtual void AnimateObjects(float fTimeElapsed);
	virtual void Render2D(ID2D1DeviceContext2 *pd2dDeviceContext);
	virtual void Render3D(ID3D11DeviceContext *pd3dDeviceContext);

protected:

	ComPtr<ID2D1SolidColorBrush>		m_d2d1hbrSample;
};

