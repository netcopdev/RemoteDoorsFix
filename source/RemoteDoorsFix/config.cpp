class CfgPatches
{
    class RemoteDoorsFix_Scripts
    {
        units[] = {};
        weapons[] = {};
        requiredVersion = 0.1;

        // RemoteDoors MUST appear before @RemoteDoorsFix in -mod.
        // We deliberately avoid hard-coding RemoteDoors' CfgPatches name,
        // because the original Workshop PBO is distributed only as config.bin.
        requiredAddons[] = {"DZ_Scripts"};
    };
};

class CfgMods
{
    class RemoteDoorsFix
    {
        dir = "RemoteDoorsFix";
        name = "RemoteDoorsFix";
        author = "Compatibility patch";
        type = "mod";

        dependencies[] = {"World"};

        class defs
        {
            class worldScriptModule
            {
                value = "";
                files[] = {"RemoteDoorsFix/Scripts/4_World"};
            };
        };
    };
};
