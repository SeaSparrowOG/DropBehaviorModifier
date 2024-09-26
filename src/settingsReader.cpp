#include "settingsReader.h"

#include "dropPrevention.h"
#include "referenceManager.h"

namespace {
	RE::TESBoundObject* ParseForm(const std::string& a_identifier) {
		if (const auto splitID = clib_util::string::split(a_identifier, "|"); splitID.size() == 2) {
			if (!clib_util::string::is_only_hex(splitID[1])) return nullptr;
			const auto  formID = clib_util::string::to_num<RE::FormID>(splitID[1], true);

			const auto& modName = splitID[0];
			if (!RE::TESDataHandler::GetSingleton()->LookupModByName(modName)) return nullptr;

			auto* baseForm = RE::TESDataHandler::GetSingleton()->LookupForm(formID, modName);
			return baseForm ? static_cast<RE::TESBoundObject*>(baseForm) : nullptr;
		}
		auto* form = RE::TESBoundObject::LookupByEditorID(a_identifier);
		if (form) return static_cast<RE::TESBoundObject*>(form);
		return nullptr;
	}
}

namespace Settings {
	bool ReadSettings()
	{
		auto* managerSingleton = Manager::Manipulator::GetSingleton();
		auto* dropManager = DropPrevention::DropObject::GetSingleton();

		std::vector<std::string> configPaths = std::vector<std::string>();
		try {
			configPaths = clib_util::distribution::get_configs(R"(Data\SKSE\Plugins\DropBehaviorModifier\)", "", ".json"sv);
		}
		catch (std::exception e) {
			_loggerError("Caught error {} while trying to fetch fire config files.", e.what());
			return false;
		}
		if (configPaths.empty()) return true;

		for (auto& config : configPaths) {
			std::ifstream rawJSON(config);
			Json::Reader  JSONReader;
			Json::Value   JSONFile;
			JSONReader.parse(rawJSON, JSONFile);

			if (JSONFile.isObject()) {
				auto members = JSONFile.getMemberNames();
				for (auto& member : members) {
					if (!JSONFile[member].isString()) {
						_loggerInfo("JSON Error for {}. Member {} is not a string.", config, member);
						continue;
					}

					auto* newMember = ParseForm(JSONFile[member].asString());
					auto* oldMember = ParseForm(member);
					if (!newMember) {
						_loggerInfo("JSON Error for {}, {}: Could not parse form.", config, JSONFile[member].asString());
						continue;
					}
					if (!oldMember) {
						_loggerInfo("JSON Error for {}, {}: Could not parse form.", config, member);
						continue;
					}

					managerSingleton->AddPair(oldMember, newMember);
				}
			}
			else if (JSONFile.isArray()) {
				for (auto& member : JSONFile) {
					if (!member.isString()) {
						_loggerInfo("JSON Error for {}. Member is not a string.", config);
						continue;
					}

					auto* form = ParseForm(member.asString());
					if (!form) {
						_loggerInfo("JSON Error for {}, {}: Could not parse form.", config, member.asString());
						continue;
					}

					dropManager->RegisterUndropableObject(form);
				}
			}
		}
		return true;
	}
}