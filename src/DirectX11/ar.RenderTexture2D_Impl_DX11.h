
#pragma once

#include "ar.DX11.h"

#include "../ar.Base.h"

namespace ar
{

class RenderTexture2D_Impl_DX11
	: public RenderTexture2D
{
private:
	ID3D11Texture2D*			texture = nullptr;
	ID3D11ShaderResourceView*	textureSRV = nullptr;
	ID3D11RenderTargetView*		textureRTV = nullptr;

public:
	RenderTexture2D_Impl_DX11();

	virtual ~RenderTexture2D_Impl_DX11();

	bool Initialize(Manager* manager, int32_t width, int32_t height, TextureFormat format);
};

}