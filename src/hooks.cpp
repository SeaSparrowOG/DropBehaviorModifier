#include "hooks.h"

#include "Hooks/alchemyItemsHooks.h"
#include "Hooks/armorHooks.h"
#include "Hooks/bookHooks.h"
#include "Hooks/ingredientHooks.h"
#include "Hooks/miscHooks.h"
#include "Hooks/soulgemHooks.h"
#include "Hooks/weapHooks.h"


namespace Hooks {
    void Install()
    {
        AlchemyItemHooks::GetSingleton()->Install();
        _loggerInfo("Installed vfunc hook for Alchemy Items.");
        ArmorHooks::GetSingleton()->Install();
        _loggerInfo("Installed vfunc hook for Armor Items.");
        BookHooks::GetSingleton()->Install();
        _loggerInfo("Installed vfunc hook for Book Items.");
        IngredientHooks::GetSingleton()->Install();
        _loggerInfo("Installed vfunc hook for Ingredient Items.");
        MiscHooks::GetSingleton()->Install();
        _loggerInfo("Installed vfunc hook for Misc Items.");
        SoulGemHooks::GetSingleton()->Install();
        _loggerInfo("Installed vfunc hook for Soul Gem Items.");
        WeaponHooks::GetSingleton()->Install();
        _loggerInfo("Installed vfunc hook for Weapon Items.");
    }
}