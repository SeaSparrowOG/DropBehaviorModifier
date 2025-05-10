# Drop Behavior Modifier
SKSE plugin that supports my model replacement mod. Replaces a given reference's model if the count of it matches a parameter.

## Requirements
* [CMake](https://cmake.org/)
	* Add this to your `PATH`
* [Vcpkg](https://github.com/microsoft/vcpkg)
	* Add the environment variable `VCPKG_ROOT` with the value as the path to the folder containing vcpkg
* [Visual Studio Community 2022](https://visualstudio.microsoft.com/)
	* Desktop development with C++

## Register Visual Studio as a Generator
* Open `x64 Native Tools Command Prompt`
* Run `cmake`
* Close the cmd window

## Building
```
git clone https://github.com/SeaSparrowOG/drop-behavior-modifier
cd drop-behavior-modifier
cmake --preset vs2022-windows
cmake --build build --config Release
```

## Usage
On its own, the DLL doesn't do anything. In order to swap models, you need to create a configuration file. Configuration files are `.json` files found in `/Data/SKSE/Plugins/DropBehaviorModifier`. Any `.json` file there will count a as a configuration file, and be evaluated. If any part of it is invalid, it is discarded. If you are developing a companion mod, make sure to check the log found in `Documents/My Games/Skyrim Special Edition/SKSE/DropBehaviorModifier.log` if things are not working as expected, since it points out common mistakes.

### Configuration - Top Level
Your JSON file should look like this when you are starting out:
```json
{
  "MinimumVersion": 1,
  "Swaps": [ ]
}
```
Fields:
* MinimumVersion:
	* The minimum parser version needed for the config to be considered valid. This is a system that is in place to account for new features being released in the future that are not backwards compatible. In short, always use the latest version. If you are unsure what the latest version is, run the game and check the log.
* Swaps:
	* An array of objects. Each object represents a "swap", ie what model to assign to a reference if it has at least count X and is of base form Y.

### Configuration - Swaps
The following fields should be placed in `"Swaps"`, but the explanation omits the top fields for clarity. See the examples for proper formatting.
```json
{
  {
    "BaseObject": "",
    "AltModels": [
      {
        "Count": 3,
        "Model": "",
        "AltTextures": []
      }
    ]
  }
}
```
Fields:
* BaseObject:
    * The form that will be assigned a dynamic model. This is a string field. Internally, I call them "formatted" strings, because in order to be valid they need to follow a specific format.
        * Formatted String: `Modname.extension|0xFormID`. For example, `Skyrim.esm|0xA` is the form `0xA` in `Skyrim.esm` (a lockpick).
        * EditorID: Alternatively, if you want to make [PowerOfThree's Tweaks](https://www.nexusmods.com/skyrimspecialedition/mods/51073) a master, you can also use EditorIDs. For example, `IronSword`.
        * If a base object is not found, it's not the end of the world. A small warning is printed in the log, but the config is considered valid.
    * Alternatively, if you want the model swap to apply to multiple objects, you can use an array of strings. Check `Example 4`!
* AltModels:
    * An array of objects. Each object represents a different model that will get assigned to the form if it has a specified count.
* Count:
    * The minimum count in a single reference needed for the swap to be valid. This is an int field.
        * MUST be 2 or larger.
* Model:
    * The path to the mesh for that swap. This is an int field.
    * Be aware, this should NOT include `Meshes\\`, or forward slashes `/`. The log will yell at you if you do that.
    * This must ALWAYS point to a valid mesh.
* AltTextures:
    * An array of objects representing alternate textures to apply to the reference.
    * This is an optional field, but must be valid if present.

### Configuration - Alternate Textures
The following objects should be placed in `"AltTextures"`, but the explanation omits the top fields for clarity. See the examples for proper formatting.
```json
{
  "Target": "MiscSackLargeFlat03:0",
  "Diffuse": "SeaSparrow\\DynamicDroppedObjects\\LargeBurlapApples.dds"
}
```
Fields:
* Target:
    * The path to the `BSTriShape` that contains the `BSLightingShaderProperty` that in turn contains the `BSShaderTextureSet`.
    * If the node is not nested, just the name is enough. For example, `MiscSackLarge` has `MiscSackLargeFlat03:0`.
    * If it IS nested, you need to note the path. For example, a theoretical `SuperSack` would be: `SuperSack01:0|SuperSackSecretCompartment02:1`. Top node is `SuperSack0101:0`, and one of its children is `SuperSackSecretCompartment02:1`, which is the node the alt texture will be placed into.
* Diffuse:
    * The path to the texture that you want to apply. Follows similar rules to the `"Model"` field - Make sure it doesn't start with `Textures\\` and don't use `/`.

## Configuration - Examples:
Example 1: 
- Replace 3+ dropped apples with a bowl with apples.
- Replace 7+ dropped apples with a bucket with apples.
- Replace 5+ dropped bread loaves with a basket of bread.
```json
{
  "MinimumVersion": 1,
  "Swaps": [
    {
      "BaseObject": "Skyrim.esm|0x64B2E",
      "AltModels": [
        {
          "Count": 3,
          "Model": "SeaSparrow\\DynamicDroppedObjects\\AppleBowl.nif"
        },
        {
          "Count": 7,
          "Model": "SeaSparrow\\DynamicDroppedObjects\\AppleBucket.nif"
        }
      ]
    },
    {
      "BaseObject": "Skyrim.esm|0x65C97",
      "AltModels": [
        {
          "Count": 3,
          "Model": "SeaSparrow\\DynamicDroppedObjects\\BreadBasket.nif"
        }
      ]
    }
  ]
}
```
Example 2:
- Replace 5+ dropped lockpicks with a small pouch.
- Replace 10+ dropped lockpicks with the same pouch, but use a gold texture!
```json
{
  "MinimumVersion": 1,
  "Swaps": [
    {
      "BaseObject": "Skyrim.esm|0xA",
      "AltModels": [
        {
          "Count": 5,
          "Model": "SeaSparrow\\DynamicDroppedObjects\\LockpickPouch.nif"
        },
        {
          "Count": 10,
          "Model": "SeaSparrow\\DynamicDroppedObjects\\LockpickPouch.nif",
          "AltTextures": [
            {
              "Target": "PouchTexture01:1",
              "Diffuse": "SeaSparrow\\DynamicDroppedObjects\\GoldenPouchTexture.dds"
            }
          ]
        }
      ]
    }
  ]
}
```
Example 3:
- Replace 10+ dropped soul gem fragments with a small mound of soul gem fragments, and make the top objects red in color.
```json
{
  "MinimumVersion": 1,
  "Swaps": [
    {
      "BaseObject": "Skyrim.esm|0x67181",
      "AltModels": [
        {
          "Count": 10,
          "Model": "SeaSparrow\\DynamicDroppedObjects\\LockpickPouch.nif",
          "AltTextures": [
            {
              "Target": "TopGem01:1",
              "Diffuse": "SeaSparrow\\DynamicDroppedObjects\\RedGemTexture.dds"
            },
            {
              "Target": "TopGem02:5",
              "Diffuse": "SeaSparrow\\DynamicDroppedObjects\\RedGemTexture.dds"
            }
          ]
        }
      ]
    }
  ]
}
```
Example 4:
- Replace 10+ dropped grand soul gems (filled and non-filled) with a small pouch of soul gems.
 - This example uses EditorIDs, so it needs PO3's tweaks.
 - Since there is a single model swap that we want to apply to multiple forms, we are also filling `"BaseObject"` with an array of strings instead of just 1 string.
 - We are also using some of the new forms from a mod called [Yet Another Soul TrapM anager](https://www.nexusmods.com/skyrimspecialedition/mods/56144). If the mod is not present, the forms will be missing, but it won't cause an error for the user.
```json
{
  "MinimumVersion": 1,
  "Swaps": [
    {
      "BaseObject": [
        "SoulGemGrand",
        "SoulGemGrandFilled",
        "SoulGemGrandFilledPetty",
        "SoulGemGrandFilledLesser",
        "SoulGemGrandFilledCommon",
        "SoulGemGrandFilledGreater"
      ],
      "AltModels": [
        {
          "Count": 10,
          "Model": "SeaSparrow\\DynamicDroppedObjects\\SoulGemPouch.nif"
        }
      ]
    }
  ]
}
```