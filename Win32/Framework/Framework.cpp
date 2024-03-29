#include "stdafx.h"
#include "Scene\Title\TitleScene.h"
#include "Framework.h"
#include "Timer\Timer.h"

CDirectXFramework::CDirectXFramework()
{
}

CDirectXFramework::~CDirectXFramework()
{
	ReleaseObjects();
}

bool CDirectXFramework::Initialize(HINSTANCE hInstance, HWND hWnd)
{
	m_hInstance = hInstance;
	m_hWnd = hWnd;
	::GetClientRect(m_hWnd, &m_rcClient);

	// 캡션 변경
#if defined(TITLESTRING)
	lstrcpy(m_CaptionTitle, TITLESTRING);
#else
	GetWindowText(m_hWnd, m_CaptionTitle, TITLE_MAX_LENGTH);
#endif
#if defined(SHOW_CAPTIONFPS)
	lstrcat(m_CaptionTitle, TEXT(" ("));
#endif
	m_TitleLength = lstrlen(m_CaptionTitle);
	SetWindowText(m_hWnd, m_CaptionTitle);

	MeasureBlockTimeElapsed([&] (auto d) {

//		auto du = chrono::duration_cast<chrono::milliseconds>(d).count();
//		MessageBox(m_hWnd, (FixValue(static_cast<long long>(du)) + TEXT(" ms")).c_str(), TEXT(""), MB_OK);
	}, [&] () {
		CreateD3D11Deivce();
		BuildScene();
	});

	// 클래스와 윈도우 프로시저 연결
//	if (!CreateD3D11Deivce()) return false;
//
//	BuildScene();

	return true;
}

void CDirectXFramework::CreateChildWindow()
{
	// Add Test Child Windows
	// Register the window class.
	WNDCLASSEX wcex = { 0 };
	wcex.cbSize = sizeof(wcex);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = ChildProc;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
	wcex.lpszClassName = L"Child";
	RegisterClassEx(&wcex);

    // Create the playback control child window.
	m_hWndChild = CreateWindowEx( WS_EX_LAYERED								// Extended window style
								, wcex.lpszClassName						// Name of window class
								, NULL										// Title-bar string
								, WS_POPUP | WS_CLIPSIBLINGS | WS_CAPTION	// Child window
								, 750, 200
								, 250, 600									// Window will be resized via MoveWindow
								, m_hWnd									// Parent
								, NULL										// Class menu
								, m_hInstance								// Handle to application instance
								, NULL);									// Window-creation data

	if (!m_hWndChild)
	{
		HRESULT_FROM_WIN32(GetLastError());
	}
	else
	{
		SetLayeredWindowAttributes(m_hWndChild, 0, 130, LWA_ALPHA);
		ShowWindow(m_hWndChild, SW_SHOWDEFAULT);
	}
}

void CDirectXFramework::ReleaseObjects()
{
	// D3D11 Device Resources
	ReleaseD3D11Devices();
	
	// D2D Resource 해제
	ReleaseD2DResources();
}

