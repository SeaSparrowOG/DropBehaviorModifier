#include "dropPrevention.h"

namespace DropPrevention {
    RE::ObjectRefHandle* DropObject::thunk(RE::Character* a_this, RE::ObjectRefHandle* a_hidden_return_argument, 
        RE::TESBoundObject* a_item, std::int32_t a_count, RE::ITEM_REMOVE_REASON a_reason, 
        RE::ExtraDataList* a_extraList, RE::TESObjectREFR* a_moveToRef, const RE::NiPoint3* a_dropLoc, 
        const RE::NiPoint3* a_rotate) {
        
        if (IsObjectRegistered(a_item) && a_reason == RE::ITEM_REMOVE_REASON::kDropping) {
            return func(a_this, a_hidden_return_argument, nullptr, a_count, a_reason, a_extraList, a_moveToRef, a_dropLoc, a_rotate);
        }
        return func(a_this, a_hidden_return_argument, a_item, a_count, a_reason, a_extraList, a_moveToRef, a_dropLoc, a_rotate);
    }

    void DropObject::Install()
    {
        REL::Relocation<std::uintptr_t> vTable(RE::PlayerCharacter::VTABLE[0]);
        func = vTable.write_vfunc(idx, &thunk);
    }

    void DropObject::RegisterUndropableObject(RE::TESBoundObject* a_obj)
    {
        if (IsObjectRegistered(a_obj)) return;
        undropableObjects.push_back(a_obj);
    }

    bool DropObject::IsObjectRegistered(RE::TESBoundObject* a_obj)
    {
        return std::find(undropableObjects.begin(), undropableObjects.end(), a_obj) != undropableObjects.end();
    }
}