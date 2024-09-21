#pragma once

#include "referenceManager.h"

namespace Hooks {
	template <typename T, class E>
	class Abstract {
	public:
		static E* GetSingleton() {
			static E singleton;
			return std::addressof(singleton);
		}

		void Install() {
			REL::Relocation<std::uintptr_t> vTable(T::VTABLE[0]);
			func = vTable.write_vfunc(0x47, &thunk);
		}

	protected:
		Abstract() = default;
		~Abstract() = default;

		Abstract(const Abstract&) = delete;
		Abstract(Abstract&&) = delete;
		Abstract& operator=(const Abstract&) = delete;
		Abstract& operator=(Abstract&&) = delete;

	private:
		static RE::NiAVObject* LoadGraphics(RE::TESObject* a_obj, RE::TESObjectREFR* a_ref)
		{
			using func_t = decltype(&LoadGraphics);
			static REL::Relocation<func_t> sub{ REL::ID(17653) };
			return sub(a_obj, a_ref);
		}

		static RE::NiNode* thunk(RE::TESObject* a1, RE::TESObjectREFR* a2) {
			if (a2 && a2->extraList.GetCount() > 1) {
				auto* newMesh = Manager::Manipulator::GetSingleton()->GetPair(a2);
				if (newMesh) {
					auto* newGraphics = LoadGraphics(newMesh, nullptr)->Clone();
					a2->Set3D(newGraphics);
					a2->Update3DPosition(true);
				}
			}
			return func(a1, a2);
		}

		static inline REL::Relocation<decltype(thunk)> func;
	};
}