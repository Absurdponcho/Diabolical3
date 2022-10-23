#pragma once
#include "Material.h"
#include "Types/DVector.h"
#include "Types/DString.h"
#include "DMath/Math.h"
#include "Logging/Logging.h"

class DMaterialUniformValueBase 
{
public:
	DMaterialUniformValueBase(GLint NewUniformLocation, const DString& NewName) 
	: UniformLocation(NewUniformLocation), Name(NewName) {}

	virtual ~DMaterialUniformValueBase() {}

	GLint UniformLocation = 0;
	DString Name;
	virtual void Apply() {}
};

template <typename T>
class DMaterialUniformValue : public DMaterialUniformValueBase
{
public:
	DMaterialUniformValue(GLint NewUniformLocation, const DString& NewName, const T& NewValue) : 
		DMaterialUniformValueBase(NewUniformLocation, NewName), Value(NewValue) {}

	T Value;

	virtual void Apply() override
	{
		ApplyInternal();
	}

	void ApplyInternal()
	{
		LOG_ERR(DString::Format("You must use a DMaterialUniformValue that has a specific template specialization. %s is not valid!", typeid(T).name()));
		Check(false);
		//static_assert(false, "You must use a DMaterialUniformValue that has a specific template specialization.");
	}
};

template <>
inline void DMaterialUniformValue<SVector4f>::ApplyInternal() { glUniform4f(UniformLocation, Value.X, Value.Y, Value.Z, Value.W); }
template <>
inline void DMaterialUniformValue<SVector3f>::ApplyInternal() { glUniform3f(UniformLocation, Value.X, Value.Y, Value.Z); }
template <>
inline void DMaterialUniformValue<SVector2f>::ApplyInternal() { glUniform2f(UniformLocation, Value.X, Value.Y); }
template <>
inline void DMaterialUniformValue<SVector3i>::ApplyInternal() { glUniform3i(UniformLocation, (GLint)Value.X, (GLint)Value.Y, (GLint)Value.Z); }
template <>
inline void DMaterialUniformValue<SVector2i>::ApplyInternal() { glUniform2i(UniformLocation, (GLint)Value.X, (GLint)Value.Y); }
template <>
inline void DMaterialUniformValue<int>::ApplyInternal() { glUniform1i(UniformLocation, Value); }
template <>
inline void DMaterialUniformValue<float>::ApplyInternal() { glUniform1f(UniformLocation, Value); }
template <>
inline void DMaterialUniformValue<SMatrix44f>::ApplyInternal() 
{	
	glUniformMatrix4fv(UniformLocation, 1, GL_FALSE, (const GLfloat*) & Value.Values);
}

class DMaterialInstance
{
public:
	DMaterialInstance(const DMaterialInstance& Other)
		:	ParentMaterial(Other.ParentMaterial), MaterialUniformValues(Other.MaterialUniformValues)
	{
		
	}

	DMaterialInstance(const DSharedPtr<DMaterial>& Material)
		: ParentMaterial(Material) 
	{
		
	}

	virtual ~DMaterialInstance() {}

	inline GLuint GetProgram() 
	{ 
		Check(ParentMaterial.IsValid());
		if (!ParentMaterial.IsValid())
		{
			return 0;
		}
		return ParentMaterial->GetProgram(); 
	}

	template <typename T>
	bool SetUniform(const DString& Name, const T& Value)
	{
		Check (Name.Length() > 0);
		if (Name.Length() == 0) return false;

		if (DMaterialUniformValue<T>* ExistingUniform = GetUniform<T>(Name))
		{
			ExistingUniform->Value = Value;
			return true;
		}

		if (GLuint Program = GetProgram())
		{
			GLint UniformLocation = glGetUniformLocation(Program, *Name);
			if (UniformLocation >= 0)
			{
				MaterialUniformValues.Add(new DMaterialUniformValue<T>(UniformLocation, Name, Value));
				return true;
			}
		}

		return false;
	}

	template <typename T>
	DMaterialUniformValue<T>* GetUniform(const DString& Name)
	{
		Check(Name.Length() > 0);
		if (Name.Length() == 0) return nullptr;

		for (DSharedPtr<DMaterialUniformValueBase>& Base : MaterialUniformValues)
		{
			if (Base->Name == Name)
			{
				if (DMaterialUniformValue<T>* Val = dynamic_cast<DMaterialUniformValue<T>*>(Base.GetRaw()))
				{
					return Val;
				}
			}
		}

		return nullptr;
	}

	inline void Bind()
	{
		GLuint ShaderProgram = GetProgram();
		glUseProgram(ShaderProgram);
		if (ShaderProgram > 0)
		{
			ApplyAllUniforms();
		}
	}

	inline void ApplyAllUniforms()
	{
		for (DSharedPtr<DMaterialUniformValueBase>& Base : MaterialUniformValues)
		{
			Base->Apply();
		}
	}

	DSharedPtr<DMaterial> GetParentMaterial() const
	{
		return ParentMaterial;
	}

protected:
	DSharedPtr<DMaterial> ParentMaterial;
	DVector<DSharedPtr<DMaterialUniformValueBase>> MaterialUniformValues;
};

