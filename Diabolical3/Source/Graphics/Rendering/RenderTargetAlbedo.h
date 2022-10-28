#pragma once
#include "RenderTarget.h"

class DRenderTargetAlbedo : public DRenderTarget
{
public:
	DRenderTargetAlbedo()
	{}

	DRenderTargetAlbedo(uint32_t InWidth, uint32_t InHeight);

	virtual ~DRenderTargetAlbedo()
	{
		if (AlbedoTexture)
		{
			LOG_ERR("Unimplemented DRenderTargetAlbedo Destructor");
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

protected:
	uint32_t Width = 0;
	uint32_t Height = 0;

	GLuint FramebufferName = 0;
	GLuint AlbedoTexture = 0;

	bool bIsValid = false;
};
