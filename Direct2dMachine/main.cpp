#include "winner.h"
#include <iostream>
#include <memory>
#include <array>
#include "graphics_2d.h"
#include "console.h"
#include "os_utils.h"
#include "key_math.h"
#include "sprite.h"
#if defined _DEBUG && !defined NDEBUG
#	pragma comment( lib, "C:/Program Files (x86)/Visual Leak Detector/lib/Win64/vld.lib" )
#	include <C:/Program Files (x86)/Visual Leak Detector/include/vld.h>
#endif

#pragma comment( linker, "/SUBSYSTEM:WINDOWS" )

#define rand01 ( rand() % 10000 ) / 10000.0f
#define IDT_TIMER1	1000
#define IDT_TIMER2	1001


std::pair<int,int> parseCommandLineArguments();
void update( HWND hWnd );
int loop( HWND hWnd );
LRESULT CALLBACK WindowProc( _In_ HWND hWnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam );
void doTiming();


static std::unique_ptr<Graphics2D> g_gph;
static int g_width;
static int g_height;
static float y = 0.0f;
static float ySpeed = 1.0f;
UINT_PTR g_pTimerEvent;
static std::unique_ptr<Sprite> g_ba2;
static std::unique_ptr<SpriteSheet> g_player;
static std::array<int, 5> g_spriteOrder = {2, 3, 0, 6, 7};
static int g_currentSprite = 0;
// timing
static LARGE_INTEGER g_freq;
static float g_deltaTime;
static float g_elapsedTime;
static LARGE_INTEGER g_initialTime;


int messageBoxPrintf( const TCHAR* caption,
	const TCHAR* format,
	... )
{
	TCHAR szBuffer[1024];
	va_list pArgs;
	va_start( pArgs,
		format );
	vswprintf_s( szBuffer,
		sizeof szBuffer / sizeof TCHAR,
		format,
		pArgs );
	va_end( pArgs );
	return MessageBoxW( nullptr,
		szBuffer,
		caption,
		0u );
}


int WINAPI wWinMain( _In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ wchar_t* lpCmdLine,
	_In_ int nShowCmd )
{
	HRESULT hres;
	LPCWSTR className = L"Direct2dClass";
	WNDCLASSEX windowClass;
	ZeroMemory( &windowClass, sizeof WNDCLASSEX );
	windowClass.cbSize = sizeof WNDCLASSEX;
	windowClass.hbrBackground = (HBRUSH) GetStockObject( WHITE_BRUSH );
	windowClass.hInstance = hInstance;
	windowClass.lpfnWndProc = &WindowProc;
	windowClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	windowClass.lpszClassName = className;
	windowClass.hCursor = LoadCursorW( nullptr,
		IDI_APPLICATION );

	RegisterClassExW( &windowClass );
	ASSERT_HRES_WIN32_IF_FAILED( hres );

	auto dims = parseCommandLineArguments();
	// calculate window size based on desired client region size
	g_width = dims.first;
	g_height = dims.second;
	RECT rect = {0, 0, dims.first, dims.second};
	BOOL ret = AdjustWindowRect( &rect,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		FALSE );

	LPCWSTR windowName = L"Direct2dMachine";
	uint32_t windowExStyles = WS_EX_OVERLAPPEDWINDOW;
	uint32_t windowStyles = WS_VISIBLE | WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU
		| WS_OVERLAPPEDWINDOW;
	HWND hWnd = CreateWindowExW( windowExStyles,
		className,
		windowName,
		windowStyles,
		200,
		100,
		rect.right - rect.left,
		rect.bottom - rect.top,
		nullptr,
		nullptr,
		hInstance,
		nullptr );
	ASSERT_HRES_WIN32_IF_FAILED( hres );

	QueryPerformanceFrequency( &g_freq );
	QueryPerformanceCounter( &g_initialTime );

	g_gph = std::make_unique<Graphics2D>( hWnd );
	g_ba2 = std::make_unique<Sprite>( LR"(img\ba2_icon.png)", g_gph.get() );
	g_player = std::make_unique<SpriteSheet>( LR"(img\player.png)", g_gph.get(),
		32,
		32 );

	UpdateWindow( hWnd );
	ShowWindow( hWnd,
		SW_SHOWNORMAL );

	// setting a timer for fun
	DWORD ms = 200;
	g_pTimerEvent = SetTimer( hWnd,
		IDT_TIMER1,
		ms,
		nullptr );
		//(TIMERPROC) timerProc );


	srand( time( nullptr ) );

	int exitCode = loop( hWnd );

	KeyConsole& console = KeyConsole::getInstance();
	console.resetInstance();
#if defined _DEBUG && !defined NDEBUG
	while ( !getchar() );
#endif
	return exitCode;
}

