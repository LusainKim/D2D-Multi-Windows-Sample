﻿#include "stdafx.h"

#if USE_DEBUG_WINDOW

#include "LogSystem.h"

bool CLogSystem::m_bCreated = false;
map<Event::EType, CObserver*> CLogSystem::m_mapObservers;

CLogSystem::~CLogSystem()
{
	m_bCreated = false;
	if (m_hWnd) DestroyWindow(m_hWnd);
	if (m_hFont) DeleteObject(m_hFont);
	ReleaseBackBufferResources();
}

bool CLogSystem::Initialize(HWND hParentWnd, HINSTANCE hInstance)
{
	if (m_bCreated) return false;

	RegistNotification();

	MyRegisterClass(hInstance);

	if(!CreateLogWindow(hParentWnd, hInstance)) return false;

	CreateBackBuffer();
	CreateFont‍();
	float height = static_cast<float>(m_rcClient.bottom - m_rcClient.top);
	int margin = 5;

	m_scroll.initialize(scroll::SCRType::SCR_V, height, height, height - 2.f * margin, POINT { m_rcClient.right - margin * 2, m_rcClient.top + margin}, 150.f, 0.f, margin * 1.f);

	// Run Draw Timer
	SetTimer(m_hWnd, WT_DEBUG_DRWACALL, m_TickFrequency, NULL);

	// 클래스와 윈도우 프로시저 연결
	::SetUserDataPtr(m_hWnd, this);

	//	윈도우 표시
	ShowWindow(m_hWnd, SW_SHOW);
	UpdateWindow(m_hWnd);

	return m_bCreated = true;
}

void CLogSystem::CreateFont‍()
{
	// Create Font
	LOGFONT logFont;

	logFont.lfHeight = 18;
	logFont.lfWidth = 0;
	logFont.lfEscapement = 0;
	logFont.lfOrientation = 0;
	logFont.lfWeight = 400;
	logFont.lfItalic = FALSE;
	logFont.lfUnderline = FALSE;
	logFont.lfStrikeOut = 0;
	logFont.lfCharSet = HANGEUL_CHARSET;
	logFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
	logFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	logFont.lfQuality = ANTIALIASED_QUALITY;
	logFont.lfPitchAndFamily = DEFAULT_PITCH | FF_SWISS;
	lstrcpy(logFont.lfFaceName, TEXT("맑은 고딕"));

	m_hFont = CreateFontIndirect(&logFont);
}

void CLogSystem::Draw(HDC hdc)
{
	FillRect(m_hDCBackBuffer, &m_rcClient, static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH)));
	auto oldFont = SelectObject(m_hDCBackBuffer, m_hFont);

	RECT rc{ 10, 10, m_rcClient.right - 10, m_rcClient.bottom };

	for (auto p : m_mapObservers)
		p.second->Draw(m_hDCBackBuffer, rc);

	m_scroll.Draw(m_hDCBackBuffer, m_rcClient);

	BitBlt(hdc, 0, 0, m_rcClient.right, m_rcClient.bottom, m_hDCBackBuffer, 0, 0, SRCCOPY);
	SelectObject(hdc, oldFont);
}

ATOM CLogSystem::MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style =
		  CS_HREDRAW 	// 클라이언트의 너비를 변경하면, 전체 윈도우를 다시 그리게 한다.
		| CS_VREDRAW	// 클라이언트의 높이를 변경하면, 전체 윈도우를 다시 그리게 한다.
	//	| CS_DBLCLKS	// 해당 윈도우에서 더블클릭을 사용해야 한다면 추가해야 한다.
		;
	wcex.lpfnWndProc = CLogSystem::WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;	//	해당 프로그램의 인스턴스 핸들.

//	MAKEINTERSOURCE : Make Inter Source. 프로그램 내부에 있는 리소스의 인덱스를 반환하는 매크로.
	wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = m_szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

	return RegisterClassEx(&wcex);
}