bool CDirectXFramework::CreateD3D11Deivce()
{

	// 지원하는 하드웨어 그래픽 드라이버를 열거합니다.
	D3D_DRIVER_TYPE d3dDriverTypes[]
	{
		// 하드웨어 드라이버, Direct3D의 기능이 최대한 하드웨어로 구현 
		// 하드웨어적으로 제공하지 않는 기능은 소프트웨어를 통해 구현
		  D3D_DRIVER_TYPE_HARDWARE
		// DirectX SDK에서 제공하는 고성능의 소프트웨어 드라이버
		// 특성 레벨 9_1에서 특성 레벨 10.1까지 제공
		// - 별도의 소프트웨어 드라이버(래스터라이저)가 필요없음
		// - 소프트웨어 드라이버이면서 그래픽 하드웨어의 최대 성능을 지원
		//   Direct3D 11을 지원하지 않는 그래픽 하드웨어에서도 사용
		, D3D_DRIVER_TYPE_WARP
		// 참조 드라이버(모든 Direct3D 특성을 지원하는 소프트웨어 드라이버)
		// DirectX SDK에서 속도보다는 정확성을 위해 제공
		// CPU를 사용하여 래스터라이저 구현 
		// Direct3D 11의 모든 특성 레벨에서 이 드라이버를 사용할 수 있음
		// 응용 프로그램의 테스트와 디버깅을 위해 주로 사용
		, D3D_DRIVER_TYPE_REFERENCE
	};

	// 이 배열은 이 응용 프로그램에서 지원하는 DirectX 하드웨어 기능 수준 집합을 정의합니다.
	// 순서를 유지해야 합니다.
	// 설명에서 응용 프로그램에 필요한 최소 기능 수준을 선언해야 합니다.
	// 별도로 지정하지 않은 경우 모든 응용 프로그램은 9.1을 지원하는 것으로 간주됩니다.
	D3D_FEATURE_LEVEL d3dFeatureLevels[]
	{
		  D3D_FEATURE_LEVEL_11_1
		, D3D_FEATURE_LEVEL_11_0
		, D3D_FEATURE_LEVEL_10_1
		, D3D_FEATURE_LEVEL_10_0
	};
	// 지원 가능한 모든 하드웨어 기능 수준 집합 수를 구합니다.
	auto szFeatureLevel = static_cast<UINT>(GetArraySize(d3dFeatureLevels));

	// 최종적으로 사용될 장치의 기능 수준을 저장하기 위한 변수입니다.
	D3D_FEATURE_LEVEL nd3dFeatureLevel = D3D_FEATURE_LEVEL_11_1;

	// Result Handle 입니다. 장치가 성공적으로 생성도는지 검사합니다.
	HRESULT hResult = S_OK;

	// 이 플래그는 API 기본값과 다른 색 채널 순서의 표면에 대한 지원을
	// 추가합니다. Direct2D와의 호환성을 위해 필요합니다.
	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(_DEBUG)

	auto IsSDKLayerAvailable = []() -> bool {
		return SUCCEEDED(D3D11CreateDevice(	  nullptr
											, D3D_DRIVER_TYPE_NULL			// 실제 하드웨어 장치를 만들 필요가 없습니다.
											, 0
											, D3D11_CREATE_DEVICE_DEBUG		// SDK 레이어를 확인하세요.
											, nullptr						// 모든 기능 수준이 적용됩니다.
											, 0
											, D3D11_SDK_VERSION				// Windows 스토어 앱의 경우 항상 이 값을 D3D11_SDK_VERSION으로 설정합니다.
											, nullptr						// D3D 장치 참조를 보관할 필요가 없습니다.
											, nullptr						// 기능 수준을 알 필요가 없습니다.
											, nullptr						// D3D 장치 컨텍스트 참조를 보관할 필요가 없습니다.
		));
	};

	// SDK 레이어 지원을 확인하세요.
	if (IsSDKLayerAvailable())
	{
		// 프로젝트가 디버그 빌드 중인 경우에는 이 플래그가 있는 SDK 레이어를 통해 디버깅을 사용하십시오.
		creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
	}
#endif
	
	// Direct3D 11 API 장치 개체와 해당 컨텍스트를 만듭니다.
	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> context;

	for (D3D_DRIVER_TYPE &p : d3dDriverTypes)
	{
		if (SUCCEEDED(hResult = D3D11CreateDevice(	  NULL						// 기본 어댑터를 사용하려면 nullptr을 지정합니다.
													, p							// 하드웨어 그래픽 드라이버를 사용하여 장치를 만듭니다.
													, 0							// 드라이버가 D3D_DRIVER_TYPE_SOFTWARE가 아닌 경우 0이어야 합니다.
													, creationFlags				// 디버그 및 Direct2D 호환성 플래그를 설정합니다.
													, d3dFeatureLevels			// 이 응용 프로그램이 지원할 수 있는 기능 수준 목록입니다.
													, szFeatureLevel			// 위 목록의 크기입니다.
													, D3D11_SDK_VERSION			// Windows 스토어 앱의 경우 항상 이 값을 D3D11_SDK_VERSION으로 설정합니다.
													, &device					// 만들어진 Direct3D 장치를 반환합니다.
													, &nd3dFeatureLevel			// 만들어진 장치의 기능 수준을 반환합니다.
													, &context					// 장치 직접 컨텍스트를 반환합니다.
												)
						)
			)
		{
			device.As(&m_pd3dDevice);
			context.As(&m_pd3dDeviceContext);

			break;
		}
	}

	// 모든 지원 가능한 드라이버에서 생성이 실패하면 프로그램을 종료합니다.
	if (!m_pd3dDevice)
	{
		MessageBox(m_hWnd, TEXT("지원 가능한 그래픽 사양이 없습니다."), TEXT("프로그램 구동 실패"), MB_OK);
		return(false);
	}

	// Swap Chain Description 구조체
	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	#pragma region [DXGI_SWAP_CHAIN_DESC 초기화]
	// 구조체 비우기
	::ZeroMemory(&dxgiSwapChainDesc, sizeof(dxgiSwapChainDesc));
	// BufferCount : 후면 버퍼의 수를 지정
	dxgiSwapChainDesc.BufferCount = 2;
		
	// BufferDesc : 후면 버퍼의 디스플레이 형식을 지정
	{
		// Width : 버퍼의 가로 크기를 픽셀로 지정
		dxgiSwapChainDesc.BufferDesc.Width = m_rcClient.right;
		// Height : 버퍼의 세로 크기를 픽셀로 지정
		dxgiSwapChainDesc.BufferDesc.Height = m_rcClient.bottom;
		// Format : 후면 버퍼 픽셀 형식
		/// DirectX 11-1(Chap 01)-Device, p.49 참조
		dxgiSwapChainDesc.BufferDesc.Format = m_dxgiFormatCurrentSelect;
		// RefreshRate : 화면 갱신 비율을 Hz 단위로 지정
		{
			// Denominator : 분모
			dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
			// Numerator : 분자
			dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		}
		// ScanlineOrdering : scan line 그리기 모드 지정(기본 0)
		//	DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED        (0) : 스캔 라인 순서를 지정하지 않음	
		//	DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE        (1) : 프로그레시브 스캔 라인 순서 지정
		//	DXGI_MODE_SCANLINE_ORDER_UPPER_FIELD_FIRST  (2) : 상위 필드로 이미지 생성
		//	DXGI_MODE_SCANLINE_ORDER_LOWER_FIELD_FIRST  (3) : 하위 필드로 이미지 생성
		dxgiSwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	}

	// BufferUsage : 후면 버퍼에 대한 표면 사용 방식과 CPU의 접근 방법 지정
	//	DXGI_USAGE_SHADER_INPUT				: shader 의 입력으로 사용
	//	DXGI_USAGE_RENDER_TARGET_OUTPUT		: render target으로 사용
	//	DXGI_USAGE_BACK_BUFFER         		: back buffer 로 사용
	//	DXGI_USAGE_SHARED              		: 공유 목적
	//	DXGI_USAGE_READ_ONLY           		: 읽기 전용
	//	DXGI_USAGE_DISCARD_ON_PRESENT  		: DXGI 내부 전용 사용(사용자가 사용하지 않음)
	//	DXGI_USAGE_UNORDERED_ACCESS    		: 무순서화 접근
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	// OutputWindow : 출력 윈도우의 핸들을 지정(반드시 유효해야 함)
	dxgiSwapChainDesc.OutputWindow = m_hWnd;

	// SampleDesc : 다중 샘플링의 품질을 지정
	// CheckMultisampleQualityLevels 함수를 사용하여 다중 샘플링 가능 여부를 확인한 뒤에 값 변경
	{
		// Count : 픽셀 당 샘플 개수
		//	1  : 다중 샘플링을 하지 않음
		//	2~ : 해당 수만큼의 다중 샘플링
		dxgiSwapChainDesc.SampleDesc.Count = 1;
		// Quality : 품질 레벨
		// 0 : 다중 샘플링을 하지 않음
		dxgiSwapChainDesc.SampleDesc.Quality = 0;
	}

	// Windowed : 윈도우 모드 또는 전체 화면 모드 지정 ~ TRUE  | 창 모드 
	//												   ~ FALSE | 전체 화면
	dxgiSwapChainDesc.Windowed = TRUE;

	// Flags : Swap Chain 동작에 대한 선택 사항을 지정
	//	DXGI_SWAP_CHAIN_FLAG_NONPREROTATED		(1) : 전체 화면 모드에서 전면 버퍼의 내용을 화면으로 옮길 때 자동 회전하지 않음
	//	DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH	(2) : 응용 프로그램이 디스플레이 모드를 변경할 수 있음
	//	DXGI_SWAP_CHAIN_FLAG_GDI_COMPATIBLE		(4) : 응용 프로그램이 GDI 로 랜더링 할 수 있음. 후면 버퍼에 GetDC() 사용 가능
	dxgiSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH/*2*/;

	// SwapEffect : Swaping을 처리하는 선택사항을 지정(기본 : 0)
	//	DXGI_SWAP_EFFECT_DISCARD		(0) : 버퍼 내용을 폐기
	//	DXGI_SWAP_EFFECT_SEQUENTIAL		(1) : 순차 복사
	// DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL	(2) : Flip 순차 복사
	dxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	
	#pragma endregion

	// DXGI Device 를 받습니다.
	ComPtr<IDXGIDevice3> pdxgiDevice = NULL;
	if (FAILED(hResult = m_pd3dDevice.As(&pdxgiDevice) ))
	{
		MessageBox(m_hWnd, TEXT("DXGI Device 객체를 반환받지 못했습니다. 프로그램을 종료합니다."), TEXT("프로그램 구동 실패"), MB_OK);
		return(false);
	}
	
	// DXGI Device 에서 Adapter를 받습니다. DXGI Factory를 생성하기 위해 필요합니다.
	ComPtr<IDXGIAdapter> pdxgiAdapter;
	if (FAILED(hResult = pdxgiDevice->GetAdapter(&pdxgiAdapter)))
	{
		MessageBox(m_hWnd, TEXT("DXGIAdapter에서의 객체 생성이 실패했습니다. 프로그램을 종료합니다."), TEXT("프로그램 구동 실패"), MB_OK);
		return(false);
	}
	
	// DXGI Factory 인스턴스를 DXGIFactory에서 받습니다.
	ComPtr<IDXGIFactory3> pdxgiFactory = NULL;
	if (FAILED(hResult = pdxgiAdapter->GetParent(IID_PPV_ARGS(&pdxgiFactory))))
	{
		MessageBox(m_hWnd, TEXT("DXGIFactory에서의 객체 생성이 실패했습니다. 프로그램을 종료합니다."), TEXT("프로그램 구동 실패"), MB_OK);
		return(false);
	}

	//  SwapChain 을 생성합니다
	ComPtr<IDXGISwapChain> pdxgiSwapChain;
	if (FAILED(hResult = pdxgiFactory->CreateSwapChain(pdxgiDevice.Get(), &dxgiSwapChainDesc, pdxgiSwapChain.GetAddressOf())))
	{
		MessageBox(m_hWnd, TEXT("SwapChain 인스턴스 생성이 실패했습니다. 프로그램을 종료합니다."), TEXT("프로그램 구동 실패"), MB_OK);
		return(false);
	}
	pdxgiSwapChain.As(&m_pdxgiSwapChain);

	// Direct2D : Direct2D 인스턴스를 생성합니다.
	if (!CreateD2D1Device(pdxgiDevice.Get()))
	{
		MessageBox(m_hWnd, TEXT("Direct2D 인스턴스 생성이 실패했습니다. 프로그램을 종료합니다."), TEXT("프로그램 구동 실패"), MB_OK);
		return(false);
	}

//	pdxgiDevice->SetMaximumFrameLatency(1);

	// render target과 depth-stencil buffer 생성
	if(!CreateRenderTargetDepthStencilView())
	{
		MessageBox(m_hWnd, TEXT("RenderTarget이나 Depth-Stencil 버퍼 생성이 실패했습니다. 프로그램을 종료합니다."), TEXT("프로그램 구동 실패"), MB_OK);
		return(false);
	}
	
	// Direct2D : RenderTarget에서 2DBackBuffer를 획득합니다.
	CreateD2DBackBuffer();
	return true;
}

