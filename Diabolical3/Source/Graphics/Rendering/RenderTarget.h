#pragma once
#include "gl/glew.h"
#include <gl/GL.h>
#include "Check.h"
#include "RenderThread.h"
#include "DiabolicalEngine.h"
#include "World/Object.h"
#include "Logging/Logging.h"
#include "Types/DString.h"
#include "Material.h"
#include "MaterialInstance.h"
#include "MeshPrimitives.h"

class DRenderTarget
{
public:

	DRenderTarget()
	{}

	virtual bool Bind()
	{
		return false;
	}

	virtual bool IsValid() const
	{
		return false;
	}

	virtual ~DRenderTarget()
	{
	}

	virtual void DrawToScreen() const
	{
	}

	virtual uint32_t GetWidth() const
	{
		return 0;
	}

	virtual uint32_t GetHeight() const
	{
		return 0;
	}

protected:
};