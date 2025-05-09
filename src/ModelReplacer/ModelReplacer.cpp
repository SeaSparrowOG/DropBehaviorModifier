#include "ModelReplacer.h"

#include "RE/Misc.h"

namespace ModelReplacer
{
	RE::NiAVObject* Swapper::AttemptModelSwap(RE::TESBoundObject* a_base,
		RE::TESObjectREFR* a_ref)
	{
		const auto count = a_ref && a_ref->extraList.HasType(RE::ExtraDataType::kCount) ? 
			a_ref->extraList.GetCount() : 0;
		if (count == 0) {
			return nullptr;
		}

		// LoadGraphics is also called to set the NiAVObject in the menu's display object.
		// However, the count is ALWAYS 100. Which "???", but I can get an accurate count
		// from the item list. Not now, though.
		auto* ui = RE::UI::GetSingleton();
		if (ui && (
			ui->IsMenuOpen(RE::MagicMenu::MENU_NAME) ||
			ui->IsMenuOpen(RE::GiftMenu::MENU_NAME) ||
			ui->IsMenuOpen(RE::ContainerMenu::MENU_NAME) ||
			ui->IsMenuOpen(RE::BarterMenu::MENU_NAME) ||
			ui->IsMenuOpen(RE::InventoryMenu::MENU_NAME)
			)) 
		{
			return nullptr;
		}

		LOG_DEBUG("Object: {}", Utilities::EDID::GetEditorID(a_base));
		auto* bestMatch = BestMatch(a_base, count);
		if (!bestMatch) {
			LOG_DEBUG("  >No match."sv);
			return nullptr;
		}
		return bestMatch->ConstructGraphics();
	}

	SwapRegistrationReport Swapper::RegisterSwap(RE::TESBoundObject* a_form, ModelSwap a_newSwap)
	{
		if (m_registeredSwaps.contains(a_form)) {
			return RegisterAdditionalSwap(a_newSwap, m_registeredSwaps[a_form]);
		}

		auto newVector = std::vector<ModelSwap>();
		newVector.push_back(a_newSwap);
		m_registeredSwaps[a_form] = newVector;
		return SwapRegistrationReport::kSuccess;
	}

	ModelSwap* Swapper::BestMatch(RE::TESBoundObject* a_base, int32_t a_count) {
		if (!m_registeredSwaps.contains(a_base)) {
			LOG_DEBUG("  Unregistered."sv);
			return nullptr;
		}

		auto& candidates = m_registeredSwaps[a_base];
		auto finish = candidates.end();
		for (auto it = candidates.begin(); it != finish; ++it) {
			auto& swap = *it;
			if (swap.Matches(a_count)) {
				LOG_DEBUG("  >Found Graphics"sv);
				return &swap;
			}
		}
		LOG_DEBUG("  >No matching graphics."sv);
		return nullptr;
	}

	SwapRegistrationReport Swapper::RegisterAdditionalSwap(ModelSwap a_newSwap, 
		std::vector<ModelSwap>& a_oldSwaps) 
	{
		if (!a_oldSwaps.empty()) {
			auto finish = a_oldSwaps.end();
			for (auto it = a_oldSwaps.begin(); it != finish; ++it) {
				const auto& swap = *it;
				if (swap.GetRequiredCount() == a_newSwap.GetRequiredCount()) {
					return SwapRegistrationReport::kCountExists;
				}
			}
		}

		a_oldSwaps.push_back(a_newSwap);
		std::sort(a_oldSwaps.begin(), a_oldSwaps.end(),
			[](ModelSwap& lhs, ModelSwap& rhs) {
				return lhs.GetRequiredCount() > rhs.GetRequiredCount();
			});
		return SwapRegistrationReport::kSuccess;
	}
	
	void TextureSwap::Apply(RE::NiAVObject* a_target) {
		auto texturePtr = RE::NiTexturePtr();
		RE::GetTexture(texturePath.c_str(), true, texturePtr, false);
		if (!texturePtr || !texturePtr.get()) {
			return;
		}
		auto* newTexture = netimmerse_cast<RE::NiSourceTexture*>(texturePtr.get());
		if (!newTexture) {
			return;
		}

		auto* targetNode = a_target;
		for (const auto& node : nodePath) {
			if (!targetNode) {
				LOG_DEBUG("No target node in for-loop."sv);
				return;
			}
			targetNode = targetNode->GetObjectByName(node);
		}
		if (!targetNode) {
			LOG_DEBUG("No target node after for-loop."sv);
			return;
		}

		auto* bsTriShape = targetNode->AsTriShape();
		auto* properties = bsTriShape ? bsTriShape->properties[1].get() : nullptr;
		auto* bsLightShader = properties ? netimmerse_cast<RE::BSLightingShaderProperty*>(properties) : nullptr;
		auto* shaderMaterial = bsLightShader ? bsLightShader->material : nullptr;
		auto* base = shaderMaterial ? skyrim_cast<RE::BSLightingShaderMaterialBase*>(shaderMaterial) : nullptr;
		if (!base) {
			LOG_DEBUG("Failed to cast."sv);
			return;
		}

		auto baseTextureSet = base->GetTextureSet();
		if (!baseTextureSet) {
			LOG_DEBUG("Failed to get texture set."sv);
			return;
		}

		auto assign = RE::NiSourceTexturePtr(newTexture);
		base->diffuseTexture = assign;
	}

	TextureSwap::TextureSwap(const std::vector<std::string>& a_nodePath, 
		const std::string& a_texturePath)
	{
		this->nodePath = a_nodePath;
		this->texturePath = a_texturePath;
	}

	RE::NiAVObject* ModelSwap::ConstructGraphics() {
		const auto args = RE::BSModelDB::DBTraits::ArgsType();
		auto out = RE::NiPointer<RE::NiNode>();
		int error = RE::Demand(altModel.c_str(), out, args);
		if (error != 0) {
			LOG_DEBUG("Errored on demand."sv);
			return nullptr;
		}

		RE::NiAVObject* constructedObject = out && out.get() ? out.get() : nullptr;
		if (!constructedObject) {
			LOG_DEBUG("Failed to get constructed object."sv);
			return nullptr;
		}

		if (!altTextures.empty()) {
			for (auto& texSwap : altTextures) {
				texSwap.Apply(constructedObject);
			}
		}

		auto* clone = RE::CloneNiAVObject(constructedObject);
		return clone;
	}

	ModelSwap::ModelSwap(int32_t a_count, 
		const std::string& a_newModel, 
		const std::vector<TextureSwap>& a_textureSwaps)
	{
		this->requiredCount = a_count;
		this->altModel = a_newModel;
		this->altTextures = a_textureSwaps;
	}
}