bool CDirectXFramework::CreateRenderTargetDepthStencilView()
{


	// Result Handle 입니다. 장치가 성공적으로 생성도는지 검사합니다.
	HRESULT hResult = S_OK; 
		
	// render target 을 생성하기 위한 back buffer 를 SwapChain 에게 요청합니다.
	ComPtr<ID3D11Texture2D> pd3dBackBuffer{ nullptr };

	if (FAILED(hResult = m_pdxgiSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *)&pd3dBackBuffer))) return(false);

	// 반환받은 버퍼를 사용하여 render target view 를 생성합니다.
	if (FAILED(hResult = m_pd3dDevice->CreateRenderTargetView(pd3dBackBuffer.Get(), NULL, &m_pd3dRenderTargetView))) return(false);



	// Rendering 추가 부분 : Depth-Stencil 을 하지 않으면 정상적인 큐브가 출력되지 않는다.
	#pragma region [Create Depth-Stencil Buffer]

	// depth stencil "texture" 를 생성합니다.
	D3D11_TEXTURE2D_DESC d3dDepthStencilBufferDesc;
	
	// 메모리는 0으로 초기화합니다.
	ZeroMemory(&d3dDepthStencilBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));



	// Width : texture 의 너비입니다.
	d3dDepthStencilBufferDesc.Width = m_rcClient.right;
	// Height : texture 의 높이입니다.
	d3dDepthStencilBufferDesc.Height = m_rcClient.bottom;
	// MipLevels : texture 최대 MipMap Level 수. 
	//				다중 샘플링 텍스처 : 1
	//				최대 밉맵 레벨 : 0
	d3dDepthStencilBufferDesc.MipLevels = 1;
	// ArraySize :texture 배열의 texture 개수. (배열이 아니면 1)
	d3dDepthStencilBufferDesc.ArraySize = 1;
	// Format : texture 픽셀 형식
	d3dDepthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// SampleDesc : 다중 샘플링의 품질을 지정
	// CheckMultisampleQualityLevels 함수를 사용하여 다중 샘플링 가능 여부를 확인한 뒤에 값 변경
	{

		// Count : 픽셀 당 샘플 개수
		//	1  : 다중 샘플링을 하지 않음
		//	2~ : 해당 수만큼의 다중 샘플링
		d3dDepthStencilBufferDesc.SampleDesc.Count = 1;
		// Quality : 품질 레벨
		// 0 : 다중 샘플링을 하지 않음
		d3dDepthStencilBufferDesc.SampleDesc.Quality = 0;
	}

	// Usage : texture 를 읽고 쓰는 방법에 대한 정의
	d3dDepthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	// BindFlags : 파이프라인 단계 어디에 연결할 것인지 정의
	d3dDepthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	// MiscFlags : 리소스에 대한 추가 선택 사항. 사용하지 않으면 0.
	d3dDepthStencilBufferDesc.MiscFlags = 0;
	// CPUAccessFlags : CPU가 버퍼를 사용할 수 있는 유형. 사용하지 않으면 0.
	d3dDepthStencilBufferDesc.CPUAccessFlags = 0;

	//	기본 깊이-스텐실 버퍼의 깊이 버퍼를 읽으려고 한다.
	if (FAILED(hResult = m_pd3dDevice->CreateTexture2D(&d3dDepthStencilBufferDesc, NULL, &m_pd3dDepthStencilBuffer))) return(false);

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC d3dDepthStencilViewDesc;
	ZeroMemory(&d3dDepthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	d3dDepthStencilViewDesc.Format = d3dDepthStencilBufferDesc.Format;

	d3dDepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	d3dDepthStencilViewDesc.Texture2D.MipSlice = 0;

	if (FAILED(hResult = m_pd3dDevice->CreateDepthStencilView(m_pd3dDepthStencilBuffer.Get(), &d3dDepthStencilViewDesc, &m_pd3dDepthStencilView))) return(false);


	#pragma endregion
	//	DXGI_FRAME_STATISTICS p;
	//	m_pdxgiSwapChain->GetFrameStatistics(&p);


	// 전체 창을 대상으로 하기 위한 3D 렌더링 뷰포트를 설정합니다.

	return true;
}

