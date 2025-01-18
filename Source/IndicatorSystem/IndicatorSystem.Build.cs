using UnrealBuildTool;

public class IndicatorSystem : ModuleRules
{
    public IndicatorSystem(ReadOnlyTargetRules Target) : base(Target)
    {
        PublicDependencyModuleNames.AddRange(new []
        {
            "Core",
            "UMG",
            "AsyncMixin", 
            "ModularGameplay",
        });

        PrivateDependencyModuleNames.AddRange(new []
        { 
            "CoreUObject", 
            "Engine", 
            "Slate", 
            "SlateCore"
        });
    }
}