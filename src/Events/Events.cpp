#include "Events.h"

#include "ModelReplacer/ModelReplacer.h"

namespace Events
{
	void Enable_ObjectREFR(RE::TESObjectREFR* a_this, bool a_resetInventory)
	{
		using func_t = decltype(&Enable_ObjectREFR);
		static REL::Relocation<func_t> func{ REL::ID(19800) };
		return func(a_this, a_resetInventory);
	}

	bool Install() {
		logger::info("Registering Event Listeners..."sv);
		auto* listenerSingleton = LoadListener::GetSingleton();
		if (!listenerSingleton) {
			logger::error("  >Failed to grab LoadListener singleton."sv);
			return false;
		}
		logger::info("  >Registering TES Cell Attach/Detach Event Listener..."sv);
		return listenerSingleton->RegisterListener();
	}

	RE::BSEventNotifyControl LoadListener::ProcessEvent
	(const RE::TESObjectLoadedEvent* a_event,
		RE::BSTEventSource<RE::TESObjectLoadedEvent>*)
	{
		using control = RE::BSEventNotifyControl;
		if (!a_event || !a_event->loaded) {
			return control::kContinue;
		}

		auto* ui = RE::UI::GetSingleton();
		if (!ui || 
			!ui->IsMenuOpen(RE::InventoryMenu::MENU_NAME))
		{
			return control::kContinue;
		}

		auto* eventRef = RE::TESForm::LookupByID<RE::TESObjectREFR>(a_event->formID);
		auto* eventBase = eventRef ? eventRef->GetBaseObject() : nullptr;
		auto eventCount = eventBase ? eventRef->extraList.GetCount() : 0;

		if (eventCount < 2) {
			return control::kContinue;
		}

		bool isValid = false;
		if (const auto* asIngredient = eventBase->As<RE::IngredientItem>(); asIngredient) {
			isValid = true;
		}

		if (!isValid) {
			return control::kContinue;
		}

		auto* ref3D = eventRef->Get3D();
		auto* replacer = ModelReplacer::Swapper::GetSingleton();
		auto* altModel = replacer ?
			replacer->AttemptModelSwap(eventBase, eventRef) :
			nullptr;

		if (ref3D && altModel && altModel->name != ref3D->name) {
			eventRef->Disable();
			Enable_ObjectREFR(eventRef, false);
		}

		return control::kContinue;
	}
}