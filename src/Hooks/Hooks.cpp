#include "Hooks.h"

#include <xbyak.h>

#include "ModelReplacer/ModelReplacer.h"
#include "RE/Offset.h"

namespace Hooks 
{
	bool Install() {
		logger::info("==========================================================");
		logger::info("Installing Hooks...");
		bool nominal = true;

		auto* miscManager = TESObjectMiscHook::GetSingleton();
		if (!miscManager) {
			logger::critical("  >Failed to get miscManager singleton."sv);
			nominal = false;
		}
		auto* alchemyItemManager = TESObjectALCIHook::GetSingleton();
		if (!alchemyItemManager) {
			logger::critical("  >Failed to get alchemyItemManager singleton."sv);
			nominal = false;
		}

		if (!nominal) {
			return false;
		}

		return miscManager->Install(RE::Offset::TESObjectMISC::VTABLE) && 
			alchemyItemManager->Install(RE::Offset::AlchemyItem::VTABLE);
	}

    bool TESObjectMiscHook::Install(REL::ID a_vtableAddress) {
		logger::info("Checking for Misc Objects hook."sv);
		try {
			auto response = HookIfNecessary(a_vtableAddress, 0x47, setting, Thunk);
			if (response.has_value()) {
				_func = response.value();
				logger::info("  >Hook installed."sv);
			}
			else {
				logger::info("  >Player disabled this hook."sv);
			}
		}
		catch (std::exception&) {
			return false;
		}
		return true;
    }

	RE::NiAVObject* TESObjectMiscHook::Thunk(RE::TESObject* a_this, 
		RE::TESObjectREFR* a_ref)
	{
		auto* replacer = ModelReplacer::Swapper::GetSingleton();
		if (replacer) {
			auto* response = replacer->AttemptModelSwap(a_this, a_ref);
			return response ? response : _func(a_this, a_ref);
		}
		return _func(a_this, a_ref);
	}

	inline bool TESObjectALCIHook::Install(REL::ID a_vtableAddress) {
		logger::info("Checking for Alchemy Item hook."sv);
		try {
			auto response = HookIfNecessary(a_vtableAddress, 0x47, setting, Thunk);
			if (response.has_value()) {
				_func = response.value();
				logger::info("  >Hook installed."sv);
			}
			else {
				logger::info("  >Player disabled this hook."sv);
			}
		}
		catch (std::exception&) {
			return false;
		}
		return true;
	}

	inline RE::NiAVObject* TESObjectALCIHook::Thunk(RE::TESObject* a_this,
		RE::TESObjectREFR* a_ref) 
	{
		auto* replacer = ModelReplacer::Swapper::GetSingleton();
		if (replacer) {
			auto* response = replacer->AttemptModelSwap(a_this, a_ref);
			return response ? response : _func(a_this, a_ref);
		}
		return _func(a_this, a_ref);
	}
}