LRESULT CALLBACK WindowProc( _In_ HWND hWnd,
	_In_ UINT uMsg,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam )
{
	switch ( uMsg )
	{
	case WM_DESTROY:
	{
		PostQuitMessage( 0 );
		return 0;
	}
	case WM_TIMER:
	{
		switch ( wParam )
		{
			case IDT_TIMER1:
			{
				doTiming();
				KeyConsole& console = KeyConsole::getInstance();
				console.print( "dt = " + std::to_string( g_deltaTime ) + "\n" );
				console.print( "Time elapsed = " + std::to_string( g_elapsedTime ) + "\n" );

				update( hWnd );
				break;
			}
			case IDT_TIMER2:
			{
				KeyConsole& console = KeyConsole::getInstance();
				console.print( "Timer 2 responded.\n" );
				break;
			}
		}
		break;
	}
	case WM_PAINT:
	{
#ifdef USE_GDIPLUS
		PAINTSTRUCT ps;
		RECT rect;

		GetClientRect( hWnd,
			&rect );

		if ( rect.bottom == 0 )
		{
			return -1;
		}

		HDC dc = BeginPaint( hWnd,
			&ps );

		COLORREF magenta = RGB( 255, math::toDecimal( 0x99 ), 255  );
		SetPixel( dc, 20, 20, magenta ); SetPixel( dc, 21, 20, magenta ); SetPixel( dc, 22, 20, magenta ); SetPixel( dc, 23, 20, magenta );
		SetPixel( dc, 20, 21, magenta ); SetPixel( dc, 21, 21, magenta ); SetPixel( dc, 22, 21, magenta ); SetPixel( dc, 23, 21, magenta );
		SetPixel( dc, 20, 22, magenta ); SetPixel( dc, 21, 22, magenta ); SetPixel( dc, 22, 22, magenta ); SetPixel( dc, 23, 22, magenta );
		SetPixel( dc, 20, 23, magenta ); SetPixel( dc, 21, 23, magenta ); SetPixel( dc, 22, 23, magenta ); SetPixel( dc, 23, 23, magenta );

		EndPaint( hWnd,
			&ps);
		
		/*
		COLORREF *arr = (COLORREF*) calloc(512*512, sizeof(COLORREF));
		// Filling array here
		
		// Creating memory bitmap
		HBITMAP map = CreateBitmap(512 // width. 512 in my case
								512, // height
								1, // Color Planes, unfortanutelly don't know what is it actually. Let it be 1
								8*4, // Size of memory for one pixel in bits (in win32 4 bytes = 4*8 bits)
								(void*) arr); // pointer to array
		// memory HDC to copy picture
		HDC src = CreateCompatibleDC(hdc); // hdc - Device context for window, I've got earlier with GetDC(hWnd) or GetDC(NULL);
		SelectObject(src, map); // Inserting picture into our temp HDC
		// Copy image from temp HDC to window
		BitBlt(hdc, // Destination
			   10,  // x and
			   10,  // y - upper-left corner of place, where we'd like to copy
			   512, // width of the region
			   512, // height
			   src, // source
			   0,   // x and
			   0,   // y of upper left corner  of part of the source, from where we'd like to copy
			   SRCCOPY); // Defined DWORD to juct copy pixels. Watch more on msdn;
		
		DeleteDC(src); // Deleting memory HDC
		*/
#else
		// Direct2D drawing
		g_gph->beginDraw();
		g_gph->clear( D2D1::ColorF{1.0f, 0.8f, 1.0f} );
		for ( int i = 0; i < 10; ++i )
		{
			g_gph->drawCircle( rand() % 800,
				rand() % 600,
				rand() % 100,
				D2D1::ColorF(rand01, rand01, rand01, rand01 / 1.75 + 0.25f),
				static_cast<int>( ceilf( rand() % 8 ) ) );
		}

		// text
		g_gph->createTextFormat( L"Zrnic" );
		const auto rect = D2D1_RECT_F{100, 100 + y, 200, 120 + y};
		g_gph->drawText( L"KeyC0de",
			rect,
			D2D1::ColorF{0, 111 / 256.f, 0} );

		// image
		const auto ba2Rect = D2D1_RECT_F{0, 0, g_ba2->getWidth(), g_ba2->getWidth()};
		g_ba2->render( g_gph.get(),
			ba2Rect,
			ba2Rect,
			.6f );

		g_player->render( g_gph.get(),
			g_spriteOrder[g_currentSprite],
			300,
			100 );

		g_gph->endDraw();
#endif
		break;
	}
	default:
		break;
	}

	return DefWindowProcW( hWnd,
		uMsg,
		wParam,
		lParam );
}

