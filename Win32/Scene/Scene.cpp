//-----------------------------------------------------------------------------
// File: Scene.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
#include "Framework\Framework.h"
#include "Framework\popupWindow.h"
#include "Scene.h"



CSceneBase::~CSceneBase()
{
	ReleaseObjects();
}

bool CSceneBase::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
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

bool CSceneBase::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_SIZE:
		break;
	default:
		return true;
	}

	return true;
}

bool CSceneBase::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		break;
	default:
		break;
	}
	return(true);
}

bool CSceneBase::FindByTag(std::wstring Tag)
{
	return Tag == m_wsTag;
}

void CScene::BuildObjects(wstring Tag, HWND hWnd, CDirectXFramework *pMasterFramework)
{
	m_wsTag = Tag;
	m_hWnd = hWnd;
	m_pMasterFramework = pMasterFramework;
}

void CSceneDirect2D::BuildObjects(std::wstring Tag, HWND hWnd, CPopupWindowBase * pMasterWindow)
{
	m_wsTag = Tag;
	m_hWnd = hWnd;
	m_pMasterWindow = pMasterWindow;
}
