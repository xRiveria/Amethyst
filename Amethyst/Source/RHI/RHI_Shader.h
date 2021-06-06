#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "../Core/AmethystObject.h"
#include "RHI_Vertex.h"
#include "RHI_Descriptor.h"
#include <atomic>
//Check new stuff.
namespace Amethyst
{
	class Context;

	class RHI_Shader : public AmethystObject
	{
	public:
		RHI_Shader() = default;
		RHI_Shader(Context* context, const RHI_Vertex_Type vertexType = RHI_Vertex_Type::RHI_Vertex_Type_Unknown);
		~RHI_Shader();

		//Compilation
		void Compile(const RHI_Shader_Type shaderType, const std::string& shaderPath, bool async);
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
		const std::shared_ptr<RHI_InputLayout>& RetrieveInputLayout()	const { return m_InputLayout; } //Only valid for vertex shader.
		const std::string& RetrieveFilePath()							const { return m_FilePath; }
		RHI_Shader_Type RetrieveShaderType()						    const { return m_ShaderType; }
		const char* RetrieveEntryPoint() const;			///
		const char* RetrieveTargetProfile() const;		///
		const char* RetrieveShaderModel() const;		///

	protected:
		std::shared_ptr<RHI_Device> m_RHI_Device;

	private:
		//All compile functions resolve to this, and this is what the underlying API implements.
		void ParseSource(const std::string& filePath);
		void Compile2();
		void* Compile3();
		void Reflect(const RHI_Shader_Type shaderType, const uint32_t* pointer, uint32_t size);

	private:
		std::string m_FilePath;
		std::string m_Source;
		std::vector<std::string> m_Names;				 // The names of the files from the include directives in the shader.
		std::vector<std::string> m_FilePaths;			 // The files paths of the files from the include directives in the shader.
		std::vector<std::string> m_Sources;				 // The source of the files from the include directives in the shader.
		std::vector<std::string> m_FilePathsMultiple;    // The file paths of include directives which are defined multiple times in the shader.

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
