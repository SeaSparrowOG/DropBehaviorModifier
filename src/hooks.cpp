#include "hooks.h"

#include "Hooks/armorHooks.h"
#include "Hooks/bookHooks.h"
#include "Hooks/miscHooks.h"
#include "Hooks/soulgemHooks.h"
#include "Hooks/weapHooks.h"


namespace Hooks {
    void Install()
    {
        ArmorHooks::GetSingleton()->Install();
        BookHooks::GetSingleton()->Install();
        MiscHooks::GetSingleton()->Install();
        SoulGemHooks::GetSingleton()->Install();
        WeaponHooks::GetSingleton()->Install();
    }
}