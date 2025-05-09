#pragma once

#include "ModelReplacer/ModelReplacer.h"
#include "Settings/INISettings.h"

namespace Hooks 
{
	/// <summary>
	/// Helper class to reduce boilerplate.
	/// </summary>
	/// <typeparam name="T">CRTP. Use the class you are instantiating. Used to give each derived class a unique _func.</typeparam>
	template <typename T>
	class VFuncHookHelper :
		public ISingleton<T>
	{
	private:
		inline static RE::NiAVObject* Thunk(RE::TESObject* a_this,
			RE::TESObjectREFR* a_ref)
		{
			auto* replacer = ModelReplacer::Swapper::GetSingleton();
			auto* base = replacer && a_this ? skyrim_cast<RE::TESBoundObject*>(a_this) : nullptr;
			auto* alt = replacer ? replacer->AttemptModelSwap(base, a_ref) : nullptr;
			return alt ? alt : _func(a_this, a_ref);
		}

		inline static REL::Relocation<decltype(&Thunk)> _func;
		inline static constexpr size_t idx{ 0x47 };

	public:
		inline static bool Install(REL::ID a_vtableAddress)
		{
			logger::info("{}"sv, T::hookName);
			logger::info("  >Checking INI to see if hook should be installed."sv);
			auto* iniHolder = Settings::INI::Holder::GetSingleton();
			if (!iniHolder) {
				logger::critical("    >Failed to fetch ini settings holder."sv);
				return false;
			}

			auto installRaw = iniHolder->GetStoredSetting<bool>(T::setting);
			bool install = installRaw.has_value() ? installRaw.value() : false;
			if (!installRaw.has_value()) {
				logger::warn("    >Setting {} not found in ini settings, treating as false.", T::setting.c_str());
			}
			if (!install) {
				logger::info("    >Hook not installed."sv);
				return true;
			}

			REL::Relocation<std::uintptr_t> VTABLE{ a_vtableAddress };
			_func = VTABLE.write_vfunc(idx, Thunk);
			logger::info("    >Hook installed."sv);
			return true;
		}
	};

	class AlchemyItemHook :
		public VFuncHookHelper<AlchemyItemHook>
	{
	public:
		inline static std::string hookName{ "Alchemy Item hook:"sv };
		inline static std::string setting{ "Hooks|bTweakAlchemyItems" };
	};

	class IngredientItemHook :
		public VFuncHookHelper<IngredientItemHook>
	{
	public:
		inline static std::string hookName{ "Ingredient hook:"sv };
		inline static std::string setting{ "Hooks|bTweakIngredients" };
	};

	class TESObjectArmoHook :
		public VFuncHookHelper<TESObjectArmoHook>
	{
	public:
		inline static std::string hookName{ "Armor hook:"sv };
		inline static std::string setting{ "Hooks|bTweakArmors" };
	};

	class TESObjectWeaponHook :
		public VFuncHookHelper<TESObjectWeaponHook>
	{
	public:
		inline static std::string hookName{ "Weapon hook:"sv };
		inline static std::string setting{ "Hooks|bTweakWeapons" };
	};

	class TESObjectBookHook :
		public VFuncHookHelper<TESObjectBookHook>
	{
	public:
		inline static std::string hookName{ "Book hook:"sv };
		inline static std::string setting{ "Hooks|bTweakBooks" };
	};

	class TESObjectMiscHook :
		public VFuncHookHelper<TESObjectMiscHook>
	{
	public:
		inline static std::string hookName{ "Misc Item hook:"sv };
		inline static std::string setting{ "Hooks|bTweakMiscObjects" };
	};

	bool Install();
}