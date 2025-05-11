#include "Hooks.h"

#include "RE/Offset.h"

namespace Hooks 
{
	bool Install() {
		logger::info("==========================================================");
		logger::info("Installing Hooks...");
		bool nominal = true;

		auto* alchemyItem = AlchemyItemHook::GetSingleton();
		if (!alchemyItem) {
			logger::critical("  >Failed to get alchemyItem singleton."sv);
			nominal = false;
		}
		auto* ingredientManager = IngredientItemHook::GetSingleton();
		if (!ingredientManager) {
			logger::critical("  >Failed to get ingredientManager singleton."sv);
			nominal = false;
		}
		auto* armorManager = TESObjectArmoHook::GetSingleton();
		if (!armorManager) {
			logger::critical("  >Failed to get armorManager singleton."sv);
			nominal = false;
		}
		auto* weaponManager = TESObjectWeaponHook::GetSingleton();
		if (!weaponManager) {
			logger::critical("  >Failed to get weaponManager singleton."sv);
			nominal = false;
		}
		auto* bookManager = TESObjectBookHook::GetSingleton();
		if (!bookManager) {
			logger::critical("  >Failed to get bookManager singleton."sv);
			nominal = false;
		}
		auto* miscItemManager = TESObjectMiscHook::GetSingleton();
		if (!miscItemManager) {
			logger::critical("  >Failed to get miscItemManager singleton."sv);
			nominal = false;
		}
		auto* soulGemManager = TESObjectSoulGemHook::GetSingleton();
		if (!soulGemManager) {
			logger::critical("  >Failed to get soulGemManager singleton."sv);
			nominal = false;
		}
		auto* inventory3DManager = Inventory3DManagerHook::GetSingleton();
		if (!inventory3DManager) {
			logger::critical("  >Failed to get inventory3DManager singleton."sv);
			nominal = false;
		}

		if (!nominal) {
			return false;
		}

		SKSE::AllocTrampoline(14);

		return alchemyItem->Install(RE::Offset::AlchemyItem::VTABLE) &&
			ingredientManager->Install(RE::Offset::IngredientItem::VTABLE) &&
			armorManager->Install(RE::Offset::TESObjectARMO::VTABLE) &&
			weaponManager->Install(RE::Offset::TESObjectWEAP::VTABLE) &&
			bookManager->Install(RE::Offset::TESObjectBOOK::VTABLE) &&
			miscItemManager->Install(RE::Offset::TESObjectMISC::VTABLE) &&
			soulGemManager->Install(RE::Offset::TESSoulGem::VTABLE) &&
			inventory3DManager->Install();
	}

	bool Inventory3DManagerHook::Install() {
		logger::info("Inventory 3D Manager:"sv);

		REL::Relocation<std::uintptr_t> target{ REL::ID(51851), offset};
		if (!(REL::make_pattern<"E9">().match(target.address()))) {
			SKSE::stl::report_and_fail("Failed to validate pattern of the Inventory 3D Manager."sv);
		}

		auto& trampoline = SKSE::GetTrampoline();
		_func = trampoline.write_branch<5>(target.address(), &Thunk);

		return true;
	}

	inline void Inventory3DManagerHook::Thunk(RE::Inventory3DManager* a_this, 
		RE::InventoryEntryData* a_entryData)
	{
		auto* base = a_entryData ? a_entryData->GetObject() : nullptr;
		auto* ref = a_this ? a_this->tempRef : nullptr;
		if (!base || !ref) {
			_func(a_this, a_entryData);
			return;
		}

		a_this->Clear3D();
		a_this->loadedModels.clear();
		_func(a_this, a_entryData);
	}
}