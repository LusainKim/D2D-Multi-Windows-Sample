#include "stdafx.h"
#include "Scene\Edit\EditScene.h"
#include "popupWindow.h"


CPopupWindowBase::CPopupWindowBase()
{
}

CPopupWindowBase::~CPopupWindowBase()
{
	if (m_hWnd) DestroyWindow(m_hWnd);
	if (m_hFont) DeleteObject(m_hFont);
}

bool CPopupWindowBase::Initialize(HWND hParentWnd, HINSTANCE hInstance)
{
	MyRegisterClass(hInstance);
	if(!CreatePopupWindow(hParentWnd, hInstance)) return false;

	// 클래스와 윈도우 프로시저 연결
	::SetUserDataPtr(m_hWnd, this);

	//	윈도우 표시
	ShowWindow(m_hWnd, SW_SHOW);
	UpdateWindow(m_hWnd);

	auto scn = make_unique<CEditScene>();
	CreateScene<CEditScene>(move(scn), L"Edit", true);
	return true;
}

void CPopupWindowBase::FrameAdvance(float fTimeElapsed)
{
	Update(fTimeElapsed);
	Render();
}

void CPopupWindowBase::Update(float fTimeELapsed)
{
	if (m_pCurrentScene) m_pCurrentScene->AnimateObjects(fTimeELapsed);
}

void CPopupWindowBase::Render()
{
	m_hwndRenderTarget->BeginDraw();
	m_hwndRenderTarget->Clear(ColorF{ ColorF::RosyBrown });
	if (m_pCurrentScene) m_pCurrentScene->Render2D(m_hwndRenderTarget.Get());
	m_hwndRenderTarget->EndDraw();
}

ATOM CPopupWindowBase::MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style =
		  CS_HREDRAW 	// 클라이언트의 너비를 변경하면, 전체 윈도우를 다시 그리게 한다.
		| CS_VREDRAW	// 클라이언트의 높이를 변경하면, 전체 윈도우를 다시 그리게 한다.
		;
	wcex.lpfnWndProc = CPopupWindowBase::WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;	//	해당 프로그램의 인스턴스 핸들.

//	MAKEINTERSOURCE : Make Inter Source. 프로그램 내부에 있는 리소스의 인덱스를 반환하는 매크로.
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = m_szWindowClass;
	wcex.hIconSm = NULL;

	return RegisterClassEx(&wcex);
}

bool CPopupWindowBase::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
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

	return(false);
}

LRESULT CPopupWindowBase::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	if (m_hWnd != hWnd) return 0;
	// 선처리 구문 : 개별 Scene에서 처리할 것인지 확인
	switch (nMessageID)
	{
	case WM_SIZE:
		{
			auto m_nWndClientWidth = static_cast<LONG>(LOWORD(lParam));
			auto m_nWndClientHeight = static_cast<LONG>(HIWORD(lParam));

			if (m_nWndClientWidth < CLIENT_MINIMUM_WIDTH - GetMarginWidth())
				m_nWndClientWidth = CLIENT_MINIMUM_WIDTH - GetMarginWidth();
				
			if (m_nWndClientHeight < CLIENT_MINIMUM_HEIGHT - GetMarginHeight())
				m_nWndClientHeight = CLIENT_MINIMUM_HEIGHT - GetMarginHeight();



			m_rcClient = RECT { 0, 0, m_nWndClientWidth, m_nWndClientHeight };

			m_pd2dMultithread->Enter();
			{
				Render();
			}
			m_pd2dMultithread->Leave();

			if (m_pCurrentScene)
				if (m_pCurrentScene->OnProcessingWindowMessage(hWnd, nMessageID, wParam,lParam)) break;
				else return 0;
		}
		break;
	default:
		if (m_pCurrentScene)	if (m_pCurrentScene->OnProcessingWindowMessage(hWnd, nMessageID, wParam, lParam)) break;
								else return 0;
	}

	// 후처리 구문 : Scene에서 처리되지 않고 남는 부분을 처리
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MOUSEMOVE:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEWHEEL:
		OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		return 0;
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_CHAR:
		OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		return 0;

	default:
		break;
	}

	return 0;
}

