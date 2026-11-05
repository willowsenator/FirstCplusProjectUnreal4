using UnrealBuildTool;

public class FirstCplusProjectTests : ModuleRules
{
    public FirstCplusProjectTests(ReadOnlyTargetRules target) : base(target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

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
                "SlateCore",
                "UMG"
            ]
        );
    }
}