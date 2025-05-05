#pragma once

namespace ModelReplacer
{
	enum class SwapRegistrationReport : int8_t
	{
		kSuccess = 0,
		kCountExists
	};

	struct ModelSwap
	{
	private:
		std::vector<std::pair<int32_t, std::string>> replacements{};
	public:
		std::string BestMatch(int32_t a_count) {
			auto finish = replacements.end();
			for (auto it = replacements.begin(); it != finish; ++it) {
				const auto& [count, path] = *it;
				if (a_count >= count) {
					return path;
				}
			}
			return "";
		}

		SwapRegistrationReport RegisterAdditionalSwap(std::string a_modelPath, int32_t a_minCount) {
			if (!replacements.empty()) {
				auto finish = replacements.end();
				for (auto it = replacements.begin(); it != finish; ++it) {
					const auto& [count, path] = *it;
					if (count == a_minCount) {
						return SwapRegistrationReport::kCountExists;
					}
				}
			}

			replacements.emplace_back(a_minCount, a_modelPath);
			std::sort(replacements.begin(), replacements.end(),
				[](const auto& lhs, const auto& rhs) {
					return lhs.first > rhs.first;
				});
			return SwapRegistrationReport::kSuccess;
		}
	};

	class Swapper : public ISingleton<Swapper>
	{
	public:
		RE::NiAVObject* AttemptModelSwap(RE::TESObject* a_base, RE::TESObjectREFR* a_ref);

		SwapRegistrationReport RegisterSwap(RE::TESBoundObject* a_form, int32_t a_count, const std::string& a_modelPath);

	private:
		std::unordered_map<RE::TESBoundObject*, ModelSwap> m_registeredSwaps{};
	};
}