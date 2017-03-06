#pragma once


namespace D2DBitmap
{
	template<typename D2DRTV = ID2D1HwndRenderTarget, typename Rect = D2D_RECT_U>
	inline bool LoadImageFromFile(	  IWICImagingFactory2		*pwicFactory
									, D2DRTV					*pd2dRenderTarget

									, LPCTSTR					pszstrFileName
									, ID2D1Bitmap1				**ppd2dBitmap
									, Rect						*pd2drcImage  = nullptr
									, UINT						nWidth		  = 0
									, UINT						nHeight		  = 0
									, WICBitmapTransformOptions	nFlipRotation = WICBitmapTransformRotate0
	)
	{
		ComPtr<IWICBitmapDecoder> pwicBitmapDecoder;

		if (FAILED(pwicFactory->CreateDecoderFromFilename(	  pszstrFileName
															, NULL
															, GENERIC_READ
															, WICDecodeMetadataCacheOnDemand
															, &pwicBitmapDecoder
		))) return false;
		
		ComPtr<IWICBitmapFrameDecode> pwicBitmapFrameDecode;
		if (FAILED(pwicBitmapDecoder->GetFrame(0, &pwicBitmapFrameDecode))) return false;

		ComPtr<IWICBitmapSource> pwicSource{ pwicBitmapFrameDecode };

		UINT nImageWidth	{ 0 };
		UINT nImageHeight	{ 0 };

		if (FAILED(pwicSource->GetSize(&nImageWidth, &nImageHeight))) return false;

		ComPtr<IWICFormatConverter>		pwicFormatConverter;
		ComPtr<IWICBitmapScaler>		pwicScaler;
		ComPtr<IWICBitmapClipper>		pwicClipper;
		ComPtr<IWICBitmapFlipRotator>	pwicFlipRotator;

		if (pd2drcImage)
		{
			if (pd2drcImage->left < 0)	pd2drcImage->left = 0;
			if (pd2drcImage->top < 0)	pd2drcImage->top = 0;
			if (pd2drcImage->right > nImageWidth)	pd2drcImage->right = nImageWidth;
			if (pd2drcImage->bottom > nImageHeight)	pd2drcImage->bottom = nImageHeight;

			WICRect wicRect
			{ 
				  static_cast<INT>(pd2drcImage->left)
				, static_cast<INT>(pd2drcImage->top)
				, static_cast<INT>(pd2drcImage->right - pd2drcImage->left)
				, static_cast<INT>(pd2drcImage->bottom - pd2drcImage->top) 
			};

			if (FAILED(pwicFactory->CreateBitmapClipper(&pwicClipper)))			return false;
			if (FAILED(pwicClipper->Initialize(pwicSource.Get(), &wicRect)))	return false;

			pwicSource = pwicClipper;
		}

		if ((nWidth != 0) || (nHeight != 0))
		{
			if (nWidth == 0) nWidth = UINT(static_cast<float>(nHeight) / static_cast<float>(nImageHeight) * static_cast<float>(nImageWidth));
			if (nHeight == 0) nHeight = UINT(static_cast<float>(nWidth) / static_cast<float>(nImageWidth) * static_cast<float>(nImageHeight));

			if (FAILED(pwicFactory->CreateBitmapScaler(&pwicScaler))) return false;
			if (FAILED(pwicScaler->Initialize(pwicSource.Get(), nWidth, nHeight, WICBitmapInterpolationModeCubic))) return false;
			pwicSource = pwicScaler;
		}

		if (nFlipRotation != WICBitmapTransformRotate0)
		{
			if (FAILED(pwicFactory->CreateBitmapFlipRotator(&pwicFlipRotator)))			return false;
			if (FAILED(pwicFlipRotator->Initialize(pwicSource.Get(), nFlipRotation)))	return false;
			pwicSource = pwicFlipRotator;
		}

		if (FAILED(pwicFactory->CreateFormatConverter(	  &pwicFormatConverter
		))) return false;

		if (FAILED(pwicFormatConverter->Initialize(	  pwicSource.Get()
													, GUID_WICPixelFormat32bppPBGRA
													, WICBitmapDitherTypeNone
													, NULL
													, 0.f
													, WICBitmapPaletteTypeMedianCut
		))) return false;
		
		if (FAILED(pd2dRenderTarget->CreateBitmapFromWicBitmap(	  pwicFormatConverter.Get()
																, NULL
																, ppd2dBitmap
		))) return false;
		
		return true;
	}

}