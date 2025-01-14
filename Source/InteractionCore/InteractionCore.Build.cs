using UnrealBuildTool;

public class InteractionCore : ModuleRules
{
    public InteractionCore(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new []
        { 
            "Core",
            "GameplayAbilities",
            "GameplayTags",
            "GameplayTasks",
            "UMG",
        });

        PrivateDependencyModuleNames.AddRange(new []
        { 
            "CoreUObject", 
            "Engine",
        });
    }
}