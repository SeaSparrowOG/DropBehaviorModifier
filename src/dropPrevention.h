#pragma once

namespace DropPrevention {
    class DropObject : public ISingleton<DropObject> {
    public:
        static void Install();
        static void RegisterUndropableObject(RE::TESBoundObject* a_obj);
        static bool IsObjectRegistered(RE::TESBoundObject* a_obj);
        static RE::ObjectRefHandle* thunk(RE::Character* a_this, RE::ObjectRefHandle* a_hidden_return_argument, 
            RE::TESBoundObject* a_item, std::int32_t a_count, RE::ITEM_REMOVE_REASON a_reason, 
            RE::ExtraDataList* a_extraList, RE::TESObjectREFR* a_moveToRef, const RE::NiPoint3* a_dropLoc, 
            const RE::NiPoint3* a_rotate);

        inline static REL::Relocation<decltype(&thunk)> func;
        inline static constexpr std::size_t idx = (0xae0 - 0x830) / 0x8;

        inline static std::vector<RE::TESBoundObject*> undropableObjects;
    };
}