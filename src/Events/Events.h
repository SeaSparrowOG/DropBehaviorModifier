#pragma once

namespace Events
{
#define EventClass Utilities::Singleton::EventClass

	bool Install();

	// But why is this needed? 
	// Because the inventory changes system is weird when it comes to ingredients, specifically.
	// Dropping ingredients can split the stack, which has the unfortunate effect of generating a single
	// NiAVObject to be used for all of them (the single count one). So, some "flickering" is needed
	// to correct the mesh after it has loaded.

	class LoadListener :
		public EventClass<LoadListener, RE::TESObjectLoadedEvent> {
	private:
		RE::BSEventNotifyControl ProcessEvent
		(const RE::TESObjectLoadedEvent* a_event,
			RE::BSTEventSource<RE::TESObjectLoadedEvent>*) override;
	};
}