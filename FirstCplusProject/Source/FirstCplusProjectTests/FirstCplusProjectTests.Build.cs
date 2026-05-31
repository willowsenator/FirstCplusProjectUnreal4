using UnrealBuildTool;

public class FirstCplusProjectTests : ModuleRules
{
    public FirstCplusProjectTests(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            [
                "Core"
            ]
        );

        PrivateDependencyModuleNames.AddRange(
            [
                "CoreUObject",
                "Engine",
                "UnrealEd",
                "FirstCplusProject",
                "Slate",
                "SlateCore"
            ]
        );
    }
}