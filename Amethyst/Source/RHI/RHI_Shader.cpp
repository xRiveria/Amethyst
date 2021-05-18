#include "Amethyst.h"
#include "RHI_Shader.h"
#include "RHI_InputLayout.h"
#include "../Threading/Threading.h"
#include "../Rendering/Renderer.h"

namespace Amethyst
{
	RHI_Shader::RHI_Shader(Context* context) : AmethystObject(context)
	{
		m_RHI_Device = m_Context->RetrieveSubsystem<Renderer>()->RetrieveRHIDevice();
		m_InputLayout = std::make_shared<RHI_InputLayout>(m_RHI_Device);
	}

	template<typename T>
	void RHI_Shader::Compile(const RHI_Shader_Type shaderType, const std::string& shaderPath)
	{
		m_ShaderType = shaderType;
		m_VertexType = RHI_Vertex_TypeToEnum<T>();

		//Can also be the source.
		const bool isFile = FileSystem::IsFile(shaderPath);

		//Deduce name and file path.
		if (isFile)
		{
			m_Name = FileSystem::RetrieveFileNameFromFilePath(shaderPath);
			m_FilePath = shaderPath;
		}
		else
		{
			m_Name.clear();
			m_FilePath.clear();
		}

		//Compile
		m_CompilationState = RHI_Shader_Compilation_State::Compiling;
		m_Resource = _Compile(shaderPath); //Juice!
		m_CompilationState = m_Resource ? RHI_Shader_Compilation_State::Succeeded : RHI_Shader_Compilation_State::Failed;

		//Log Compilation Results

		std::string typeString = "Unknown";
		typeString = shaderType == RHI_Shader_Vertex	? "Vertex"  : typeString;
		typeString = shaderType == RHI_Shader_Pixel		? "Pixel"   : typeString;
		typeString = shaderType == RHI_Shader_Compute   ? "Compute" : typeString;

		std::string defines;
		for (const auto& define : m_Defines)
		{
			if (!defines.empty())
			{
				defines += ", ";
			}

			defines += define.first + " = " + define.second;
		}

		if (m_CompilationState == RHI_Shader_Compilation_State::Succeeded)
		{
			if (defines.empty())
			{
				AMETHYST_INFO("Successfully compiled %s shader from \"%s\".", typeString.c_str(), shaderPath.c_str());
			}
			else
			{
				AMETHYST_INFO("Successfully compiled %s shader from \"%s\" with definitions \"%s\".", typeString.c_str(), shaderPath.c_str(), defines.c_str());
			}
		}
		else if (m_CompilationState == RHI_Shader_Compilation_State::Failed)
		{
			if (defines.empty())
			{
				AMETHYST_INFO("Failed to compile %s shader from \"%s\".", typeString.c_str(), shaderPath.c_str());
			}
			else
			{
				AMETHYST_INFO("Failed to compile %s shader from \"%s\" with definitions \"%s\".", typeString.c_str(), shaderPath.c_str(), defines.c_str());
			}
		}
	}

	template<typename T>
	void RHI_Shader::CompileAsync(const RHI_Shader_Type shaderType, const std::string& shaderPath)
	{
		m_Context->RetrieveSubsystem<Threading>()->AddTask([this, shaderType, shaderPath]()
		{
			Compile<T>(shaderType, shaderPath);
		});
	}

	/*	//Explicit Template Instantiation
		
		We use explicit instantiation to create an instantiation of a templated class or function without actually using them in our code. This is useful when
		creating library (.lib) files that use templates for distributions as uninstantiated template definitions are not put into object (.obj) files.

		libstdc++ contains the explicit instantiation for all stream, locale and string classes, specializedd for char and wchar_t.
		For example, everytime we use std::string, the same function code doesn't need to be copied to objects. The compiler only need to refer (link) to those in libstdc++.
	*/
	template void RHI_Shader::CompileAsync<RHI_Vertex_Undefined>(const RHI_Shader_Type shaderType, const std::string& shaderPath);
	template void RHI_Shader::CompileAsync<RHI_Vertex_Position>(const RHI_Shader_Type shaderType, const std::string& shaderPath);
	template void RHI_Shader::CompileAsync<RHI_Vertex_PositionTexture>(const RHI_Shader_Type shaderType, const std::string& shaderPath);
	template void RHI_Shader::CompileAsync<RHI_Vertex_PositionColor>(const RHI_Shader_Type shaderType, const std::string& shaderPath);
	template void RHI_Shader::CompileAsync<RHI_Vertex_Position2DTextureColor8>(const RHI_Shader_Type shaderType, const std::string& shaderPath);
	template void RHI_Shader::CompileAsync<RHI_Vertex_PositionTextureNormalTangent>(const RHI_Shader_Type shaderType, const std::string& shaderPath);

	void RHI_Shader::WaitForCompilation()
	{
		//Wait
		while (m_CompilationState == RHI_Shader_Compilation_State::Compiling)
		{
			AMETHYST_INFO("Wait for shader \"%s\" to compile...", m_Name.c_str());
			std::this_thread::sleep_for(std::chrono::milliseconds(16));
		}

		//Log error in case of failure.
		if (m_CompilationState != RHI_Shader_Compilation_State::Succeeded)
		{
			AMETHYST_ERROR("Shader \"%s\" failed to compile.", m_Name.c_str());
		}
	}

	const char* RHI_Shader::RetrieveEntryPoint() const //How we contextualize our main() functions in our shaders.
	{
		static const char* entryPointEmpty = nullptr;

		static const char* entryPointVertexShader = "mainVS";
		static const char* entryPointPixelShader = "mainPS";
		static const char* entryPointComputeShader = "mainCS";

		if (m_ShaderType == RHI_Shader_Vertex)  return entryPointVertexShader;
		if (m_ShaderType == RHI_Shader_Pixel)   return entryPointPixelShader;
		if (m_ShaderType == RHI_Shader_Compute) return entryPointComputeShader;

		return entryPointEmpty;
	}

	//We need to specify the shader target — the set of shader features — to compile against when you call the D3DCompile, D3DCompile2, or D3DCompileFromFile function. 
	const char* RHI_Shader::RetrieveTargetProfile() const 
	{
		return nullptr;
	}

	const char* RHI_Shader::RetrieveShaderModel() const
	{	
		return nullptr;
	}
}