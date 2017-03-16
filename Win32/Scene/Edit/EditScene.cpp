#include "stdafx.h"
#include "Scene\Scene.h"
#include "EditScene.h"

bool CEditScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return false;
}

bool CEditScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{

	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
		m_pt.push_back({ LOWORD(lParam),HIWORD(lParam) });
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
		return false;
	}

	return true;
}

bool CEditScene::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MOUSEMOVE:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEWHEEL:
		OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_CHAR:
		OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		break;

	default:
		return false;
	}
	return true;
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

	for (auto p : m_pt)
	{
		float angle = XMConvertToRadians(m_fTick * 270.f);
		float x = p.x + cos(angle) * 25.f;
		float y = p.y + sin(angle) * 25.f;
		pd2dDeviceContext->FillRectangle(RectF(x - 10.f, y - 10.f, x + 10.f, y + 10.f), hbr.Get());
	}
}
