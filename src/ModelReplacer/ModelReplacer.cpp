#include "ModelReplacer.h"

#include "RE//Misc.h"

namespace ModelReplacer
{
	RE::NiAVObject* Swapper::AttemptModelSwap(RE::TESObject* a_base,
		RE::TESObjectREFR* a_ref)
	{
		const auto count = a_ref ? a_ref->extraList.GetCount() : 0;

		if (a_base && a_ref && count > 1) {
			const auto args = RE::BSModelDB::DBTraits::ArgsType();
			auto out = RE::NiPointer<RE::NiNode>();
			int error = RE::Demand("Weapons/Weapons/IronSword.nif", out, args);
			return error == 0 ? out.get() : nullptr;
		}
		
		return nullptr;
	}
}