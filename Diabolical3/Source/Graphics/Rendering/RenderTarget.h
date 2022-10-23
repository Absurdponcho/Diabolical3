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

	DRenderTarget(uint32_t InWidth, uint32_t InHeight)
	 : Width(InWidth), Height(InHeight), bIsValid(true)
	{
		LOG(DString::Format("Generating Render Target Width %i, Height %i", Width, Height));

		DEngine::RenderThread->InvokeImmediately(DAction<int>([&](int a)
			{
				glGenFramebuffers(1, &FramebufferName);
				glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

				glGenTextures(1, &RenderTexture);
				glBindTexture(GL_TEXTURE_2D, RenderTexture);

				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

				glGenRenderbuffers(1, &DepthRenderBuffer);
				glBindRenderbuffer(GL_RENDERBUFFER, DepthRenderBuffer);
				glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, Width, Height);
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, DepthRenderBuffer);

				glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, RenderTexture, 0);

				GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
				glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

				if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				{
					Check(false);
					return;
				}

				glBindFramebuffer(GL_FRAMEBUFFER, 0);
				glBindTexture(GL_TEXTURE_2D, 0);
				glBindRenderbuffer(GL_RENDERBUFFER, 0);

			}));
	}

	bool Bind()
	{
		if (!FramebufferName || !RenderTexture || !DepthRenderBuffer || !Width || !Height || !bIsValid)
		{
			Checkf(false, "Render target is not configured properly.");
			return false;
		}
		DEngine::RenderThread->Invoke(DAction<int>([=](int a)
			{
				glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
				glViewport(0, 0, Width, Height);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			}));

		return true;
	}

	virtual ~DRenderTarget()
	{
		if (RenderTexture)
		{
			LOG_ERR("Unimplemented DRenderTarget Destructor");
			Check(false);
		}
	}

	void DrawToScreen() const
	{
		DSharedPtr<DMaterialInstance> MaterialInstance = new DMaterialInstance(DMaterial::GetQuadCopyMaterial());
		DEngine::RenderThread->Invoke(DAction<int>([=](int a)
			{ 
				glBindTexture(GL_TEXTURE_2D, RenderTexture);
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
				glViewport(0, 0, Width, Height);

				MaterialInstance->Bind();

				MeshPrimitives::Quad->Draw();
			}));
	}

	uint32_t GetWidth() const
	{
		return Width;
	}

	uint32_t GetHeight() const
	{
		return Height;
	}

protected:
	uint32_t Width = 0;
	uint32_t Height = 0;

	GLuint FramebufferName = 0;
	GLuint RenderTexture = 0;
	GLuint DepthRenderBuffer = 0;

	bool bIsValid = false;
};