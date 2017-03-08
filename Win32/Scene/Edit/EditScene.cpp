#include "stdafx.h"
#include "Scene\Scene.h"
#include "EditScene.h"

bool CEditScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return false;
}

bool CEditScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return false;
}

bool CEditScene::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return false;
}

void CEditScene::BuildObjects(std::wstring Tag, HWND hWnd, CPopupWindowBase * pMasterFramework)
{
	CSceneDirect2D::BuildObjects(Tag, hWnd, pMasterFramework);
}

void CEditScene::ReleaseObjects()
{
}

void CEditScene::AnimateObjects(float fTimeElapsed)
{
	Update2D(fTimeElapsed);
}

void CEditScene::BuildObjecsFromD2D1Devices(ID2D1Device2 * pd2dDevice, ID2D1DeviceContext2 * pd2dDeviceContext, IDWriteFactory3 * pdwFactory, IWICImagingFactory2 * pwicFactory)
{
}

void CEditScene::Update2D(float fTimeElapsed)
{
	m_fTick += fTimeElapsed;
}

void CEditScene::Render2D(ID2D1HwndRenderTarget * pd2dDeviceContext)
{
	ComPtr<ID2D1SolidColorBrush> hbr;
	pd2dDeviceContext->CreateSolidColorBrush(ColorF{ ColorF::AliceBlue }, &hbr);

	float angle = XMConvertToRadians(m_fTick * 270.f);
	float x = 50 + cos(angle) * 25.f;
	float y = 50 + sin(angle) * 25.f;
	pd2dDeviceContext->FillRectangle(RectF(x - 10.f, y - 10.f, x + 10.f, y + 10.f), hbr.Get());
}