void CDirectXFramework::ReleaseD3D11Devices()
{

}

//void CDirectXFramework::



void CDirectXFramework::Render()
{
	// OM에 RenderTarget 재설정
	SetMainRenderTargetView();
	ClearRenderTargetView();
	ClearDepthStencilView();

	long long llte_2D = 0;
//	long long llte_3D = 0;

//	MeasureFunctionTimeElapsed([&](auto d) {
//		auto du = chrono::duration_cast<chrono::nanoseconds>(d).count();
//		llte_3D += du;
//	}
//	, &CScene::Render3D, m_pCurrentScene, m_pd3dDeviceContext.Get());

	MeasureFunctionTimeElapsed([&](auto d) {
		auto du = chrono::duration_cast<chrono::nanoseconds>(d).count();
		llte_2D += du;
	}
	, &CDirectXFramework::Render2D, this);

#if USE_DEBUG_WINDOW
	{
		Event_MeasureTimeElapsed event(string("Rendering_Cost"), llte_2D + llte_3D);
		CLogSystem::PropagateNotification(nullptr, &event);	// 정보로 넘겨줘야할 object가 있을 경우
	}
	{
		Event_MeasureTimeElapsed event(string("Render2Dpercent"), static_cast<long long>(static_cast<double>(llte_2D * 100 * 1000 * 1000) / static_cast<double>(llte_2D + llte_3D)));
		CLogSystem::PropagateNotification(nullptr, &event);	// 정보로 넘겨줘야할 object가 있을 경우
	}
#endif

//	if (m_pCurrentScene) m_pCurrentScene->Render3D();
//
//	Render2D();

	m_pdxgiSwapChain->Present(0, 0);
}

