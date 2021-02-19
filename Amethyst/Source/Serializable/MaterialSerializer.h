#pragma once
#include "Serializer.h"

namespace Amethyst
{
	//Simple dummy material system to test our serializing/deserializing system.

	class Material
	{
	public:
		//Shadow States
		bool m_ShadowCasting = true;
		bool m_ShadowReceiving = true;

		std::string m_NormalMapFilePath = "Source/Path";
		std::string m_AlbedoMapFilePath = "Source/Path";
		std::string m_SpecularMapFilePath = "Source/Path";
		std::string m_MaterialName = "Testing Material";
	};

	class MaterialSerializer : public Serializer
	{
	public:
		MaterialSerializer();
		~MaterialSerializer();
		
		void SaveMaterialToFile(Material& material, const std::string& outputFilePath);
		bool LoadMaterialFromFile(Material& material, const std::string& inputFilePath); //Takes in an existing reference from an empty/incorrect material from a model.

	private:
		virtual void SerializeToYAMLFile(const std::string& outputFilePath) override;
		virtual bool DeserializeFromYAMLFile(const std::string& inputFilePath) override;
	};
}
