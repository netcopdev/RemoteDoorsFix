class CfgPatches
{
    class RemoteDoorsFix_Scripts
    {
        units[] = {};
        weapons[] = {};
        requiredVersion = 0.1;

        // RemoteDoors 1.30.0 declares CfgPatches.RemoteDoors.
        // Resolve its World scripts before extending the upstream controller.
        requiredAddons[] = {"DZ_Scripts", "RemoteDoors"};
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
