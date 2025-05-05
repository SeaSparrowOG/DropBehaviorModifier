#include "ModelReplacer.h"

#include "RE//Misc.h"

namespace ModelReplacer
{
	RE::NiAVObject* Swapper::AttemptModelSwap(RE::TESObject* a_base,
		RE::TESObjectREFR* a_ref)
	{
		const auto count = a_ref ? a_ref->extraList.GetCount() : 0;
		if (count == 0) {
			LOG_DEBUG("Object with count 0"sv);
			return nullptr;
		}

		auto* bound = a_base ? skyrim_cast<RE::TESBoundObject*>(a_base) : nullptr;
		if (!bound) {
			LOG_DEBUG("Failed to cast as Bound Object."sv);
			return nullptr;
		}
		else if (!m_registeredSwaps.contains(bound)) {
			return nullptr;
		}

		auto& swapHolder = m_registeredSwaps.at(bound);
		auto newModel = swapHolder.BestMatch(count);
		if (newModel.empty()) {
			LOG_DEBUG("New model not found."sv);
			return nullptr;
		}

		const auto args = RE::BSModelDB::DBTraits::ArgsType();
		auto out = RE::NiPointer<RE::NiNode>();
		int error = RE::Demand(newModel.c_str(), out, args);
		if (error != 0) {
			LOG_DEBUG("Errored on demand."sv);
			return nullptr;
		}

		LOG_DEBUG("Replaced {}'s model with {}", a_base->GetName(), newModel);
		return out && out.get() ? RE::CloneNiAVObject(out.get()) : nullptr;
	}

	SwapRegistrationReport Swapper::RegisterSwap(RE::TESBoundObject* a_form, 
		int32_t a_count, 
		const std::string& a_modelPath) 
	{
		if (m_registeredSwaps.contains(a_form)) {
			auto& existingSwap = m_registeredSwaps[a_form];
			return existingSwap.RegisterAdditionalSwap(a_modelPath, a_count);
		}

		auto newSwap = ModelSwap();
		auto response = newSwap.RegisterAdditionalSwap(a_modelPath, a_count);
		if (response != SwapRegistrationReport::kSuccess) {
			return response;
		}

		m_registeredSwaps[a_form] = newSwap;
		return response;
	}
}