#pragma once

#include "ModelReplacer/ModelReplacer.h"

namespace Settings
{
	namespace JSON
	{
		enum class InternalResponse : uint8_t
		{
			kSuccess = 0,
			kFailure,
			kNoOp
		};

		class Holder : 
			public ISingleton<Holder>
		{
		public:
			bool Read();

		private:
			std::string MIN_VERSION_FIELD{ "MinimumVersion" };
			std::string SWAPS_FIELD{ "Swaps" };

			std::string BASE_OBJECT_FIELD{ "BaseObject" };
			std::string ALT_MODELS_FIELD{ "AltModels" };

			std::string CONDITIONAL_COUNT_FIELD{ "Count" };
			std::string CONDITIONAL_MODEL_FIELD{ "Model" };

			std::string ALT_TEXTURE_FIELD{ "AltTextures" };
			std::string ALT_TEXTURE_PATH_FIELD{ "Diffuse" };
			std::string ALT_TEXTURE_TARGET_FIELD{ "Target" };

			std::vector<std::pair<RE::TESBoundObject*, ModelReplacer::ModelSwap>> configData{};
			std::vector<RE::TESBoundObject*>                                      swapForms{};

			bool ReadConfig(const Json::Value& a_json);
			bool ReadEntry(const Json::Value& a_json);
			bool ReadNewModel(const Json::Value& a_json, RE::TESBoundObject* a_base);

			/// <summary>
			/// Populates a given vector with all the valid texture swaps found in the provided json field.
			/// </summary>
			/// <param name="a_json">The field to look into.</param>
			/// <param name="a_target">The vector to populate. Doesn't reserve.</param>
			/// <returns>kSuccess on at least one new texture swap, kNoOp on no error but no texture swaps, kFailure on error.</returns>
			InternalResponse PopulateTextureSwaps(const Json::Value& a_json,
				std::vector<ModelReplacer::TextureSwap>& a_target);
		};
	}
}