bool CLogSystem::CreateLogWindow(HWND hParentWnd, HINSTANCE hInstance)
{
	//	윈도우 스타일
	DWORD dwStyle =
		  WS_OVERLAPPED 	// 디폴트 윈도우. 타이틀 바와 크기 조절이 안되는 경계선을 가진다. 아무런 스타일도 주지 않으면 이 스타일이 적용된다.
		| WS_CAPTION 		// 타이틀 바를 가진 윈도우를 만들며 WS_BORDER 스타일을 포함한다.
//		| WS_SYSMENU		// 시스템 메뉴를 가진 윈도우를 만든다.
//		| WS_MINIMIZEBOX	// 최소화 버튼을 만든다.
//		| WS_BORDER			// 단선으로 된 경계선을 만들며 크기 조정은 할 수 없다.
		| WS_THICKFRAME		// 크기 조정이 가능한 두꺼운 경계선을 가진다. WS_BORDER와 같이 사용할 수 없다.
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

void CLogSystem::CreateBackBuffer()
{
	ReleaseBackBufferResources();

	HDC hdc = GetDC(m_hWnd);
	{
		GetClientRect(m_hWnd, &m_rcClient);

		m_hDCBackBuffer = CreateCompatibleDC(hdc);
		m_hbmpBackBufffer = CreateCompatibleBitmap(hdc, m_rcClient.right, m_rcClient.bottom);

		SelectObject(m_hDCBackBuffer, m_hbmpBackBufffer);

		InvalidateRect(m_hWnd, NULL, FALSE);
	}
	ReleaseDC(m_hWnd, hdc);
}

void CLogSystem::ReleaseBackBufferResources()
{
	if (m_hDCBackBuffer)
	{
		SelectObject(m_hDCBackBuffer, static_cast<HBITMAP>(NULL));
		DeleteDC(m_hDCBackBuffer);
		m_hDCBackBuffer = NULL;
	}
	if (m_hbmpBackBufffer)
	{
		DeleteObject(m_hbmpBackBufffer);
		m_hbmpBackBufffer = NULL;
	}
}

void CLogSystem::RegistNotification() 
{
	m_mapObservers[Event::MeasureTimeElapsed]	= new CMeasureTimeElapsedObserver();
	m_mapObservers[Event::CustomLogger]			= new CCustomLoggerObserver();

}

void CLogSystem::PropagateNotification(const CObject* obj, Event* pEvent)
{
	m_mapObservers[pEvent->ID]->onNotify(obj, pEvent);
}

LRESULT CLogSystem::WndProc(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	CLogSystem* self = ::GetUserDataPtr<CLogSystem*>(hWnd);

	if (!self)
		return ::DefWindowProc(hWnd, nMessageID, wParam, lParam);

	switch (nMessageID)
	{
	case WM_CREATE:
		break;

	case WM_SYSKEYDOWN:
		switch (wParam)
		{
		case VK_F4:
			return 0;
		}
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_SPACE: 
			for(auto p : self->m_mapObservers)
				p.second->RenewalViewList();
			break;
		}
		break;

	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
		self->m_scroll.OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;

	case WM_TIMER:
		switch (wParam)
		{
		case WT_DEBUG_DRWACALL:
			InvalidateRect(hWnd, NULL, FALSE);
			break;
		}
		break;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = ::BeginPaint(hWnd, &ps);
		self->Draw(hdc);
		::EndPaint(hWnd, &ps);
	}
	break;

	default:
		return DefWindowProc(hWnd, nMessageID, wParam, lParam);
	}
	
	return 0;
}



void CMeasureTimeElapsedObserver::RenewalViewList()
{
	string filename = "MeasureTimeElapse.log";

	ifstream ifs(filename, ios::in);
	if (!ifs.is_open())
	{
		ifs.close();

		ofstream ofs(filename, ios::out);
		auto D = MessageBox(NULL, TEXT("view list 파일이 없습니다. 새로 작성합니다. 모든 시간 경과를 출력합니까?"), TEXT("오류"), MB_YESNO);
		if (D == IDYES)
		{
			for (auto s : m_mapFunctionTimeElapsed)
				ofs << s.first << endl;

		}
		ofs.close();
		return RenewalViewList();
	}

	while (!ifs.eof())
	{
		string str;
		ifs >> str;

		if (str.length() > 0)
			m_vViewStrings.emplace_back(move(str));
	}
	ifs.close();
}

