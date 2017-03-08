#pragma once

enum class EBoxFace 
{
	  Front
	, Up
	, Back
	, Down
	, Left
	, Right
};



// xz 평면의 x
constexpr size_t map_width	= 200;
// xz 평면의 z
constexpr size_t map_height = 200;
// xy, yz 평면의 y
constexpr size_t map_depth	= 10;

// cube의 각 변의 너비의 반
constexpr float	cubeScale = 0.25f;



constexpr LONG CLIENT_WIDTH	 = 800;
constexpr LONG CLIENT_HEIGHT = 600;

constexpr LONG CLIENT_MINIMUM_WIDTH	 = 600;
constexpr LONG CLIENT_MINIMUM_HEIGHT = 480;



// 캡션 FPS 출력 여부 -------------------
// 항상 캡션에 FPS를 출력		(0 : 비활성 | 1 : 활성)
#define USE_CAPTIONFPS_ALWAYS	 0
#define USE_TITLE_STRING		 1
#define USE_LIMIT_MAX_FPS		 1
#define USE_DEBUG_WINDOW		 0

#define USE_EXTNED_TILE_FLAG	 1

#if USE_CAPTIONFPS_ALWAYS
#define SHOW_CAPTIONFPS 
#elif _DEBUG	// Debug에서는 항상 실행
#define SHOW_CAPTIONFPS 
#endif

#if defined(SHOW_CAPTIONFPS)
#define MAX_UPDATE_FPS 1.0 / 3.0
#endif

// 최대 FPS
#if USE_LIMIT_MAX_FPS
#define MAX_FPS 1.0 / 120.0
#else
#define MAX_FPS 0.0
#endif

#define TITLE_MAX_LENGTH 64

#if USE_TITLE_STRING
#define TITLESTRING	TEXT("Lab Project")
#endif

#define DEBUG_CLIENT_WIDTH	400
#define DEBUG_CLIENT_HEIGHT	600

#ifdef NEMOLAND_CLIENT
#if USE_DEBUG_WINDOW

#include "Framework\LogSystem\LogSystem.h"

#endif
#endif

// Network
#define	WM_SOCKET				WM_USER + 1 //
#define	BUF_SIZE				1024 
