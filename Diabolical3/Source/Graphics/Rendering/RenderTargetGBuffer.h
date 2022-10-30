#pragma once
#include "RenderTarget.h"

class DRenderTargetGBuffer : public DRenderTarget
{
public:
	DRenderTargetGBuffer()
	{}

	DRenderTargetGBuffer(uint32_t InWidth, uint32_t InHeight);

	virtual ~DRenderTargetGBuffer()
	{
		if (AlbedoTexture)
		{
			LOG_ERR("Unimplemented DRenderTargetGBuffer Destructor");
			Check(false);
		}
	}

	virtual bool Bind(bool bClearBuffer = true) override;

	virtual void DrawToScreen() const override;

	virtual uint32_t GetWidth() const override
	{
		return Width;
	}

	virtual uint32_t GetHeight() const override
	{
		return Height;
	}

	virtual bool IsValid() const override
	{
		return bIsValid;
	}

	virtual uint32_t GetAlbedoTexture() const
	{
		return AlbedoTexture;
	}

	virtual uint32_t GetPositionTexture() const
	{
		return PositionTexture;
	}

	virtual uint32_t GetNormalTexture() const
	{
		return NormalTexture;
	}

	virtual uint32_t GetMaterialPropertiesTexture() const
	{
		return MaterialPropertiesTexture;
	}

protected:
	uint32_t Width = 0;
	uint32_t Height = 0;

	GLuint FramebufferName = 0;
	GLuint AlbedoTexture = 0;
	GLuint PositionTexture = 0;
	GLuint NormalTexture = 0;
	GLuint MaterialPropertiesTexture = 0;
	GLuint DepthRenderBuffer = 0;

	bool bIsValid = false;
};
