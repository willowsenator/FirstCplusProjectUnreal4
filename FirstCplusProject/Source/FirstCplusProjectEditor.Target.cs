// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class FirstCplusProjectEditorTarget : TargetRules
{
	public FirstCplusProjectEditorTarget( TargetInfo target) : base(target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		
		ExtraModuleNames.Add("FirstCplusProject");
	}
}
