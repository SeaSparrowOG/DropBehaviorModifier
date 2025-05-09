#include "JSONSettings.h"

namespace Settings::JSON
{
	bool Holder::Read()
	{
		logger::info("==========================================================");
		std::string jsonFolder = fmt::format(R"(.\Data\SKSE\Plugins\{})"sv, Plugin::NAME);
		logger::info("Reading and validating project JSON files in {}.", jsonFolder);

		std::vector<std::string> paths{};
		try {
			for (const auto& entry : std::filesystem::directory_iterator(jsonFolder)) {
				if (entry.is_regular_file() && entry.path().extension() == ".json") {
					paths.push_back(entry.path().string());
				}
			}

			std::sort(paths.begin(), paths.end());
			logger::info("  >Found {} configuration files."sv, std::to_string(paths.size()));
		}
		catch (const std::exception& e) {
			logger::warn("Caught {} while reading files.", e.what());
			return false;
		}
		if (paths.empty()) {
			logger::info("No settings found");
			return true;
		}

		auto* modelReplacer = ModelReplacer::Swapper::GetSingleton();
		if (!modelReplacer) {
			logger::critical("  >Failed to get internal model replacer singleton."sv);
			return false;
		}

		for (const auto& path : paths) {
			Json::Reader JSONReader;
			Json::Value JSONFile;
			try {
				std::ifstream rawJSON(path);
				auto filename = path.substr(jsonFolder.size() + 1, path.size() - 1);
#ifdef NDEBUG
				if (filename == "_testConfig.json") {
					continue;
				}
#endif
				JSONReader.parse(rawJSON, JSONFile);
				logger::info("  >Reading config {}..."sv, filename);
				if (JSONFile.empty()) {
					logger::warn("  >Failed to read config {}."sv, filename);
					continue;
				}

				try {
					if (!configData.empty()) {
						configData.clear();
					}
					
					if (!ReadConfig(JSONFile)) {
						continue;
					}
				}
				catch (Json::Exception& e) {
					logger::warn("    >Caught {} while reading file. File will be ignored."sv, e.what());
				}
				catch (std::exception& e) {
					logger::warn("    >Caught unexpected exception {} while reading file. This should be reported to the mod page."sv, e.what());
				}

				for (auto& [base, data] : configData) {
					LOG_DEBUG("Registering rule for {}"sv, base->GetName());
					modelReplacer->RegisterSwap(base, data);
				}
			}
			catch (const Json::Exception& e) {
				logger::warn("Caught {} while reading files.", e.what());
				continue;
			}
			catch (const std::exception& e) {
				logger::error("Caught unhandled exception {} while reading files.", e.what());
				continue;
			}
		}

		logger::info("Finished reading all settings."sv);
		return true;
	}

	bool Holder::ReadConfig(const Json::Value& a_json) {
		if (!a_json.isObject()) {
			logger::warn("    >Expected Object at top level, config is invalid and will be skipped."sv);
			return false;
		}

		const auto& minVersionField = a_json[MIN_VERSION_FIELD];
		if (!minVersionField) {
			logger::info("    >Failed to fetch {}, treating as 1."sv, MIN_VERSION_FIELD);
		}
		else if (!minVersionField.isInt()) {
			logger::error("    >{} was specified, but was not an integer. Config will be treated as invalid and skipped."sv, MIN_VERSION_FIELD);
			return false;
		}
		else if (minVersionField.asInt() < 1) {
			logger::info("    >Config has {} specified, but it is less than 1. Treating as 1."sv, MIN_VERSION_FIELD);
		}
		else if (minVersionField.asInt() > 1) {
			logger::error("    >Config has {} specified, and requires a newer version of {}. Update on Nexus."sv, MIN_VERSION_FIELD, Plugin::NAME);
			return false;
		}

		const auto& newSwapsField = a_json[SWAPS_FIELD];
		if (!newSwapsField) {
			logger::error("    >Config does not have {} specified."sv, SWAPS_FIELD);
			return false;
		}
		else if (!newSwapsField.isArray()) {
			logger::error("    >Config has {}, but it is not an array. Treating config as invalid."sv, SWAPS_FIELD);
			return false;
		}
		
		for (const auto& swap : newSwapsField) {
			if (!ReadEntry(swap)) {
				return false;
			}
		}

		logger::info("    >Finished. Constructed {} swaps from config."sv, configData.size());
		return true;
	}

