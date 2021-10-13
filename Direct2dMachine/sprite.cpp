#include "sprite.h"
#include "os_utils.h"

#pragma comment( lib, "windowscodecs.lib" )


namespace mwrl = Microsoft::WRL;

Sprite::Sprite( const std::wstring& filename,
	Graphics2D* gph )
{
	// create the wic factory
	HRESULT hres = CoCreateInstance( CLSID_WICImagingFactory,
		nullptr,
		CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS( &m_pWicFactory ) );
	ASSERT_HRES_IF_FAILED;

	// create the decoder to decode the image files
	hres = m_pWicFactory->CreateDecoderFromFilename( filename.c_str(),
		nullptr,
		GENERIC_READ,
		WICDecodeMetadataCacheOnLoad,
		&m_pWicDecoder );
	ASSERT_HRES_IF_FAILED;

	// get frame from image
	hres = m_pWicDecoder->GetFrame( 0,
		&m_pBitmapFrame );
	ASSERT_HRES_IF_FAILED;

	// create wic format converter
	hres = m_pWicFactory->CreateFormatConverter( &m_pWicConverter );
	ASSERT_HRES_IF_FAILED;

	// setup the converter to create 32bpp BGRA bitmaps
	hres = m_pWicConverter->Initialize( m_pBitmapFrame.Get(),
		GUID_WICPixelFormat32bppPBGRA,	// Pixel Format
		WICBitmapDitherTypeNone,
		nullptr,
		0.0,
		WICBitmapPaletteTypeCustom );
	ASSERT_HRES_IF_FAILED;

	// create the D2D1 bitmap
	hres = gph->getRenderTarget()->CreateBitmapFromWicBitmap( m_pWicConverter.Get(),
		nullptr,
		&m_pBitmap );
	ASSERT_HRES_IF_FAILED;
}

Sprite::~Sprite()
{
}

void Sprite::render( Graphics2D* gph,
	const D2D1_RECT_F& srcRect,
	const D2D1_RECT_F& destRect,
	float alpha )
{
	gph->getRenderTarget()->DrawBitmap( m_pBitmap.Get(),
		destRect,
		alpha,
		D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
		srcRect );
}

ID2D1Bitmap* Sprite::getBitmap() const noexcept
{
	return m_pBitmap.Get();
}

float Sprite::getWidth() const noexcept
{
	return m_pBitmap->GetSize().width;
}

float Sprite::getHeight() const noexcept
{
	return m_pBitmap->GetSize().height;
}

/*
HRESULT SaveBitmapToStream(
	_In_ CComPtr<ID2D1Bitmap1> d2dBitmap,
	_In_ CComPtr<IWICImagingFactory2> wicFactory2,
	_In_ CComPtr<ID2D1DeviceContext> d2dContext,
	_In_ REFGUID wicFormat,
	_In_ IStream* stream
)
{
	// Create and initialize WIC Bitmap Encoder.
	CComPtr<IWICBitmapEncoder> wicBitmapEncoder;
	auto hr =
		wicFactory2->CreateEncoder(
			wicFormat,
			nullptr,    // No preferred codec vendor.
			&wicBitmapEncoder
		);

	hr =
		wicBitmapEncoder->Initialize(
			stream,
			WICBitmapEncoderNoCache
		);

// Create and initialize WIC Frame Encoder.
	CComPtr<IWICBitmapFrameEncode> wicFrameEncode;
	hr =
		wicBitmapEncoder->CreateNewFrame(
			&wicFrameEncode,
			nullptr     // No encoder options. use EnableV5Header32bppBGRA
		);

	if (FAILED(hr))
		return hr;
	hr = 
		wicFrameEncode->Initialize(nullptr);
	if (FAILED(hr))
		return hr;

// Retrieve D2D Device.
	CComPtr<ID2D1Device> d2dDevice;
	if (!d2dContext)
		return E_FAIL;
	d2dContext->GetDevice(&d2dDevice);

	// Create IWICImageEncoder.
	CComPtr<IWICImageEncoder> imageEncoder;
	hr = 
		wicFactory2->CreateImageEncoder(
			d2dDevice,
			&imageEncoder
		);
	if (FAILED(hr))
		return hr;

	hr =        
		imageEncoder->WriteFrame(
			d2dBitmap,  wicFrameEncode,
			nullptr     // Use default WICImageParameter options.
		);
	if (FAILED(hr))
		return hr;

	hr = wicFrameEncode->Commit();

	if (FAILED(hr))
		return hr;

	hr = wicBitmapEncoder->Commit();

	if (FAILED(hr))
		return hr;
// Flush all memory buffers to the next-level storage object.
	hr =
		stream->Commit(STGC_DEFAULT);

	return hr;

}
*/

SpriteSheet::SpriteSheet( const std::wstring& filename,
	Graphics2D* gph,
	int spriteWidth,
	int spriteHeight )
	:
	Sprite{filename, gph}
{
	m_spriteWidth = spriteWidth;
	m_spriteHeight = spriteHeight;
	m_nSpritesAcross = getWidth() / spriteWidth;
}

void SpriteSheet::render( Graphics2D* gph,
	int index,
	int x,
	int y,
	float alpha )
{
	float srcRectX = (float)( index % m_nSpritesAcross ) * m_spriteWidth;
	float srcRectY = (float)( index / m_nSpritesAcross ) * m_spriteHeight;
	D2D1_RECT_F srcRect = D2D1::RectF( srcRectX,
		srcRectY,
		srcRectX + m_spriteWidth,
		srcRectY + m_spriteHeight );
	D2D1_RECT_F destRect = D2D1::RectF( x,
		y,
		x + m_spriteWidth,
		y + m_spriteHeight );

	Sprite::render( gph,
		srcRect,
		destRect,
		alpha );
}
