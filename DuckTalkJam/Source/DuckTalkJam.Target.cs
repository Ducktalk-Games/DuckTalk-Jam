// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.Collections.Generic;

public class DuckTalkJamTarget : TargetRules
{
	public DuckTalkJamTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V7;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_8;

		ExtraModuleNames.AddRange( new string[] { "DuckTalkJam" } );

		bBuildDeveloperTools = false;

		OptimizeForSizeModules = new string[]
		{
			"DuckTalkJam"
		};
    }
}