	bool Holder::ReadEntry(const Json::Value& a_json) {
		const auto& baseObjectField = a_json[BASE_OBJECT_FIELD];
		if (!baseObjectField) {
			logger::error("    >Config has swap defined in {} without {}. Treating config as invalid."sv, SWAPS_FIELD, BASE_OBJECT_FIELD);
			return false;
		}
		else if (!baseObjectField.isString()) {
			logger::error("    >Config has swap defined in {} with {}, but it is not a string. Treating config as invalid."sv, SWAPS_FIELD, BASE_OBJECT_FIELD);
			return false;
		}

		auto baseString = baseObjectField.asString();
		logger::info("    >Reading {}..."sv, baseString);
		auto* objectBase = Utilities::Forms::GetFormFromString<RE::TESBoundObject>(baseString);
		if (!objectBase) {
			logger::info("      >Base object not found. This may be normal if the form comes from an optional mod."sv);
			return true;
		}

		auto& altModelsField = a_json[ALT_MODELS_FIELD];
		if (!altModelsField) {
			logger::error("    >Config has swap defined in {} without {}. Treating config as invalid."sv, SWAPS_FIELD, ALT_MODELS_FIELD);
			return false;
		}
		else if (!altModelsField.isArray()) {
			logger::error("    >Config has swap defined in {} with {}, but it is not an array. Treating config as invalid."sv, SWAPS_FIELD, ALT_MODELS_FIELD);
			return false;
		}

		for (const auto& altMode : altModelsField) {
			if (!ReadNewModel(altMode, objectBase)) {
				return false;
			}
		}
	}

	bool Holder::ReadNewModel(const Json::Value& a_json, RE::TESBoundObject* a_base) {
		if (!a_json.isObject()) {
			logger::error("    >Element of {} is not an object, treating config as invalid."sv, ALT_MODELS_FIELD);
			return false;
		}

		const auto& newModelField = a_json[CONDITIONAL_MODEL_FIELD];
		if (!newModelField) {
			logger::error("      >Missing {}. Treating config as invalid."sv, CONDITIONAL_MODEL_FIELD);
			return false;
		}
		else if (!newModelField.isString()) {
			logger::error("      >{} is present, but not a string. Treating config as invalid."sv, CONDITIONAL_MODEL_FIELD);
			return false;
		}

		auto newModelPathRaw = newModelField.asString();
		auto newModelPath = Utilities::String::tolower(newModelPathRaw);

		if (!std::regex_match(newModelPath.c_str(), std::regex(R"(^((?!meshes\\).)*\\([^\\\n]*\.nif)$)"))) {
			logger::error("      >Provided field {} <{}> is invalid. Treating config as invalid."sv, CONDITIONAL_MODEL_FIELD, newModelPathRaw);
			return false;
		}

		std::string fullPath = "meshes\\" + newModelPath;
		if (!RE::BSResourceNiBinaryStream(fullPath.c_str()).good()) {
			logger::warn("      >Provided filed {} has path <{}> that could not be resolved. This may be normal if it comes from an optional mod."sv, CONDITIONAL_MODEL_FIELD, newModelPathRaw);
			return true;
		}

		const auto& condCountField = a_json[CONDITIONAL_COUNT_FIELD];
		if (!condCountField) {
			logger::error("      >Could not fetch the {} field. Treating config as invalid."sv, CONDITIONAL_COUNT_FIELD);
			return false;
		}
		else if (!condCountField.isInt()) {
			logger::error("      >Entry has {} specified, but it is not an int. Treating config as invalid."sv, CONDITIONAL_COUNT_FIELD);
			return false;
		}

		const auto minCount = condCountField.asInt();
		if (minCount < 1) {
			logger::error("      >Provided {} is less than 1, which is not supported. Treating config as invalid."sv, CONDITIONAL_COUNT_FIELD);
			return false;
		}

		auto results = std::vector<ModelReplacer::TextureSwap>();
		const auto& textureSwapField = a_json[ALT_TEXTURE_FIELD];
		if (textureSwapField) {
			if (!textureSwapField.isArray()) {
				logger::error("      >{} has {} specified, but it is not an array. Treating config as invalid."sv, SWAPS_FIELD, ALT_TEXTURE_FIELD);
				return false;
			}

			const auto size = textureSwapField.size();
			results.reserve(size);

			logger::info("      >{} found, reading {} swaps...", ALT_TEXTURE_FIELD, size);
			auto opResult = PopulateTextureSwaps(textureSwapField, results);

			if (opResult == InternalResponse::kNoOp) {
				logger::warn("        >{} field had no valid texture swaps. This may be normal if they are part of an optional mod."sv, ALT_TEXTURE_FIELD);
			}
			else if (opResult == InternalResponse::kFailure) {
				return false;
			}
		}

		auto constructedSwap = ModelReplacer::ModelSwap(minCount, newModelPath, results);
		auto newPair = std::pair<RE::TESBoundObject*, ModelReplacer::ModelSwap>(a_base, constructedSwap);
		LOG_DEBUG("Constructed rule for {}/{}/{}"sv, a_base->GetName(), minCount, newModelPath);
		configData.push_back(newPair);
		return true;
	}

