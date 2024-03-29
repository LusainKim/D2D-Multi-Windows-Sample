// DXMain.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
// 대상 플랫폼 : Windows 10
// 대상 플랫폼 버전 : 10.0.10586.0 기준 작성(하위버전에서는 단계를 내려야 합니다)


#include "stdafx.h"
#include "DXMain.h"
#include "Framework\Framework.h"
#include "Framework\popupWindow.h"
#include "Framework\Timer\Timer.h"

#if USE_DEBUG_WINDOW
#include "Framework\LogSystem\LogSystem.h"
#endif

namespace
{

	int g_iMarginWidth = 0;
	int g_iMarginHeight = 0;
	HINSTANCE hInst;                                // 현재 인스턴스입니다.

	CTimer				timer;

	CDirectXFramework	framework;
	CPopupWindowBase	popupWnd;
	CPopupWindowBase	popupWnd2;
#if USE_DEBUG_WINDOW
	CLogSystem			logsys;
#endif

	unique_ptr<thread> g_thread;
	concurrent_queue<MSG> g_Msg;
	atomic_bool g_bExit = false;

};

int GetMarginWidth() { return g_iMarginWidth; }
int GetMarginHeight() { return g_iMarginHeight; }
HINSTANCE GetHInstance() { return hInst; }

void SendMessageToFramework(MSG msg) { g_Msg.push(msg); }
void DestroyMessage(bool b) { g_bExit = b; }

#define MAX_LOADSTRING 100

// 전역 변수:
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM	MyRegisterClass(HINSTANCE hInstance);
BOOL	InitInstance(HINSTANCE, int);

void ThreadFunc()
{
	while (!g_bExit)
	{
		MSG msg;
		if (g_Msg.empty())
		{
			if (!timer.Update()) continue;
			framework.FrameAdvance(timer.GetTimeElapsed());
			popupWnd.FrameAdvance(timer.GetTimeElapsed());
			popupWnd2.FrameAdvance(timer.GetTimeElapsed());
		}
		else if (g_Msg.try_pop(msg))
		{
			framework.OnProcessingWindowMessage(msg.hwnd, msg.message, msg.wParam, msg.lParam);
			popupWnd.OnProcessingWindowMessage(msg.hwnd, msg.message, msg.wParam, msg.lParam);
			popupWnd2.OnProcessingWindowMessage(msg.hwnd, msg.message, msg.wParam, msg.lParam);
		}
	}
}

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
	wsprintf(szTitle, L"Lab Project");
	wsprintf(szWindowClass, L"winMain");
    MyRegisterClass(hInstance);

    // 응용 프로그램 초기화를 수행합니다.
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    MSG msg;

    // 기본 메시지 루프입니다.
	while (GetMessage(&msg, nullptr, 0, 0))
	{
//		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	if (g_thread) g_thread->join();

    return (int) msg.wParam;
}




ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = 0
		| CS_HREDRAW 	// 클라이언트의 너비를 변경하면, 전체 윈도우를 다시 그리게 한다.
		| CS_VREDRAW	// 클라이언트의 높이를 변경하면, 전체 윈도우를 다시 그리게 한다.
	//	| CS_DBLCLKS	// 해당 윈도우에서 더블클릭을 사용해야 한다면 추가해야 한다.
		;
	wcex.lpfnWndProc = CDirectXFramework::WndProc;
//	wcex.lpfnWndProc = [](HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) -> HRESULT
//	{return myFramework.OnProcessingWindowMessage(hWnd, message, wParam, lParam);};
//	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;	//	해당 프로그램의 인스턴스 핸들.

//	MAKEINTERSOURCE : Make Inter Source. 프로그램 내부에 있는 리소스의 인덱스를 반환하는 매크로.
	wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(hInstance, IDI_APPLICATION);

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	//	메인 윈도우 핸들
	HWND hWnd;

	//	윈도우 스타일
	DWORD dwStyle = 0
		| WS_OVERLAPPED 	// 디폴트 윈도우. 타이틀 바와 크기 조절이 안되는 경계선을 가진다. 아무런 스타일도 주지 않으면 이 스타일이 적용된다.
//		| WS_CAPTION 		// 타이틀 바를 가진 윈도우를 만들며 WS_BORDER 스타일을 포함한다.
		| WS_SYSMENU		// 시스템 메뉴를 가진 윈도우를 만든다.
		| WS_MINIMIZEBOX	// 최소화 버튼을 만든다.
