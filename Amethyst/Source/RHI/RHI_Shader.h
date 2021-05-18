#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "../Core/AmethystObject.h"
#include "RHI_Vertex.h"
#include "RHI_Descriptor.h"
#include <atomic>

namespace Amethyst
{
	class Context;

	class RHI_Shader : public AmethystObject
	{
	public:
		RHI_Shader() = default;
		RHI_Shader(Context* context);
		~RHI_Shader();

		//Compilation
		template<typename T> void Compile(const RHI_Shader_Type shaderType, const std::string& shaderPath);
		void Compile(const RHI_Shader_Type shaderType, const std::string& shaderPath) { Compile<RHI_Vertex_Undefined>(shaderType, shaderPath); }

		template<typename T> void CompileAsync(const RHI_Shader_Type shaderType, const std::string& shaderPath);
		void CompileAsync(const RHI_Shader_Type shaderType, const std::string& shaderPath) { CompileAsync<RHI_Vertex_Undefined>(shaderType, shaderPath); }

		RHI_Shader_Compilation_State RetrieveCompilationState() const { return m_CompilationState; }

		bool IsCompiled() const { return m_CompilationState == RHI_Shader_Compilation_State::Succeeded; }
		void WaitForCompilation();

		//Resource
		void* RetrieveResource() const { return m_Resource; }
		bool HasResource() const { return m_Resource != nullptr; }

		//Name
		const std::string& RetrieveName() const { return m_Name; }
		void SetName(const std::string& shaderName) { m_Name = shaderName; }

		//Defines
		void AddDefine(const std::string& define, const std::string& value = "1") { m_Defines[define] = value; } ///
		auto& RetrieveDefines() const { return m_Defines; } ///

		//Misc
		const std::vector<RHI_Descriptor>& RetrieveDescriptors()		const { return m_Descriptors; }
		const std::shared_ptr<RHI_InputLayout> RetrieveInputLayout()	const { return m_InputLayout; } //Only valid for vertex shader.
		const std::string& RetrieveFilePath()							const { return m_FilePath; }
		RHI_Shader_Type RetrieveShaderType()						    const { return m_ShaderType; }
		const char* RetrieveEntryPoint() const;			///
		const char* RetrieveTargetProfile() const;		///
		const char* RetrieveShaderModel() const;		///

	protected:
		std::shared_ptr<RHI_Device> m_RHI_Device;

	private:
		//All compile functions resolve to this, and this is what the underlying API implements.
		void _Compile(const std::string& shaderPath);
		void _Reflect(const RHI_Shader_Type shaderType, const uint32_t* pointer, uint32_t size); ///

	private:
		std::string m_Name;
		std::string m_FilePath;
		std::unordered_map<std::string, std::string> m_Defines;
		std::vector<RHI_Descriptor> m_Descriptors;
		std::shared_ptr<RHI_InputLayout> m_InputLayout;
		std::atomic<RHI_Shader_Compilation_State> m_CompilationState = RHI_Shader_Compilation_State::Idle;
		RHI_Shader_Type m_ShaderType = RHI_Shader_Unknown;
		RHI_Vertex_Type m_VertexType = RHI_Vertex_Type_Unknown;

		//API
		void* m_Resource = nullptr;
	};
}
