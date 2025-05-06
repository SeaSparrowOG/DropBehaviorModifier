#pragma once

namespace ModelReplacer
{
	enum class SwapRegistrationReport : int8_t
	{
		kSuccess = 0,
		kCountExists
	};

	class TextureSwap
	{
	private:
		std::vector<std::string> nodePath{};
		std::string              texturePath{};

	public:
		void Apply(RE::NiAVObject* a_target);

		TextureSwap(const std::vector<std::string>& a_nodePath, 
			const std::string& a_texturePath);
	};

	class ModelSwap
	{
	private:
		std::vector<TextureSwap> altTextures{};
		std::string              altModel{};
		int32_t                  requiredCount{ 100 };

	public:
		bool Matches(int32_t a_otherCount) const {
			return a_otherCount >= requiredCount;
		}

		int32_t GetRequiredCount() const {
			return requiredCount;
		}

		RE::NiAVObject* ConstructGraphics();

		ModelSwap(int32_t a_count, 
			const std::string& a_newModel, 
			const std::vector<TextureSwap>& a_textureSwaps);
	};

	class Swapper : public ISingleton<Swapper>
	{
	public:
		RE::NiAVObject* AttemptModelSwap(RE::TESObject* a_base, RE::TESObjectREFR* a_ref);

		SwapRegistrationReport RegisterSwap(RE::TESBoundObject* a_form, ModelSwap a_newSwap);

	private:
		ModelSwap* BestMatch(RE::TESBoundObject* a_base, int32_t a_count);
		SwapRegistrationReport RegisterAdditionalSwap(ModelSwap a_newSwap,
			std::vector<ModelSwap>& a_oldSwaps);

		std::unordered_map<RE::TESBoundObject*, std::vector<ModelSwap>> m_registeredSwaps{};
	};
}