bool CPopupWindowBase::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_F1:
			break;
		}
		break;
	default:
		break;
	}
	return(false);
}

bool CPopupWindowBase::CreatePopupWindow(HWND hParentWnd, HINSTANCE hInstance)
{
	//	윈도우 스타일
	DWORD dwStyle =
		  WS_OVERLAPPED 	// 디폴트 윈도우. 타이틀 바와 크기 조절이 안되는 경계선을 가진다. 아무런 스타일도 주지 않으면 이 스타일이 적용된다.
		| WS_POPUP
		| WS_CAPTION 		// 타이틀 바를 가진 윈도우를 만들며 WS_BORDER 스타일을 포함한다.
		| WS_BORDER			// 단선으로 된 경계선을 만들며 크기 조정은 할 수 없다.
		;					// 추가로 필요한 윈도우 스타일은 http://www.soen.kr/lecture/win32api/reference/Function/CreateWindow.htm 참고.

	//	데스크톱 윈도우 사이즈
	RECT rcParentWindow;
	GetWindowRect(hParentWnd, &rcParentWindow);

	//	클라이언트 사이즈
	m_rcClient.left = m_rcClient.top = 0;
	m_rcClient.right = DEBUG_CLIENT_WIDTH;
	m_rcClient.bottom = DEBUG_CLIENT_HEIGHT;
	//	윈도우 사이즈에 실제로 추가되는(캡션, 외곽선 등) 크기를 보정.
	AdjustWindowRect(&m_rcClient, dwStyle, FALSE);
	
	m_rcClient.right -= m_rcClient.left;
	m_rcClient.bottom -= m_rcClient.top;
	m_rcClient.left = m_rcClient.top = 0;

	//	클라이언트 절대좌표(left, top)
	//	부모 클라이언트의 좌상단에 위치하도록 설정
	POINT ptClientWorld;
	ptClientWorld.x = rcParentWindow.right + m_ptStartPosition.x;
	ptClientWorld.y = rcParentWindow.top + m_ptStartPosition.y;

	//	윈도우 생성
	m_hWnd = CreateWindow(
		  m_szWindowClass		//	윈도우 클래스 명
		, m_szTitle				//	캡션 표시 문자열
		, dwStyle				//	윈도우 스타일
		, ptClientWorld.x		//	부모 윈도우 기반 윈도우 시작좌표 : x
		, ptClientWorld.y		//	부모 윈도우 기반 윈도우 시작좌표 : y
		, m_rcClient.right		//	윈도우 사이즈 : width
		, m_rcClient.bottom		//	윈도우 사이즈 : height
		, hParentWnd			//	부모 윈도우.
		, NULL					//	메뉴 핸들
		, hInstance				//	인스턴스 핸들
		, NULL					//	추가 파라메터 : NULL
	);

	if (!m_hWnd)
	{
		LPVOID lpMsgBuf;
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL, GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR) &lpMsgBuf, 0, NULL);
		MessageBox(NULL, (LPCTSTR) lpMsgBuf, L"Create Debug Window Fail", MB_ICONERROR);
		LocalFree(lpMsgBuf);
		return false;
	}

	return true;
}

LRESULT CPopupWindowBase::WndProc(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
//	CPopupWindowBase* self = ::GetUserDataPtr<CPopupWindowBase*>(hWnd);
//
//	if (!self)
//		return ::DefWindowProc(hWnd, nMessageID, wParam, lParam);

	switch (nMessageID)
	{

	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MOUSEMOVE:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEWHEEL:

	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_CHAR:

	case WM_SIZE:

		::SendMessageToFramework(hWnd, nMessageID, wParam, lParam);

		break;

	default:
		return DefWindowProc(hWnd, nMessageID, wParam, lParam);
	}
	
	return 0;
}

