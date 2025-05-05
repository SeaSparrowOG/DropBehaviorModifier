#pragma once

#include "offset.h"

namespace RE
{
	// Thanks PO3
	namespace BSModelDB
	{
		struct DBTraits
		{
		public:
			inline static constexpr auto          RTTI = RTTI_BSModelDB__DBTraits;
			inline static constexpr std::uint32_t LOAD_QUEUE_SIZE = 8;
			inline static constexpr std::uint32_t RELEASE_QUEUE_SIZE = 2;

			using U_Type = NiPointer<NiNode>;

			struct ArgsType
			{
			public:
				// members
				std::uint32_t LODmult{ 0 };         // 0
				std::uint32_t texLoadLevel{ 3 };    // 4
				bool          unk8{ true };         // 8
				bool          unk9{ false };        // 9
				bool          unkA{ true };         // A
				bool          postProcess{ true };  // B
			};
			static_assert(sizeof(ArgsType) == 0xC);
		};
		static_assert(std::is_empty_v<DBTraits>);
	}

	// Extra thank you to PO3
	int Demand(const char* a_modelPath, NiPointer<NiNode>& a_modelOut, const BSModelDB::DBTraits::ArgsType& a_args)
	{
		using func_t = decltype(&Demand);
		static REL::Relocation<func_t> func{ REL::ID(75782) };
		return func(a_modelPath, a_modelOut, a_args);
	}

	NiAVObject* CloneNiAVObject(NiAVObject* a_original)
	{
		using func_t = decltype(&CloneNiAVObject);
		static REL::Relocation<func_t> func{ REL::ID(70187) };
		return func(a_original);
	}

	// Probably unneeded
	inline int GetModelType(RE::TESForm* a_form)
	{
		using func_t = decltype(&GetModelType);
		static REL::Relocation<func_t> func{ REL::ID(13413) };
		return func(a_form);
	}
}