void CDirectXFramework::Update(float fTimeElapsed)
{
	if (m_pCurrentScene) m_pCurrentScene->AnimateObjects(fTimeElapsed);
}

void CDirectXFramework::SetMainRenderTargetView()
{
	ID3D11RenderTargetView *const targets[1] = { m_pd3dRenderTargetView.Get() }; SetRenderTargetViews(targets);
}



void CDirectXFramework::SetCameraViewport(CCamera * pCamera)
{
}



void CDirectXFramework::ReleaseD2DResources()
{

}

bool CDirectXFramework::CreateD2D1Device(IDXGIDevice3 * pdxgiDevice)
{
	if (!pdxgiDevice) return (false);

	HRESULT hResult = S_OK;

	// Direct2D 리소스를 초기화합니다.
	D2D1_FACTORY_OPTIONS options;
	ZeroMemory(&options, sizeof(D2D1_FACTORY_OPTIONS));

#if defined(_DEBUG)
	// 프로젝트가 디버그 빌드 중인 경우 SDK 레이어를 통해 Direct2D 디버깅을 사용합니다.
	options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif

	// Direct2D 팩터리를 초기화합니다.
	if(FAILED(hResult = D2D1CreateFactory(	  D2D1_FACTORY_TYPE_MULTI_THREADED
											, __uuidof(decltype(m_pd2dFactory)::InterfaceType)
											, &options
											, &m_pd2dFactory
	))) goto ReleaseDXGI;

	// DirectWrite 팩터리를 초기화합니다.
	if (FAILED(hResult = DWriteCreateFactory(	  DWRITE_FACTORY_TYPE_SHARED
												, __uuidof(decltype(m_pdwFactory)::InterfaceType)
												, &m_pdwFactory
	))) goto ReleaseDXGI;
	
	// COM Library를 초기화합니다.
	// UWP에는 없어도 잘 돌아가더니 여기선 안 돌아가네.
	if (FAILED(hResult = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED))) goto ReleaseDXGI;

	// WIC(Windows Imaging Container) 팩터리를 초기화합니다.
	if (FAILED(hResult = CoCreateInstance(	  CLSID_WICImagingFactory
											, nullptr
											, CLSCTX_INPROC_SERVER
											, IID_PPV_ARGS(&m_wicFactory)
	))) goto ReleaseDXGI;
	
	// Multi-Thread 용 lock
	if (FAILED(hResult = m_pd2dFactory.As(&m_pd2dMultithread))) goto ReleaseDXGI;

	m_pd2dFactory->CreateDevice(pdxgiDevice, &m_pd2dDevice);
	m_pd2dFactory->CreateDrawingStateBlock(&m_pd2dStateBlock);

