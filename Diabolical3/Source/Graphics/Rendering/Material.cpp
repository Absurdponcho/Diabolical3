#include "Material.h"
#include "Logging/Logging.h"
#include "AssetManager/AssetManager.h"
#include "MaterialInstance.h"
#include "Graphics/Rendering/RenderThread.h"
#include "DiabolicalEngine.h"

DSharedPtr<DMaterial> DMaterial::DefaultMaterial = nullptr;
DSharedPtr<DMaterial> DMaterial::QuadCopyMaterial = nullptr;

DSharedPtr<DMaterial> DMaterial::GetDefaultMaterial()
{
	static bool bInitializedDefaultMaterial = false;
	if (!bInitializedDefaultMaterial)
	{
		InitializeDefaultMaterial();
		bInitializedDefaultMaterial = true;
	}
	return DefaultMaterial;
}

void DMaterial::InitializeDefaultMaterial()
{
	DString VertexShader = DAssetManager::Get().SynchronousLoadAsset("Assets/Shaders/Default.vert")->AsString();
	DString FragmentShader = DAssetManager::Get().SynchronousLoadAsset("Assets/Shaders/Default.frag")->AsString();

	Check(VertexShader.Length() > 0);
	Check(FragmentShader.Length() > 0);

	DefaultMaterial = std::make_shared<DMaterial>();
	DefaultMaterial->BuildShader(VertexShader, FragmentShader);
}

DSharedPtr<DMaterial> DMaterial::GetQuadCopyMaterial()
{
	static bool bInitializedQuadCopyMaterial = false;
	if (!bInitializedQuadCopyMaterial)
	{
		InitializeQuadCopyMaterial();
		bInitializedQuadCopyMaterial = true;
	}
	return QuadCopyMaterial;
}

void DMaterial::InitializeQuadCopyMaterial()
{
	DString VertexShader = DAssetManager::Get().SynchronousLoadAsset("Assets/Shaders/QuadCopy.vert")->AsString();
	DString FragmentShader = DAssetManager::Get().SynchronousLoadAsset("Assets/Shaders/QuadCopy.frag")->AsString();

	Check(VertexShader.Length() > 0);
	Check(FragmentShader.Length() > 0);

	QuadCopyMaterial = std::make_shared<DMaterial>();
	QuadCopyMaterial->BuildShader(VertexShader, FragmentShader);
}

void DMaterial::BuildShader(const DString& Vertex, const DString& Fragment)
{
	DEngine::RenderThread->InvokeImmediately(DAction<int>([&](int a)
		{
			Check(Vertex.Length());
			Check(Fragment.Length());

			BuildVertexShader(Vertex);
			BuildFragmentShader(Fragment);

			if (!VertexShader || !FragmentShader)
			{
				DestroyShader();
			}

			BuildProgram();
		}));
}

void DMaterial::BuildVertexShader(const DString& Vertex)
{
	// NOTE: Must be on render thread.

	const char* VertexShaderCString = *Vertex;

	VertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(VertexShader, 1, &VertexShaderCString, NULL);
	glCompileShader(VertexShader);

	int Success;
	char InfoLog[512];
	glGetShaderiv(VertexShader, GL_COMPILE_STATUS, &Success);

	if (!Success)
	{
		glGetShaderInfoLog(VertexShader, 512, NULL, InfoLog);
		LOG_ERR(DString::Format("Vertex Shader: %s", InfoLog));
		VertexShader = 0;
	}
}

void DMaterial::BuildFragmentShader(const DString& Fragment)
{
	// NOTE: Must be on render thread.
	
	const char* FragmentShaderCString = *Fragment;

	FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(FragmentShader, 1, &FragmentShaderCString, NULL);
	glCompileShader(FragmentShader);

	int Success;
	char InfoLog[512];
	glGetShaderiv(FragmentShader, GL_COMPILE_STATUS, &Success);

	if (!Success)
	{
		glGetShaderInfoLog(FragmentShader, 512, NULL, InfoLog);
		LOG_ERR(DString::Format("Fragment Shader: %s", InfoLog));
		FragmentShader = 0;
	}
}

void DMaterial::BuildProgram()
{
	// NOTE: Must be on render thread.
	
	ShaderProgram = glCreateProgram();

	Check (VertexShader);
	Check (FragmentShader);
	if (!VertexShader || !FragmentShader)
	{
		// the minimum for a shader program is vertex + fragment
		DestroyShader();
		return;
	}

	glAttachShader(ShaderProgram, VertexShader);
	glAttachShader(ShaderProgram, FragmentShader);
	glLinkProgram(ShaderProgram);

	int Success;
	char InfoLog[512];
	glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);
	if (!Success) {
		glGetProgramInfoLog(ShaderProgram, 512, NULL, InfoLog);
		LOG_ERR(DString::Format("Shader Program: %s", InfoLog));
		ShaderProgram = 0;
	}

	if (VertexShader)
	{
		glDeleteShader(VertexShader);
		VertexShader = 0;
	}
	if (FragmentShader)
	{
		glDeleteShader(FragmentShader);
		FragmentShader = 0;
	}
}

DMaterial::~DMaterial()
{
	DestroyShader();
}

void DMaterial::DestroyShader()
{
	if (VertexShader)
	{
		glDeleteShader(VertexShader);
		VertexShader = 0;
	}

	if (FragmentShader)
	{
		glDeleteShader(FragmentShader);
		FragmentShader = 0;
	}

	if (ShaderProgram)
	{
		glDeleteProgram(ShaderProgram);
		ShaderProgram = 0;
	}
}
