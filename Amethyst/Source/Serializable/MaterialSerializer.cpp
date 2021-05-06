#include "Amethyst.h"
#include "MaterialSerializer.h"
#include <ostream>
#include <fstream>
#include <yaml-cpp/yaml.h>

namespace Amethyst
{
	static Material* s_MaterialBuffer = nullptr;

	MaterialSerializer::MaterialSerializer()
	{

	}

	MaterialSerializer::~MaterialSerializer()
	{

	}

	void MaterialSerializer::SaveMaterialToFile(Material& material, const std::string& outputFilePath)
	{
		s_MaterialBuffer = &material;
		SerializeToYAMLFile(outputFilePath);
	}

	bool MaterialSerializer::LoadMaterialFromFile(Material& material, const std::string& inputFilePath)
	{
		s_MaterialBuffer = &material;
		return DeserializeFromYAMLFile(inputFilePath);
	}

	void SerializeMaterial(YAML::Emitter& outputStream)
	{
		//We can create a key and map for each type of serializable "category".
		outputStream << YAML::Key << "Material_Map_Paths";
		outputStream << YAML::BeginMap; //File Paths
		outputStream << YAML::Key << "Normal_Map_Path" << YAML::Value << s_MaterialBuffer->m_NormalMapFilePath;
		outputStream << YAML::Key << "Albedo_Map_Path" << YAML::Value << s_MaterialBuffer->m_AlbedoMapFilePath;
		outputStream << YAML::Key << "Specular_Map_Path" << YAML::Value << s_MaterialBuffer->m_SpecularMapFilePath;
		outputStream << YAML::EndMap; //File Paths

		outputStream << YAML::Key << "Shadow_Settings";
		outputStream << YAML::BeginMap;
		outputStream << YAML::Key << "Shadow_Casting" << YAML::Value << s_MaterialBuffer->m_ShadowCasting;
		outputStream << YAML::Key << "Shadow_Receiving" << YAML::Value << s_MaterialBuffer->m_ShadowReceiving;
		outputStream << YAML::EndMap; //File Paths

	}

	void MaterialSerializer::SerializeToYAMLFile(const std::string& outputFilePath)
	{
		YAML::Emitter outputStream;

		outputStream << YAML::BeginMap;
		outputStream << YAML::Key << "Material" << YAML::Value << s_MaterialBuffer->m_MaterialName;
		
		SerializeMaterial(outputStream); //Material Paths

		outputStream << YAML::EndMap;

		//At the end of everything, we output it to a custom file that is saved on our system.
		std::ofstream materialOutputFile(outputFilePath);
		materialOutputFile << outputStream.c_str();
	}

	bool MaterialSerializer::DeserializeFromYAMLFile(const std::string& inputFilePath)
	{
		YAML::Node retrievalBuffer = YAML::LoadFile(inputFilePath);
		if (!retrievalBuffer["Material"]) { return false; } //If the Material node isn't present, something's wrong. Nodes are essentially the keys we created earlier.

		s_MaterialBuffer->m_MaterialName = retrievalBuffer["Material"].as<std::string>(); //We get our material name.

		YAML::Node materialPaths = retrievalBuffer["Material_Map_Paths"];
		if (materialPaths)
		{
			s_MaterialBuffer->m_AlbedoMapFilePath = materialPaths["Albedo_Map_Path"].as<std::string>();
			s_MaterialBuffer->m_SpecularMapFilePath = materialPaths["Specular_Map_Path"].as<std::string>();
			s_MaterialBuffer->m_NormalMapFilePath = materialPaths["Normal_Map_Path"].as<std::string>();
		}

		YAML::Node shadowSettings = retrievalBuffer["Shadow_Settings"];
		if (shadowSettings)
		{
			s_MaterialBuffer->m_ShadowCasting = shadowSettings["Shadow_Casting"].as<bool>();
			s_MaterialBuffer->m_ShadowReceiving = shadowSettings["Shadow_Receiving"].as<bool>();
		}

		return true;
	}
}