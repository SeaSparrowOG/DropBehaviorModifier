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
	}
}