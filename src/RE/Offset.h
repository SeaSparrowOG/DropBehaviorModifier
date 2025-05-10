#pragma once

namespace RE
{
	namespace Offset
	{
		// Used by Hooks.h
		namespace AlchemyItem
		{
			constexpr auto VTABLE = REL::ID(186204);
		}
		namespace IngredientItem
		{
			constexpr auto VTABLE = REL::ID(186407);
		}
		namespace TESObjectARMO
		{
			constexpr auto VTABLE = REL::ID(189542);
		}
		namespace TESObjectWEAP
		{
			constexpr auto VTABLE = REL::ID(189786);
		}
		namespace TESObjectBOOK
		{
			constexpr auto VTABLE = REL::ID(189577);
		}
		namespace TESObjectMISC
		{
			constexpr auto VTABLE = REL::ID(189689);
		}
		namespace TESSoulGem
		{
			constexpr auto VTABLE = REL::ID(189850);
		}

		// Used by Misc.h. The offsets were nabbed from PO3's commonlib.
		namespace NiAVObject
		{
			constexpr auto Demand = REL::ID(75782);
		}
		namespace NiAVObject
		{
			constexpr auto Clone = REL::ID(70187);
		}
		namespace NiTexture
		{
			constexpr auto GetTexture = REL::ID(105640);
		}
	}
}