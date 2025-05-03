#pragma once

namespace Hooks {
	class LoadListener : public ISingleton<LoadListener>
	{
	public:
		bool Install();

	private:
		struct IngredientListener
		{
			static bool Install();
			static RE::NiAVObject* Thunk(RE::TESObject* a_this, RE::TESObjectREFR* a_ref);

			inline static size_t offset{ 0x47 };
			inline static REL::Relocation<decltype(&Thunk)> _func;
		};
	};

	bool Install();
}