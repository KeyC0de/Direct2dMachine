#include "graphics_2d.h"
#include "os_utils.h"
#include "assertions_console.h"

#pragma comment( lib, "d2d1.lib" )
#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "dxgi.lib" )
#pragma comment( lib, "d3dcompiler.lib" )
#pragma comment( lib, "dwrite.lib" )


namespace mwrl = Microsoft::WRL;

Graphics2D::Graphics2D( HWND hWnd )
{
	D2D1_FACTORY_OPTIONS d2dOptions{};

#if defined _DEBUG && !defined NDEBUG
	d2dOptions.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif
	HRESULT hres = D2D1CreateFactory( D2D1_FACTORY_TYPE_SINGLE_THREADED,
		__uuidof( ID2D1Factory1 ),
		&d2dOptions,
		&m_pFactory );
	ASSERT_HRES_IF_FAILED;

	RECT rect;
	GetClientRect( hWnd,
		&rect );
	auto size = D2D1::SizeU( rect.right - rect.left, rect.bottom - rect.top );
	hres = m_pFactory->CreateHwndRenderTarget( D2D1::RenderTargetProperties( D2D1_RENDER_TARGET_TYPE_HARDWARE,
			D2D1::PixelFormat( DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED ) ),
		D2D1::HwndRenderTargetProperties( hWnd,
			size ),
		&m_pRenderTarget );

	m_pRenderTarget->QueryInterface( __uuidof( ID2D1DeviceContext ),
		reinterpret_cast<void**>( m_pDeviceContext.GetAddressOf() ) );
	ASSERT( m_pDeviceContext,
		"Device Context is null!" );

	//check with device context->IsDxgiFormatSupported( DXGI_FORMAT_B8G8R8A8_UNORM );
	hres = DWriteCreateFactory( DWRITE_FACTORY_TYPE_SHARED,
		__uuidof( IDWriteFactory ),
		reinterpret_cast<IUnknown**>( m_pWriteFactory.GetAddressOf() ) );
	ASSERT_HRES_IF_FAILED;
}

Graphics2D::Graphics2D( Graphics2D&& rhs ) noexcept
	:
	m_pFactory{rhs.m_pFactory.Get()},
	m_pRenderTarget{rhs.m_pRenderTarget.Get()},
	m_pDeviceContext{rhs.m_pDeviceContext.Get()},
	m_pBrush{rhs.m_pBrush.Get()},
	m_pWriteFactory{rhs.m_pWriteFactory.Get()},
	m_pTextFormat{rhs.m_pTextFormat.Get()}
{
	rhs.m_pFactory.Reset();
	rhs.m_pRenderTarget.Reset();
	rhs.m_pDeviceContext.Reset();
	rhs.m_pBrush.Reset();
	rhs.m_pWriteFactory.Reset();
	rhs.m_pTextFormat.Reset();
}

Graphics2D& Graphics2D::operator=( Graphics2D&& rhs ) noexcept
{
	Graphics2D tmp{std::move( rhs )};
	rhs.m_pFactory.Reset();
	rhs.m_pRenderTarget.Reset();
	rhs.m_pDeviceContext.Reset();
	rhs.m_pBrush.Reset();
	rhs.m_pWriteFactory.Reset();
	rhs.m_pTextFormat.Reset();
	return tmp;
}

void Graphics2D::beginDraw()
{
	m_pRenderTarget->BeginDraw();
}

void Graphics2D::endDraw()
{
	m_pRenderTarget->EndDraw();
}

void Graphics2D::clear( const D2D1::ColorF& rgb )
{
	m_pRenderTarget->Clear( rgb );
}

void Graphics2D::createTextFormat( const std::wstring& fontName,
	float fontSize,
	const std::wstring& fontLocale )
{
	HRESULT hres = m_pWriteFactory->CreateTextFormat( fontName.c_str(),
		nullptr,
		DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STYLE_NORMAL,
		DWRITE_FONT_STRETCH_NORMAL,
		fontSize,
		fontLocale.c_str(),
		&m_pTextFormat );
	ASSERT_HRES_IF_FAILED;
}

void Graphics2D::drawLine( const D2D1_POINT_2F& v0,
	const D2D1_POINT_2F& v1,
	const D2D1::ColorF& rgba,
	int strokeWidth )
{
	m_pRenderTarget->CreateSolidColorBrush( rgba,
		&m_pBrush );
	m_pRenderTarget->DrawLine( v0,
		v1,
		m_pBrush.Get(),
		strokeWidth	);
}

void Graphics2D::drawRect( const D2D1_RECT_F& rect,
	const D2D1::ColorF& rgba,
	int strokeWidth )
{
	m_pRenderTarget->CreateSolidColorBrush( rgba,
		&m_pBrush );
	m_pRenderTarget->DrawRectangle( rect,
		m_pBrush.Get(),
		strokeWidth );
}

void Graphics2D::drawRoundedRect( const D2D1_RECT_F& rect,
	float radiusX,
	float radiusY,
	const D2D1::ColorF& rgba,
	int strokeWidth  )
{
	m_pRenderTarget->CreateSolidColorBrush( rgba,
		&m_pBrush );
	D2D1_ROUNDED_RECT roundRect{rect, radiusX, radiusY};
	m_pRenderTarget->DrawRoundedRectangle( &roundRect,
		m_pBrush.Get(),
		strokeWidth );
}

void Graphics2D::drawEllipse( float x,
	float y,
	float hRadius,
	float vRadius,
	const D2D1::ColorF& rgba,
	int strokeWidth )
{
	m_pRenderTarget->CreateSolidColorBrush( rgba,
		&m_pBrush );
	m_pRenderTarget->DrawEllipse( D2D1::Ellipse( D2D1::Point2F(x, y),
			hRadius,
			vRadius ),
		m_pBrush.Get(),
		strokeWidth );
}

void Graphics2D::drawCircle( float x,
	float y,
	float radius,
	const D2D1::ColorF& rgba,
	int strokeWidth )
{
	drawEllipse( x,
		y,
		radius,
		radius,
		rgba,
		strokeWidth );
}

void Graphics2D::drawText( const std::wstring& txt,
	const D2D1_RECT_F& rect,
	const D2D1::ColorF& rgba )
{
	m_pRenderTarget->CreateSolidColorBrush( rgba,
		&m_pBrush );
	m_pRenderTarget->DrawText( txt.data(),
		txt.length(),
		m_pTextFormat.Get(),
		rect,
		m_pBrush.Get() );
}

ID2D1HwndRenderTarget* Graphics2D::getRenderTarget() const noexcept
{
	return m_pRenderTarget.Get();
}