std::pair<int,int> parseCommandLineArguments()
{
	HRESULT hres;
	LPWSTR commandLine = GetCommandLineW();
	int argc;
	LPWSTR* argv = CommandLineToArgvW( commandLine,
		&argc );
	ASSERT_HRES_WIN32_IF_FAILED( hres );

	wchar_t* end;
	int width = std::wcstol( argv[1], &end, 10 );
	int height = std::wcstol( argv[2], &end, 10 );
	return {width, height};
}

int messageProcessingLoop() noexcept
{
	MSG msg;
	while ( PeekMessageW( &msg, nullptr, 0u, 0u, PM_REMOVE ) )
	{
		// check for quit because peekmessage does not signal this via return val
		if ( msg.message == WM_QUIT || msg.message == WM_DESTROY )
		{
			return (int)msg.wParam;
		}
		TranslateMessage( &msg );
		DispatchMessageW( &msg );
	}
	return INT_MIN;
}

void update( HWND hWnd )
{
	ySpeed += .1f * math::signum( ySpeed );
	y += ySpeed;
	if ( y > g_height - 120 - 1 || y < -100 )
	{
		ySpeed = fabs( ySpeed ) * math::signum( ySpeed ) * -1.0f ;
	}

	g_currentSprite = ( g_currentSprite + 1 ) % ( g_spriteOrder.size() );

	RECT windowRect;
	GetWindowRect( hWnd,
		&windowRect );
	RedrawWindow( hWnd,
		&windowRect,
		nullptr,
		RDW_INTERNALPAINT | RDW_ALLCHILDREN );
}

// A winapi Timer doesn't work on a different thread
void CALLBACK timerProc( HWND hWnd,
	UINT uMsg,
	UINT idEvent,
	DWORD time )
{
	if ( g_pTimerEvent == idEvent )
	{
		KeyConsole& console = KeyConsole::getInstance();
		console.print( "Timer about to be killed!" );
		KillTimer( hWnd,
			idEvent );
	}
}

void doTiming()
{
	static LARGE_INTEGER current;
	static LARGE_INTEGER last;
	QueryPerformanceCounter( &current );
	const auto fFreq = static_cast<float>( g_freq.QuadPart );
	g_deltaTime = ( current.QuadPart - last.QuadPart ) / fFreq;
	g_elapsedTime = ( current.QuadPart - g_initialTime.QuadPart ) / fFreq;
	last = current;
}

int loop( HWND hWnd )
{
	while ( true )
	{
		if ( messageProcessingLoop() != INT_MIN )
		{
			KillTimer( hWnd,
				g_pTimerEvent );
			return EXIT_SUCCESS;
		}
	}
}
