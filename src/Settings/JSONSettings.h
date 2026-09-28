#pragma once

#include "ClibUtil/string.hpp"
#include "ModelReplacer/ModelReplacer.h"

namespace Settings
{
	namespace JSON
	{
		template <typename T>
		T* GetFormFromString(const std::string& a_str)
		{
			auto* dh = RE::TESDataHandler::GetSingleton();
			if (!dh) {
				return nullptr;
			}

			const auto splitID = clib_util::string::split(a_str, "|");
			const auto splitSize = splitID.size();

			if (splitSize == 2) {
				const auto& pluginName = splitID[0];
				const auto& rawID = splitID[1];
				if (!dh->LookupModByName(pluginName)) {
					return nullptr;
				}
				else if (!clib_util::string::is_only_hex(rawID)) {
					return nullptr;
				}

				const auto formID = clib_util::string::to_num<RE::FormID>(rawID, true);
				auto* foundForm = dh->LookupForm(formID, pluginName);
				if (!foundForm) {
					nullptr;
				}
				else {
					return skyrim_cast<T*>(foundForm);
				}
			}
			auto* foundForm = RE::TESForm::LookupByEditorID(a_str);
			return foundForm ? skyrim_cast<T*>(foundForm) : nullptr;
		}

		enum class InternalResponse : uint8_t
		{
			kSuccess = 0,
			kFailure,
			kNoOp
		};

		class Holder : 
			public REX::TSingleton<Holder>
		{
		public:
			bool Read();

		private:
			std::string MIN_VERSION_FIELD{ "MinimumVersion" };
			std::string SWAPS_FIELD{ "Swaps" };

			std::string SHOW_IN_MENUS_FIELD{ "ShowInMenus" };
			std::string BASE_OBJECT_FIELD{ "BaseObject" };
			std::string ALT_MODELS_FIELD{ "AltModels" };

			std::string CONDITIONAL_COUNT_FIELD{ "Count" };
			std::string CONDITIONAL_MODEL_FIELD{ "Model" };

			std::string ALT_TEXTURE_FIELD{ "AltTextures" };
			std::string ALT_TEXTURE_PATH_FIELD{ "Diffuse" };
			std::string ALT_TEXTURE_TARGET_FIELD{ "Target" };

			std::vector<std::pair<RE::TESBoundObject*, ModelReplacer::ModelSwap>> configData{};
			std::vector<RE::TESBoundObject*>                                      swapForms{};
			bool                                                                  showInMenus{ false };

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
