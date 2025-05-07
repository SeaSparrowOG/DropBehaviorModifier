#pragma once

#include "Settings/INISettings.h"

namespace Hooks 
{
	template <class F>
	std::optional<std::uintptr_t> HookIfNecessary(REL::ID a_vtableID, 
		std::size_t a_idx,
		const std::string& a_setting,
		F a_newFunc) 
	{
		logger::info("  >Checking INI to see if hook should be installed."sv);
		auto* iniHolder = Settings::INI::Holder::GetSingleton();
		if (!iniHolder) {
			logger::critical("    >Failed to fetch ini settings holder."sv);
			throw std::exception("Failed to retrieve ini holder.");
		}

		auto installRaw = iniHolder->GetStoredSetting<bool>(a_setting);
		bool install = installRaw.has_value() ? installRaw.value() : false;
		if (!installRaw.has_value()) {
			logger::warn("    >Setting {} not found in ini settings, treating as false.", a_setting.c_str());
		}
		if (!install) {
			return std::nullopt;
		}

		REL::Relocation<std::uintptr_t> VTABLE{ a_vtableID };
		return VTABLE.write_vfunc(a_idx, a_newFunc);
	}

	class TESObjectMiscHook :
		public ISingleton<TESObjectMiscHook>
	{
	public:
		inline static bool Install(REL::ID a_vtableAddress);

	private:
		inline static RE::NiAVObject* Thunk(RE::TESObject* a_this, RE::TESObjectREFR* a_ref);
		inline static REL::Relocation<decltype(&Thunk)> _func;
		inline static std::string setting{ "Hooks|bTweakMiscObjects" };
		static constexpr size_t loadGraphicsOffset{ 0x47 };
	};

	class TESObjectALCIHook :
		public ISingleton<TESObjectALCIHook>
	{
	public:
		inline static bool Install(REL::ID a_vtableAddress);

	private:
		inline static RE::NiAVObject* Thunk(RE::TESObject* a_this, RE::TESObjectREFR* a_ref);
		inline static REL::Relocation<decltype(&Thunk)> _func;
		inline static std::string setting{ "Hooks|bTweakAlchemyItems" };
		static constexpr size_t loadGraphicsOffset{ 0x47 };
	};

	bool Install();
}