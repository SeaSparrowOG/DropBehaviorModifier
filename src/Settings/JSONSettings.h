#pragma once

namespace Settings
{
	namespace JSON
	{
		class Holder : 
			public ISingleton<Holder>
		{
		public:
			bool Read();

		private:
			std::string MIN_VERSION_FIELD{ "MinimumVersion" };
			std::string CONDITIONAL_COUNT_FIELD{ "Count" };
			std::string CONDITIONAL_MODEL_FIELD{ "Model" };

			struct Candidate
			{
				Candidate(RE::TESBoundObject* a_base, 
					const std::vector<std::pair<int32_t, std::string>>& a_swaps) 
				{
					baseForm = a_base;
					swaps = a_swaps;
				}

				RE::TESBoundObject* baseForm{ nullptr };
				std::vector<std::pair<int32_t, std::string>> swaps{};
			};

			void ReadConfig(const Json::Value& a_json);
			bool CreateNewEntry(const Json::Value& a_json, 
				std::vector<std::pair<int32_t, std::string>>& a_results);
		};
	}
}
