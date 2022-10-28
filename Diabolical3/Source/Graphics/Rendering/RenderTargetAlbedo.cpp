#include "RenderTargetAlbedo.h"


DRenderTargetAlbedo::DRenderTargetAlbedo(uint32_t InWidth, uint32_t InHeight)
	: Width(InWidth), Height(InHeight), bIsValid(true)
{
	LOG(DString::Format("Generating Render Target Width %i, Height %i", Width, Height));

	DEngine::RenderThread->InvokeImmediately(DAction<int>([&](int a)
		{
			glGenFramebuffers(1, &FramebufferName);
			glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

			{ // Albedo
				glActiveTexture(GL_TEXTURE0);
				glGenTextures(1, &AlbedoTexture);
				glBindTexture(GL_TEXTURE_2D, AlbedoTexture);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, AlbedoTexture, 0);
			}

			GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
			glDrawBuffers(1, DrawBuffers);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			{
				Check(false);
				return;
			}

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glBindTexture(GL_TEXTURE_2D, 0);

		}));
}

bool DRenderTargetAlbedo::Bind(bool bClearBuffer)
{
	if (!FramebufferName || !AlbedoTexture || !Width || !Height || !bIsValid)
	{
		Checkf(false, "Render target is not configured properly.");
		return false;
	}
	DEngine::RenderThread->Invoke(DAction<int>([=](int a)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
			glViewport(0, 0, Width, Height);
			if (bClearBuffer)
			{
				glClearColor(0.0, 0.0, 0.0, 1.0);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			}

		}));

	return true;
}

void DRenderTargetAlbedo::DrawToScreen() const
{
	DSharedPtr<DMaterialInstance> MaterialInstance = new DMaterialInstance(DMaterial::GetQuadCopyMaterial());
	DEngine::RenderThread->Invoke(DAction<int>([=](int a)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, AlbedoTexture);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glViewport(0, 0, Width, Height);

			MaterialInstance->SetUniform("albedoTexture", 0);

			MaterialInstance->Bind();
			MeshPrimitives::Quad->Draw();
		}));
}