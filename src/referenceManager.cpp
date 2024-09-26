#include "referenceManager.h"

namespace Manager {
	RE::TESBoundObject* Manipulator::GetPair(RE::TESObjectREFR* a_ref)
	{
		auto* refBase = a_ref->GetBaseObject();
		if (!_matches.contains(refBase)) return nullptr;
		return _matches[refBase];
	}

	void Manipulator::AddPair(RE::TESBoundObject* a_base, RE::TESBoundObject* a_newModel)
	{
		if (!a_base || !a_newModel) return;
		_matches[a_base] = a_newModel;

		std::string key = _debugEDID(a_base);
		if (key.empty()) {
			key = a_base->GetName();
		}
		if (key.empty()) {
			std::stringstream ss;
			ss << a_base->formID;
			key = ss.str();
		}

		std::string_view value = _debugEDID(a_newModel);
		if (value.empty()) {
			value = a_newModel->GetName();
		}
		if (value.empty()) {
			std::stringstream ss;
			ss << a_newModel->formID;
			value = ss.str();
		}

		_loggerInfo("Registered new pair: {} to {}", key, value);
	}
}