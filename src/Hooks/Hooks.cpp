#include "Hooks.h"

#include <xbyak.h>

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

		if (!nominal) {
			return false;
		}

		return alchemyItem->Install(RE::Offset::AlchemyItem::VTABLE) &&
			ingredientManager->Install(RE::Offset::IngredientItem::VTABLE) &&
			armorManager->Install(RE::Offset::TESObjectARMO::VTABLE) &&
			weaponManager->Install(RE::Offset::TESObjectWEAP::VTABLE) &&
			bookManager->Install(RE::Offset::TESObjectBOOK::VTABLE) &&
			miscItemManager->Install(RE::Offset::TESObjectMISC::VTABLE);
	}
}