//		| WS_BORDER			// 단선으로 된 경계선을 만들며 크기 조정은 할 수 없다.
		| WS_THICKFRAME		// 크기 조정이 가능한 두꺼운 경계선을 가진다. WS_BORDER와 같이 사용할 수 없다.
		;					// 추가로 필요한 윈도우 스타일은 http://www.soen.kr/lecture/win32api/reference/Function/CreateWindow.htm 참고.

	//	인스턴스 핸들을 전역 변수에 저장
	hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

	//	데스크톱 윈도우 사이즈
	RECT getWinSize;
	GetWindowRect(GetDesktopWindow(), &getWinSize);

	//	클라이언트 사이즈
	RECT rc;
	rc.left = rc.top = 0;
	rc.right = CLIENT_WIDTH;
	rc.bottom = CLIENT_HEIGHT;
	//	윈도우 사이즈에 실제로 추가되는(캡션, 외곽선 등) 크기를 보정.
	AdjustWindowRect(&rc, dwStyle, FALSE);

	g_iMarginWidth = rc.right - rc.left - CLIENT_WIDTH;
	g_iMarginHeight = rc.bottom - rc.top - CLIENT_HEIGHT;

	//	클라이언트 절대좌표(left, top)
	//	데스크톱의 중앙에 클라이언트가 위치하도록 설정
	POINT ptClientWorld;
	ptClientWorld.x = (getWinSize.right - CLIENT_WIDTH) / 2;
	ptClientWorld.y = (getWinSize.bottom - CLIENT_HEIGHT) / 2;

	//	윈도우 생성
	hWnd = CreateWindow(
		  szWindowClass			//	윈도우 클래스 명
		, szTitle				//	캡션 표시 문자열
		, dwStyle				//	윈도우 스타일
		, ptClientWorld.x		//	부모 윈도우 기반 윈도우 시작좌표 : x
		, ptClientWorld.y		//	부모 윈도우 기반 윈도우 시작좌표 : y
		, rc.right - rc.left	//	윈도우 사이즈 : width
		, rc.bottom - rc.top	//	윈도우 사이즈 : height
		, NULL					//	부모 윈도우.
		, NULL					//	메뉴 핸들
		, hInstance				//	인스턴스 핸들
		, NULL					//	추가 파라메터 : NULL
		);

	//	생성 실패시 프로그램 종료
	//	확인 : WndProc의 default msg handler가 DefWindowProc 함수를 반환하는가?
	if (!hWnd)
	{
		LPVOID lpMsgBuf;
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL, GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR) &lpMsgBuf, 0, NULL);
		MessageBox(NULL, (LPCTSTR) lpMsgBuf, L"Create Window Fail", MB_ICONERROR);
		LocalFree(lpMsgBuf);
		return FALSE;
	}

#if USE_DEBUG_WINDOW
	if (!logsys.Initialize(hWnd, hInst)) return FALSE;
#endif
	
	if (!framework.Initialize(hInst, hWnd)) return FALSE;
	if (!popupWnd.Initialize(hWnd, hInst)) return FALSE;
	if (!popupWnd2.Initialize(hWnd, hInst)) return FALSE;
	
	ComPtr<ID2D1HwndRenderTarget> hrt;
	framework.CreateHwndRenderTarget(popupWnd.GetHwnd(), hrt);
	popupWnd.SetRenderTarget(move(hrt));
	
	ComPtr<ID2D1Multithread> mul;
	framework.CreateAsD2D1Factory(mul);
	popupWnd.CreateMultithread(move(mul));

	ComPtr<ID2D1HwndRenderTarget> hrt2;
	framework.CreateHwndRenderTarget(popupWnd2.GetHwnd(), hrt2);
	popupWnd2.SetRenderTarget(move(hrt2));

	ComPtr<ID2D1Multithread> mul2;
	framework.CreateAsD2D1Factory(mul2);
	popupWnd2.CreateMultithread(move(mul2));

//	popupWnd.SetTimer(framework.GetTimer());

	g_thread = make_unique<thread>(ThreadFunc);

	//	윈도우 표시
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	//	성공 반환
	return TRUE;
}

