#pragma once

#include <wincodec.h>
#include "graphics_2d.h"
#include "key_wrl.h"


// https://docs.microsoft.com/en-us/windows/win32/wic/jpeg-ycbcr-support
class Sprite
{
	Microsoft::WRL::ComPtr<IWICImagingFactory> m_pWicFactory;
	Microsoft::WRL::ComPtr<IWICBitmapDecoder> m_pWicDecoder;
	Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> m_pBitmapFrame;
	Microsoft::WRL::ComPtr<IWICFormatConverter> m_pWicConverter;
	Microsoft::WRL::ComPtr<ID2D1Bitmap> m_pBitmap;
public:
	Sprite( const std::wstring& filename, Graphics2D* gph );
	virtual ~Sprite();
	Sprite( const Sprite& rhs ) = delete;
	Sprite& operator=( const Sprite& rhs ) = delete;

	// draw the bitmap to the render target
	void render( Graphics2D* gph, const D2D1_RECT_F& srcRect, const D2D1_RECT_F& destRect,
		float alpha = 1.0f );

	ID2D1Bitmap* getBitmap() const noexcept;
	float getWidth() const noexcept;
	float getHeight() const noexcept;
};

class SpriteSheet
	: public Sprite
{
	int m_spriteWidth;
	int m_spriteHeight;
	int m_nSpritesAcross;
public:
	SpriteSheet( const std::wstring& filename, Graphics2D* gph, int spriteWidth,
		int spriteHeight );

	void render( Graphics2D* gph, int index, int x, int y, float alpha = 1.0f );
};