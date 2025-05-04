#pragma once

namespace Hooks {
	class LoadListener : public ISingleton<LoadListener>
	{
	public:
		bool Install();

	private:
		struct LoadGraphicsPrologue
		{
			static bool Install();
			static RE::NiAVObject* Thunk(RE::TESObject* a_this, RE::TESObjectREFR* a_ref);

			inline static REL::Relocation<decltype(&Thunk)> _func;
		};
	};

	bool Install();
}