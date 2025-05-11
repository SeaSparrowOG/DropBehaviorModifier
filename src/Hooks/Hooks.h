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
#ifndef NDEBUG
			const auto itemCount = a_ref ? a_ref->extraList.GetCount() : 0;
			if (base && itemCount > 1 && itemCount != 100) {
				LOG_DEBUG("Stacked item: [{}]:{}", itemCount, base->GetName());
			}
#endif
			auto* alt = replacer ? replacer->AttemptModelSwap(base, a_ref, replaceInventoryModel) : nullptr;
			return alt ? alt : _func(a_this, a_ref);
		}

		inline static REL::Relocation<decltype(&Thunk)> _func;
		inline static constexpr size_t idx{ 0x47 };

		inline static std::string inventorySetting{ "Inventory|bEnable" };
		inline static bool        replaceInventoryModel{ false };

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

			auto enableInventoryRaw = iniHolder->GetStoredSetting<bool>(inventorySetting);
			if (!enableInventoryRaw.has_value()) {
				logger::warn("    >Setting {} not found in ini settings, treating as false."sv, inventorySetting);
			}
			replaceInventoryModel = enableInventoryRaw.has_value() ? enableInventoryRaw.value() : false;

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

	class TESObjectSoulGemHook :
		public VFuncHookHelper<TESObjectSoulGemHook>
	{
	public:
		inline static std::string hookName{ "Soul Gem hook:"sv };
		inline static std::string setting{ "Hooks|bTweakSoulGems" };
	};

	class Inventory3DManagerHook :
		public ISingleton<Inventory3DManagerHook> {
	public:
		bool Install();

	private:
		inline static void Thunk(RE::Inventory3DManager* a_this,
			RE::InventoryEntryData* a_entryData);

		inline static REL::Relocation<decltype(&Thunk)> _func;
		inline static constexpr size_t offset{ 0x4F };
		std::string setting{ "Inventory|bEnable" };
	};

	bool Install();
}