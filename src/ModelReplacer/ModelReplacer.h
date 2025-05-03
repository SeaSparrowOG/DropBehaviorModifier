#pragma once

namespace ModelReplacer
{
	class Swapper : public ISingleton<Swapper>
	{
	public:
		void AttemptModelSwap(RE::TESBoundObject* a_base);
	};
}