//	CreateHwndRenderTarget(m_hWndChild, m_hWndRenderTarget);

	// Direct2D DC를 생성합니다.
	m_pd2dDevice->CreateDeviceContext(
		  D2D1_DEVICE_CONTEXT_OPTIONS_NONE
		, &m_pd2dDeviceContext
	);

ReleaseDXGI:
	return (!FAILED(hResult));
}

void CDirectXFramework::CreateHwndRenderTarget(HWND hWnd, ComPtr<ID2D1HwndRenderTarget>& phwndRenderTarget)
{
	RECT rc{};
	::GetClientRect(hWnd, &rc);

	D2D1_SIZE_U size
	{
		  static_cast<UINT32>(rc.right - rc.left)
		, static_cast<UINT32>(rc.bottom - rc.top)
	};

	m_pd2dFactory->CreateHwndRenderTarget
	(
		  D2D1::RenderTargetProperties()
		, D2D1::HwndRenderTargetProperties(hWnd, size, D2D1_PRESENT_OPTIONS_IMMEDIATELY)
		, &phwndRenderTarget
	);
}

bool CDirectXFramework::CreateD2DBackBuffer()

{
	// 화면의 해상도를 얻습니다.
	auto dpi = GetDpiForWindow(m_hWnd);
	
	HRESULT hResult = S_OK;

	// 스왑 체인 백 버퍼에 연결된 Direct2D 대상
	// 비트맵을 만들고 이를 현재 대상으로 설정합니다.
	D2D1_BITMAP_PROPERTIES1 bitmapProperties =
		BitmapProperties1(	  D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW
				, PixelFormat(m_dxgiFormatCurrentSelect, D2D1_ALPHA_MODE_PREMULTIPLIED)
				, dpi
				, dpi
		);

	// DXGI 표면을 얻습니다.
	ComPtr<IDXGISurface2> dxgiBackBuffer;
	if (FAILED(hResult = m_pdxgiSwapChain->GetBuffer(0, IID_PPV_ARGS(&dxgiBackBuffer)))) return false;
	if (FAILED(hResult = m_pd2dDeviceContext->CreateBitmapFromDxgiSurface(	  dxgiBackBuffer.Get()
																			, &bitmapProperties
																			, &m_pd2dBmpBackBuffer
	))) return false;

	m_pd2dDeviceContext->SetTarget(m_pd2dBmpBackBuffer.Get());
	m_pd2dDeviceContext->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);

	return true;
}

