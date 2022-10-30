#include "RenderTargetGBuffer.h"

DRenderTargetGBuffer::DRenderTargetGBuffer(uint32_t InWidth, uint32_t InHeight)
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

			{ // Position
				glActiveTexture(GL_TEXTURE1);
				glGenTextures(1, &PositionTexture);
				glBindTexture(GL_TEXTURE_2D, PositionTexture);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Width, Height, 0, GL_RGBA, GL_FLOAT, 0);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, PositionTexture, 0);
			}

			{ // Normal
				glActiveTexture(GL_TEXTURE2);
				glGenTextures(1, &NormalTexture);
				glBindTexture(GL_TEXTURE_2D, NormalTexture);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Width, Height, 0, GL_RGBA, GL_FLOAT, 0);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, NormalTexture, 0);
			}

			{ // Material Properties
				glActiveTexture(GL_TEXTURE3);
				glGenTextures(1, &MaterialPropertiesTexture);
				glBindTexture(GL_TEXTURE_2D, MaterialPropertiesTexture);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, Width, Height, 0, GL_RGBA, GL_FLOAT, 0);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, MaterialPropertiesTexture, 0);
			}

			glGenRenderbuffers(1, &DepthRenderBuffer);
			glBindRenderbuffer(GL_RENDERBUFFER, DepthRenderBuffer);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, Width, Height); 
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, DepthRenderBuffer);

			GLenum DrawBuffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
			glDrawBuffers(4, DrawBuffers);

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

bool DRenderTargetGBuffer::Bind(bool bClearBuffer)
{
	if (!FramebufferName || !AlbedoTexture || !DepthRenderBuffer || !Width || !Height || !bIsValid)
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

void DRenderTargetGBuffer::DrawToScreen() const
{
	DSharedPtr<DMaterialInstance> MaterialInstance = new DMaterialInstance(DMaterial::GetQuadCopyMaterial());
	DEngine::RenderThread->Invoke(DAction<int>([=](int a)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, AlbedoTexture);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, PositionTexture);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, NormalTexture);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glViewport(0, 0, Width, Height);

			MaterialInstance->SetUniform("albedoTexture", 0);
			MaterialInstance->SetUniform("positionTexture", 1);
			MaterialInstance->SetUniform("normalTexture", 2);

			MaterialInstance->Bind();
			MeshPrimitives::Quad->Draw();
		}));
}