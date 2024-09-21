#pragma once

namespace Manager {
	class Manipulator : public ISingleton<Manipulator> {
	public:
		RE::TESBoundObject* GetPair(RE::TESObjectREFR* a_ref);
		void AddPair(RE::TESBoundObject* a_base, RE::TESBoundObject* a_newModel);

	private:
		std::unordered_map<RE::TESBoundObject*, RE::TESBoundObject*> _matches;
	};
}