void CDirectXFramework::Render2D()
{
	m_pd2dDeviceContext->SaveDrawingState(m_pd2dStateBlock.Get());

	m_pd2dDeviceContext->BeginDraw();
	{

		m_pd2dDeviceContext->Clear(ColorF{ ColorF::Gainsboro });
		if (m_pCurrentScene) m_pCurrentScene->Render2D(m_pd2dDeviceContext.Get());

	}
	m_pd2dDeviceContext->EndDraw();

	m_pd2dDeviceContext->RestoreDrawingState(m_pd2dStateBlock.Get());

	/////////////////////////////////////////////////////////////////////
//	m_hWndRenderTarget->BeginDraw();
//	{
//		m_hWndRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::CornflowerBlue));
//		if (m_pCurrentScene) m_pCurrentScene->Render2D(m_hWndRenderTarget.Get());
//
//	}
//	m_hWndRenderTarget->EndDraw();

}



void CDirectXFramework::BuildScene()
{

	auto pNewScene{ make_unique<CTitleScene>() };

	pNewScene->BuildObjects(TEXT("Title"), m_hWnd, this);
	pNewScene->BuildObjecsFromD3D11Devices(m_pd3dDevice.Get(), m_pd3dDeviceContext.Get());
	pNewScene->BuildObjecsFromD2D1Devices(m_pd2dDevice.Get(), m_pd2dDeviceContext.Get(), m_pdwFactory.Get(), m_wicFactory.Get());
	pNewScene->BuildContainer(m_pd3dDevice.Get(), m_pd2dFactory.Get(), m_pdwFactory, m_wicFactory);

	m_lstScenes.push_back(move(pNewScene));
	m_pCurrentScene = m_lstScenes.back().get();
}

bool CDirectXFramework::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
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