void CMeasureTimeElapsedObserver::Draw(HDC hdc, RECT& rcDrawArea)
{
	POINT ptStart { rcDrawArea.left, rcDrawArea.top };

//	int iHeight = 0;
	for (auto& p : m_vViewStrings)
	{
		const size_t newsizew = strlen(p.c_str()) + 1;
		size_t convertedChars = 0;
		wstring wstr;
		wstr.resize(newsizew);
		mbstowcs_s(&convertedChars, &(wstr[0]), newsizew, p.c_str(), _TRUNCATE);
		if (wstr.back() == 0) wstr.pop_back();

		double fMaxcount = 0.f;

#if USE_TIME_ELAPSED_HISTORY

		long long maxcount = 0;

		for (auto q : m_mapFunctionTimeElapsed[p])
			maxcount += q;


		if (maxcount > 0)
			fMaxcount = static_cast<double>(maxcount) 
			/ static_cast<double>(m_mapFunctionTimeElapsed[p].size()) 
			* 0.001	//  μs
			* 0.001	//	ms
			;
#else
		
		bool bValid = (m_mapFunctionTimeElapsed[p].second > 0);

		if (bValid)
			fMaxcount	= static_cast<double>(m_mapFunctionTimeElapsed[p].first)
						/ static_cast<double>(m_mapFunctionTimeElapsed[p].second) 
						* 0.001	//  μs
						* 0.001	//	ms
			;
#endif

		wstring soutput = 
#if USE_TIME_ELAPSED_HISTORY
			wstr + L" : " + ((m_mapFunctionTimeElapsed[p].size() > 0) ?
			to_wstring(fMaxcount) : L"0") + L" ms"
			+ L" / " + FixValue(m_mapFunctionTimeElapsed[p].size()) + L" 회";
#else
			wstr + L" : " + (bValid ? _Floating_to_wstring(L"%.3lf", fMaxcount) : L"0") + L" ms"
			+ L" / " + FixValue(m_mapFunctionTimeElapsed[p].second) + L" 회";
#endif
		rcDrawArea.top += DrawText(hdc, soutput.c_str(), -1, &rcDrawArea, DT_LEFT | DT_SINGLELINE);
		
	}

}



void CCustomLoggerObserver::RenewalViewList()
{
	string filename = "CustomLogger.log";

	ifstream ifs(filename, ios::in);
	if (!ifs.is_open())
	{
		ifs.close();

		ofstream ofs(filename, ios::out);
		auto D = MessageBox(NULL, TEXT("view list 파일이 없습니다. 새로 작성합니다. 모든 로거를 출력합니까?"), TEXT("오류"), MB_YESNO);
		if (D == IDYES)
		{
			for (auto s : m_mapStringLogger)
				ofs << s.first << endl;

		}
		ofs.close();
		return RenewalViewList();
	}

	while (!ifs.eof())
	{
		string str;
		ifs >> str;

		if (str.length() > 0)
			m_vViewStrings.emplace_back(move(str));
	}
	ifs.close();
}

void CCustomLoggerObserver::Draw(HDC hdc, RECT& rcDrawArea)
{
	POINT ptStart { rcDrawArea.left, rcDrawArea.top };

	int iHeight = 0;
	for (auto p : m_vViewStrings)
	{
		string str = p + " : " + m_mapStringLogger[p];

		const size_t newsizew = strlen(str.c_str()) + 1;
		size_t convertedChars = 0;
		wstring wstr;
		wstr.resize(newsizew);
		mbstowcs_s(&convertedChars, &(wstr[0]), newsizew, str.c_str(), _TRUNCATE);
		if (wstr.back() == 0) wstr.pop_back();

		RECT rc { rcDrawArea.left, rcDrawArea.top + iHeight, rcDrawArea.right, rcDrawArea.bottom };

		rcDrawArea.top += DrawText(hdc, wstr.c_str(), -1, &rcDrawArea, DT_LEFT | DT_SINGLELINE);

	}
}

#endif
