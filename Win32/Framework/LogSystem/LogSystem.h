#pragma once
#if USE_DEBUG_WINDOW

#define USE_TIME_ELAPSED_HISTORY 0

struct Event
{
public:

	enum EType : USHORT {
		  unknown = 0
		, MeasureTimeElapsed
		, CustomLogger
	//	, ...
		, count
	};

	const EType		ID;
	bool CheckType(USHORT id) const { return ID == id; }

};

class CObserver {
	
protected:

	friend class CLogSystem;
	CObserver() = default;

public:

	virtual ~CObserver() {}
	virtual void onNotify(const class CObject* obj, Event* _event) = 0;
	virtual void Draw(HDC hdc, RECT& rcDrawArea) {}

	virtual void RenewalViewList() {}

};



struct Event_MeasureTimeElapsed
	: public Event
{
	Event_MeasureTimeElapsed() : Event{ MeasureTimeElapsed } {}
	Event_MeasureTimeElapsed(string str, long long te)
		: Event_MeasureTimeElapsed()
	{
		strTag = str;
		llTimeElapsed = te;
	}

	string		strTag;
	long long	llTimeElapsed = 0LL;
};

class CMeasureTimeElapsedObserver
	: public CObserver
{
	using MyEvent = Event_MeasureTimeElapsed;

private:

#if USE_TIME_ELAPSED_HISTORY
	map<string, vector<long long>>	m_mapFunctionTimeElapsed;
#else
	map<string, pair<long long,size_t>>	m_mapFunctionTimeElapsed;
#endif
	vector<string>						m_vViewStrings;

public:

	CMeasureTimeElapsedObserver() 
	{
		RenewalViewList();
	}
	virtual ~CMeasureTimeElapsedObserver() {}

	void RenewalViewList();

	virtual void onNotify(const class CObject* obj, Event* _event)
	{
		if (!_event->CheckType(Event::MeasureTimeElapsed)) return;
		auto ThisEvent = static_cast<MyEvent*>(_event);
#if USE_TIME_ELAPSED_HISTORY
		m_mapFunctionTimeElapsed[ThisEvent->strTag].emplace_back(ThisEvent->llTimeElapsed);
#else
		m_mapFunctionTimeElapsed[ThisEvent->strTag].first += ThisEvent->llTimeElapsed;
		m_mapFunctionTimeElapsed[ThisEvent->strTag].second ++;
#endif
	}

	void Draw(HDC hdc, RECT& rcDrawArea);

};

struct Event_CutsomLogger
	: public Event
{
	Event_CutsomLogger() : Event{ CustomLogger } {}
	Event_CutsomLogger(string str, string log)
		: Event_CutsomLogger()
	{
		strTag = str;
		strLog = log;
	}

	string		strTag;
	string		strLog;
};

class CCustomLoggerObserver
	: public CObserver
{
	using MyEvent = Event_CutsomLogger;

private:

	map<string, string>				m_mapStringLogger;
	vector<string>					m_vViewStrings;

public:

	CCustomLoggerObserver()
	{
		RenewalViewList();
	}
	virtual ~CCustomLoggerObserver() {}

	void RenewalViewList();

	virtual void onNotify(const class CObject* obj, Event* _event)
	{
		if (!_event->CheckType(Event::CustomLogger)) return;
		auto ThisEvent = static_cast<MyEvent*>(_event);
		m_mapStringLogger[ThisEvent->strTag] = ThisEvent->strLog;
	}

	virtual void Draw(HDC hdc, RECT& rcDrawArea);

};




class CLogSystem {
	
private:
#define WT_DEBUG_DRWACALL 0

public:

	CLogSystem() = default;
	virtual ~CLogSystem();

	bool Initialize(HWND hParentWnd, HINSTANCE hInstance);

	void CreateFont‍();

	void Draw(HDC hdc);

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

protected:

	ATOM MyRegisterClass(HINSTANCE hInstance);
	bool CreateLogWindow(HWND hParentWnd, HINSTANCE hInstance);
	void CreateBackBuffer();
	void ReleaseBackBufferResources();

	void RegistNotification();

	static bool		m_bCreated;

	HWND			m_hWnd				{ nullptr };
	LPCTSTR			m_szWindowClass		{ TEXT("CLogSystem") };
	LPCTSTR			m_szTitle			{ TEXT("Log Window") };
	POINT			m_ptStartPosition	{ 20,20	};


	RECT			m_rcClient					;

	HDC				m_hDCBackBuffer		{ NULL };
	HBITMAP			m_hbmpBackBufffer	{ NULL };


	LONG			m_TickFrequency		{ 100 }	;

	scroll_Win32	m_scroll					;

	HFONT			m_hFont				{ NULL };
protected:

	// 단, 등록된 Observer에게만 정보를 보냅니다.
	static map<Event::EType, CObserver*> m_mapObservers;

public:

	static void PropagateNotification(const class CObject* obj, Event* pEvent);

};

#endif
