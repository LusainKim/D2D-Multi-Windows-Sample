#pragma once

#include <windows.h>
#include <d2d1_1.h>
#include <wrl\client.h>



inline Microsoft::WRL::ComPtr<ID2D1HwndRenderTarget> CreateHwndRenderTarget(
	  ID2D1Factory* pd2dFactory
	, HWND hWnd
	, D2D1_RENDER_TARGET_PROPERTIES prop = D2D1::RenderTargetProperties()
	, D2D1_PRESENT_OPTIONS presentOpt = D2D1_PRESENT_OPTIONS_IMMEDIATELY)
{
	Microsoft::WRL::ComPtr<ID2D1HwndRenderTarget> phwndRenderTarget{ nullptr };

	RECT rc{};
	::GetClientRect(hWnd, &rc);

	D2D1_SIZE_U size
	{
		  static_cast<UINT32>(rc.right - rc.left)
		, static_cast<UINT32>(rc.bottom - rc.top)
	};

	pd2dFactory->CreateHwndRenderTarget
	(
		  prop
		, D2D1::HwndRenderTargetProperties(hWnd, size, presentOpt)
		, &phwndRenderTarget
	);
	return phwndRenderTarget;
}