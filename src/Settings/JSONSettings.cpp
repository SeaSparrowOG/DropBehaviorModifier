#include "JSONSettings.h"

#include "ModelReplacer/ModelReplacer.h"

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

		for (const auto& path : paths) {
			Json::Reader JSONReader;
			Json::Value JSONFile;
			try {
				std::ifstream rawJSON(path);
				JSONReader.parse(rawJSON, JSONFile);
				auto filename = path.substr(jsonFolder.size() + 1, path.size() - 1);
				logger::info("  >Reading config {}..."sv, filename);
				if (JSONFile.empty()) {
					logger::warn("  >Failed to read config {}."sv, filename);
					continue;
				}
				else if (!JSONFile.isObject()) {
					logger::warn("  >Config {} has an invalid structure at the top level."sv, filename);
					continue;
				}

				const auto& minVersionField = JSONFile[MIN_VERSION_FIELD];
				if (!minVersionField) {
					logger::warn("  >Config {} is lacking a minimum version. Treating as 0."sv, filename);
				}
				else if (!minVersionField.isInt()) {
					logger::warn("  >Config {} has a {} field, but it is not an int. Config will be ignored."sv, filename, MIN_VERSION_FIELD);
					continue;
				}
				else if (minVersionField.asInt() > 1) {
					logger::error("  >Config {} requires a newer version of {}. Check the mod page for updates."sv, filename, Plugin::NAME);
					continue;
				}

				try {
					ReadConfig(JSONFile);
				}
				catch (Json::Exception& e) {
					logger::warn("    >Caught {} while reading file. File will be ignored."sv, e.what());
				}
				catch (std::exception& e) {
					logger::warn("    >Caught unexpected exception {} while reading file. This should be reported to the mod page."sv, e.what());
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

	void Holder::ReadConfig(const Json::Value& a_json) {
		auto* modelReplacer = ModelReplacer::Swapper::GetSingleton();
		if (!modelReplacer) {
			logger::critical("Failed to get internal singleton, this is fatal and completely unexpected."sv);
			throw std::exception("Internal Model Replacer manager could not be resolved.");
		}

		const auto rawForms = a_json.getMemberNames();
		std::vector<Candidate> candidates{};
		candidates.reserve(rawForms.size() - 1);

		for (const auto& rawForm : rawForms) {
			if (rawForm == MIN_VERSION_FIELD) {
				continue;
			}

			const auto& candidateField = a_json[rawForm];
			if (!candidateField.isArray()) {
				logger::warn("    >Field {} is not an array. Config will be ignored."sv, rawForm);
				return;
			}

			auto* baseForm = Utilities::Forms::GetFormFromString<RE::TESBoundObject>(rawForm);
			if (!baseForm) {
				logger::info("    >Failed to resolve <{}> form. This could be fine."sv, rawForm);
				continue;
			}

			auto conditionalMeshes = std::vector<std::pair<int32_t, std::string>>();
			conditionalMeshes.reserve(candidateField.size());
			
			for (const auto& entry : candidateField) {
				if (!entry.isObject()) {
					logger::warn("    >An entry in {} is not an object. Config will be ignored."sv, rawForm);
					return;
				}

				if (!CreateNewEntry(entry, conditionalMeshes)) {
					return;
				}
			}
			Candidate newCandidate = Candidate(baseForm, conditionalMeshes);
			candidates.push_back(std::move(newCandidate));
		}
		logger::info("    >Found {} potential swaps."sv, candidates.size());
		for (const auto& candidate : candidates) {
			auto* candidateForm = candidate.baseForm;
			logger::info("      >{}"sv, candidateForm->GetName());
			for (const auto& [count, path] : candidate.swaps) {
				auto result = modelReplacer->RegisterSwap(candidateForm, count, path);
				if (result == ModelReplacer::SwapRegistrationReport::kCountExists) {
					logger::info("        >A swap for count {} has already been specified."sv, count);
				}
				else {
					logger::info("        >Replaced by {} when it has {} or more count."sv, path, count);
				}
			}
		}
	}

	bool Holder::CreateNewEntry(const Json::Value& a_json, 
		std::vector<std::pair<int32_t, std::string>>& a_results) 
	{
		const auto& condCountField = a_json[CONDITIONAL_COUNT_FIELD];
		const auto& newMeshField = a_json[CONDITIONAL_MODEL_FIELD];
		if (!condCountField) {
			logger::warn("    >Entry does not have a count specified."sv);
			return false;
		}
		else if (!condCountField.isInt()) {
			logger::warn("    >Entry has a non-int count specified."sv);
			return false;
		}

		const auto minCount = condCountField.asInt();
		if (minCount < 1) {
			logger::warn("    >Entry has a count less than 1, which is not supported."sv);
			return false;
		}

		if (!newMeshField) {
			logger::warn("    >Entry does not have a replacement mesh specified."sv);
			return false;
		}
		else if (!newMeshField.isString()) {
			logger::warn("    >Entry has a non-string entry in the replacement mesh field."sv);
			return false;
		}

		auto modelPath = newMeshField.asString();
		if (!std::regex_match(modelPath.c_str(), std::regex(R"(^((?!Meshes\\).)*\\([^\\\n]*\.nif)$)"))) {
			logger::warn("    >Provided mesh path <{}> is likely invalid."sv, modelPath);
			return false;
		}

		std::string fullPath = "meshes\\" + modelPath;
		if (!RE::BSResourceNiBinaryStream(fullPath.c_str()).good()) {
			logger::warn("    >Failed to find <{}>."sv, modelPath);
			return false;
		}

		auto newPair = std::pair<int32_t, std::string>(minCount, modelPath);
		a_results.push_back(std::move(newPair));
		return true;
	}
}
