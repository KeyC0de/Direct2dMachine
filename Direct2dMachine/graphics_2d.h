#pragma once

#include "winner.h"
#include <d2d1_1.h>
#include <d3dcompiler.h>
#include "key_wrl.h"
#include <dwrite.h>
#include <string>


class Graphics2D
{
	Microsoft::WRL::ComPtr<ID2D1Factory> m_pFactory;
	Microsoft::WRL::ComPtr<ID2D1HwndRenderTarget> m_pRenderTarget;
	Microsoft::WRL::ComPtr<ID2D1DeviceContext> m_pDeviceContext;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> m_pBrush;
	Microsoft::WRL::ComPtr<IDWriteFactory> m_pWriteFactory;
	Microsoft::WRL::ComPtr<IDWriteTextFormat> m_pTextFormat;
public:
	Graphics2D( HWND hWnd );
	Graphics2D( const Graphics2D& rhs ) = delete;
	Graphics2D& operator=( const Graphics2D& rhs ) = delete;
	Graphics2D( Graphics2D&& rhs ) noexcept;
	Graphics2D& operator=( Graphics2D&& rhs ) noexcept;

	void beginDraw();
	void endDraw();
	void clear( const D2D1::ColorF& rgb );
	void createTextFormat( const std::wstring& fontName = L"Arial", float fontSize = 24.0f,
		const std::wstring& fontLocale = L"en-us" );
	void drawLine( const D2D1_POINT_2F& v0, const D2D1_POINT_2F& v1,
		const D2D1::ColorF& rgba, int strokeWidth = 1 );
	void drawRect( const D2D1_RECT_F& rect, const D2D1::ColorF& rgba,
		int strokeWidth = 1 );
	void drawRoundedRect( const D2D1_RECT_F& rect, float radiusX, float radiusY,
		const D2D1::ColorF& rgba, int strokeWidth = 1 );
	void drawEllipse( float x, float y, float hRadius, float vRadius,
		const D2D1::ColorF& rgba, int strokeWidth = 1 );
	void drawCircle( float x, float y, float radius, const D2D1::ColorF& rgba,
		int strokeWidth = 1 );
	void drawText( const std::wstring& txt, const D2D1_RECT_F& rect,
		const D2D1::ColorF& rgba );
	ID2D1HwndRenderTarget* getRenderTarget() const noexcept;
};