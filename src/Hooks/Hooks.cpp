#include "Hooks.h"

#include <xbyak.h>

#include "ModelReplacer/ModelReplacer.h"

namespace Hooks 
{
	bool Install() {
		logger::info("==========================================================");
		logger::info("Installing Hooks...");
		SKSE::AllocTrampoline(34);
		return LoadListener::GetSingleton()->Install();
	}

	bool LoadListener::Install() {
		return LoadGraphicsPrologue::Install();
	}

	bool LoadListener::LoadGraphicsPrologue::Install() {
        REL::Relocation<std::uintptr_t> target{ REL::ID(17653) }; // TESObject::LoadGraphics

        struct Patch : Xbyak::CodeGenerator
        {
            Patch(std::uintptr_t a_originalFuncAddr, std::size_t a_originalByteLength)
            {
                for (size_t i = 0; i < a_originalByteLength; i++) {
                    db(*reinterpret_cast<uint8_t*>(a_originalFuncAddr + i));
                }

                jmp(qword[rip]);
                dq(a_originalFuncAddr + a_originalByteLength);
            }
        };

        Patch p(target.address(), 6);
        p.ready();

        auto& trampoline = SKSE::GetTrampoline();
        trampoline.write_branch<5>(target.address(), Thunk);

        auto alloc = trampoline.allocate(p.getSize());
        memcpy(alloc, p.getCode(), p.getSize());

        _func = reinterpret_cast<std::uintptr_t>(alloc);
        return true;
	}

	RE::NiAVObject* LoadListener::LoadGraphicsPrologue::Thunk(RE::TESObject* a_this, RE::TESObjectREFR* a_ref) {
        auto* replacer = ModelReplacer::Swapper::GetSingleton();
        if (replacer) {
            auto* response = replacer->AttemptModelSwap(a_this, a_ref);
            return response ? response : _func(a_this, a_ref);
        }
        return _func(a_this, a_ref);
	}
}