LRESULT CDirectXFramework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
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

			m_pd3dDeviceContext->OMSetRenderTargets(0, NULL, NULL);

			// SwapChain과 연관된 모든 리소스들을 해제하고, 
			m_pd3dRenderTargetView = nullptr;
			m_pd2dDeviceContext->SetTarget(nullptr);
			m_pd2dBmpBackBuffer = nullptr;
			m_pd3dDepthStencilView = nullptr;
			m_pd3dDepthStencilBuffer = nullptr;

			// 여기 문제
			HRESULT hr = m_pdxgiSwapChain->ResizeBuffers(2	, m_nWndClientWidth
															, m_nWndClientHeight
															, m_dxgiFormatCurrentSelect
															, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
			if (hr != S_OK) {

				if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
				{
					// 추가 사항 : 장치 재생성 구현 필요
					MessageBox(m_hWnd, TEXT("Device Lost! 프로그램을 다시 실행해 주세요."), TEXT("ResizeBuffer Fail!"), MB_OK);
				}
				else MessageBox(m_hWnd, TEXT("알 수 없는 이유! 프로그램을 다시 실행해 주세요."), TEXT("ResizeBuffer Fail!"), MB_OK);
				DestroyWindow(m_hWnd);
			}

			CreateRenderTargetDepthStencilView();
			CreateD2DBackBuffer();

			m_pd2dMultithread->Enter();
			{
				Render();
			}
			m_pd2dMultithread->Leave();

			if (m_pCurrentScene)
				if (m_pCurrentScene->OnProcessingWindowMessage(hWnd, nMessageID, reinterpret_cast<WPARAM>(m_pd3dDeviceContext.Get()), reinterpret_cast<LPARAM>(m_pd2dDeviceContext.Get()))) break;
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

void SetClientRect(HWND hWnd, int width, int height)
{
	RECT crt;
	DWORD Style, ExStyle;

	SetRect(&crt, 0, 0, width, height);
	Style = GetWindowLong(hWnd, GWL_STYLE);
	ExStyle = GetWindowLong(hWnd, GWL_EXSTYLE);
	AdjustWindowRectEx(&crt, Style, GetMenu(hWnd) != NULL, ExStyle);
	if (Style & WS_VSCROLL) crt.right += GetSystemMetrics(SM_CXVSCROLL);
	if (Style & WS_HSCROLL) crt.bottom += GetSystemMetrics(SM_CYVSCROLL);
	SetWindowPos(hWnd, NULL, 0, 0, crt.right - crt.left, crt.bottom - crt.top,
		SWP_NOMOVE | SWP_NOZORDER);
}

bool CDirectXFramework::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_F1:
			m_FullScreen.SetFullscreen_WindowMode(hWnd, m_rcClient);
			break;
		case VK_ESCAPE:
			::DestroyMessage(true);
			::PostMessage(m_hWnd, WM_DESTROY, 0, 0);
			break;
		}
		break;
	default:
		break;
	}
	return(false);
}


void CDirectXFramework::FrameAdvance(float fTimeElapsed)
{
	// 지속적인 상수버퍼의 갱신을 확인하기 위한 Update 함수
	Update(fTimeElapsed);

	m_pd2dMultithread->Enter();
	{
		Render();
	}
	m_pd2dMultithread->Leave();

	// 프레임레이트를 출력합니다.
#if defined(SHOW_CAPTIONFPS)
#if USE_DEBUG_WINDOW
	Event_CutsomLogger eventShowFPS(string("FPS"), to_string(m_fps));
	CLogSystem::PropagateNotification(nullptr, &eventShowFPS);
#endif

	_itow_s(static_cast<int>(m_Timer->GetFPS() + 0.1), m_CaptionTitle + m_TitleLength, TITLE_MAX_LENGTH - m_TitleLength, 10);
	wcscat_s(m_CaptionTitle + m_TitleLength, TITLE_MAX_LENGTH - m_TitleLength, TEXT(" FPS)"));
//	SetWindowText(m_hWnd, m_CaptionTitle);
#endif

}



LRESULT CALLBACK CDirectXFramework::WndProc(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	//	CDirectXFramework* self = ::GetUserDataPtr<CDirectXFramework*>(hWnd);
	//	if (!self)
	//		return ::DefWindowProc(hWnd, nMessageID, wParam, lParam);
	
	switch (nMessageID)
	{
	case WM_GETMINMAXINFO:
		((MINMAXINFO*) lParam)->ptMinTrackSize.x = CLIENT_MINIMUM_WIDTH;
		((MINMAXINFO*) lParam)->ptMinTrackSize.y = CLIENT_MINIMUM_HEIGHT;

		return 0;

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


	case WM_DESTROY:
		//		::SetUserDataPtr(hWnd, NULL);
		::DestroyMessage(true);
		::PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, nMessageID, wParam, lParam);

	}
	return 0;
}

LRESULT CALLBACK CDirectXFramework::ChildProc(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_DESTROY:
		::DestroyWindow(hWnd);
		break;

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