	InternalResponse Holder::PopulateTextureSwaps(const Json::Value& a_json, 
		std::vector<ModelReplacer::TextureSwap>& a_target)
	{
		auto response = InternalResponse::kNoOp;
		for (const auto& swap : a_json) {
			if (!swap.isObject()) {
				logger::error("          >Entry is not an object, treating config as invalid."sv);
				return InternalResponse::kFailure;
			}

			const auto& newTexturePathField = swap[ALT_TEXTURE_PATH_FIELD];
			if (!newTexturePathField) {
				logger::error("          >Entry does not have a {} field, treating config as invalid."sv, ALT_TEXTURE_PATH_FIELD);
				return InternalResponse::kFailure;
			}
			else if (!newTexturePathField.isString()) {
				logger::error("          >Entry has {}, but it is not a string. Treating config as invalid."sv, ALT_TEXTURE_PATH_FIELD);
				return InternalResponse::kFailure;
			}

			auto newTexturePathRaw = newTexturePathField.asString();
			if (!std::regex_match(newTexturePathRaw.c_str(), std::regex(R"(^((?!tex\\).)*\\([^\\\n]*\.dds)$)"))) {
				logger::error("          >Provided field {} <{}> is invalid. Treating config as invalid."sv, ALT_TEXTURE_PATH_FIELD, newTexturePathRaw);
				return InternalResponse::kFailure;
			}

			auto newTexturePath = "textures\\" + newTexturePathRaw;
			newTexturePath = Utilities::String::tolower(newTexturePath);
			if (!RE::BSResourceNiBinaryStream(newTexturePath.c_str()).good()) {
				logger::warn("          >Path <{}> could not be resolved. This may be normal if it comes from an optional mod."sv, newTexturePathRaw);
				continue;
			}

			const auto& targetField = swap[ALT_TEXTURE_TARGET_FIELD];
			if (!targetField) {
				logger::error("          >Entry does not have a {} field, treating config as invalid."sv, ALT_TEXTURE_TARGET_FIELD);
				return InternalResponse::kFailure;
			}
			else if (!targetField.isString()) {
				logger::error("          >Entry has {}, but it is not a string. Treating config as invalid."sv, ALT_TEXTURE_TARGET_FIELD);
				return InternalResponse::kFailure;
			}

			auto nodePath = Utilities::String::split(targetField.asString(), "|");
			if (nodePath.empty()) {
				logger::warn("          >Entry with node path {} failed to return a proper node path. This is potentially an error, but won't invalidate the config."sv, targetField.asString());
				continue;
			}

			auto newAltTexture = ModelReplacer::TextureSwap(nodePath, newTexturePath);
			a_target.push_back(newAltTexture);
			response = InternalResponse::kSuccess;
		}
		return response;
	}
}
