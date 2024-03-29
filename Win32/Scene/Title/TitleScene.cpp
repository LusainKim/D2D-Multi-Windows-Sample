//-----------------------------------------------------------------------------
// File: Scene.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Framework\Framework.h"
#include "TitleScene.h"

CTitleScene::CTitleScene()
{

}

CTitleScene::~CTitleScene()
{
	ReleaseObjects();
}

bool CTitleScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
		break;
	case WM_RBUTTONDOWN:
		break;
	case WM_MOUSEMOVE:
		break;
	case WM_LBUTTONUP:
		break;
	case WM_RBUTTONUP:
		break;
	default:
		break;
	}

	return(true);
}

bool CTitleScene::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_SIZE:
		break;


	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MOUSEMOVE:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;

	case WM_KEYDOWN:
	case WM_KEYUP:
		return OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		break;

	default:
		return true;
	}

	return false;
}

bool CTitleScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch(wParam)
		{

		default:
			return(true);
		}
		break;
	default:
		;
	}
	return(false);
}

void CTitleScene::BuildObjects(wstring Tag, HWND hWnd, CDirectXFramework *pMasterFramework)
{
	CScene::BuildObjects(Tag, hWnd, pMasterFramework);

}

void CTitleScene::BuildObjecsFromD2D1Devices(ID2D1Device2 * pd2dDevice, ID2D1DeviceContext2 * pd2dDeviceContext, IDWriteFactory3 * pdwFactory, IWICImagingFactory2* pwicFactory)
{
	pd2dDeviceContext->CreateSolidColorBrush(ColorF{ ColorF::AliceBlue }, &m_d2d1hbrSample);
}

void CTitleScene::ReleaseObjects()
{
}

void CTitleScene::Update2D(float fTimeElapsed)
{
	m_fTick += fTimeElapsed;
}

void CTitleScene::Update3D(float fTimeElapsed)
{
}

void CTitleScene::AnimateObjects(float fTimeElapsed)
{
	Update2D(fTimeElapsed);
	Update3D(fTimeElapsed);
}

void CTitleScene::Render2D(ID2D1DeviceContext2 * pd2dDeviceContext)
{
	float angle = XMConvertToRadians(m_fTick * 270.f);
	float x = 350 + cos(angle) * 25.f;
	float y = 250 + sin(angle) * 25.f;
	pd2dDeviceContext->FillRectangle(RectF(x - 40.f, y - 40.f, x + 40.f, y + 40.f), m_d2d1hbrSample.Get());
}

void CTitleScene::Render3D(ID3D11DeviceContext *pd3dDeviceContext)
{

}
