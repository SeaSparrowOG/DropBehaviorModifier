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
        ArmorHooks::GetSingleton()->Install();
        BookHooks::GetSingleton()->Install();
        IngredientHooks::GetSingleton()->Install();
        MiscHooks::GetSingleton()->Install();
        SoulGemHooks::GetSingleton()->Install();
        WeaponHooks::GetSingleton()->Install();
    }
}