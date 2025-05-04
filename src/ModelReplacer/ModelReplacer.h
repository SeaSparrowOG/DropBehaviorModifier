#pragma once

namespace ModelReplacer
{
	struct ModelSwap
	{

	};

	class Swapper : public ISingleton<Swapper>
	{
	public:
		RE::NiAVObject* AttemptModelSwap(RE::TESObject* a_base, RE::TESObjectREFR* a_ref);
	};
}