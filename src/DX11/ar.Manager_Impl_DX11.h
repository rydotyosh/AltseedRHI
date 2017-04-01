﻿
#pragma once

#include "ar.DX11.h"

#include "../ar.Base.h"

namespace ar
{
	class Manager_Impl_DX11
		: public Manager
	{
		ID3D11Device*			device = nullptr;
		ID3D11DeviceContext*	context = nullptr;
		IDXGIFactory1*		dxgiFactory = nullptr;
		IDXGIAdapter1*		adapter = nullptr;
		IDXGISwapChain*		swapChain = nullptr;

		ID3D11Texture2D*			defaultBack = nullptr;
		ID3D11RenderTargetView*		defaultBackRenderTargetView = nullptr;

	public:
		Manager_Impl_DX11();
		virtual ~Manager_Impl_DX11();

		ErrorCode Initialize(const ManagerInitializationParameter& param) override;

		void BeginScene(const SceneParameter& param) override;

		void EndScene() override;

		void Present() override;

		ID3D11Device* GetDevice() const { return device; }

		ID3D11DeviceContext* GetContext() { return context; }
	};
}