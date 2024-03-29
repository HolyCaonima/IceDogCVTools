// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System.IO;

public class IceDogCV : ModuleRules
{
	private string ModulePath
	{
		get { return ModuleDirectory; }
	}
	private string ThirdPartyPath
	{
		get { return Path.GetFullPath(Path.Combine(ModulePath, "../../ThirdParty/")); }
	}

	public bool LoadCVLib(TargetInfo Target)
	{
		bool isLibrarySupported = false;

		// Create CVLib Path
		string IceDogCVLibPath = Path.Combine(ThirdPartyPath, "IceDogCVLib");

		// Gete Library Path
		string LibPath = "";
		bool isdebug = Target.Configuration == UnrealTargetConfiguration.Debug && BuildConfiguration.bDebugBuildsActuallyUseDebugCRT;
		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			LibPath = Path.Combine(IceDogCVLibPath, "Libraries", "Win64");
			isLibrarySupported = true;
		}
		else if (Target.Platform == UnrealTargetPlatform.Win32)
		{
			LibPath = Path.Combine(IceDogCVLibPath, "Libraries", "Win32");
			isLibrarySupported = true;
		}
		else
		{
			string Err = string.Format("{0} dedicated server is made to depend on {1}. We want to avoid this, please correct module dependencies.", Target.Platform.ToString(), this.ToString()); System.Console.WriteLine(Err);
		}

		if (isLibrarySupported)
		{
			//Add Include path 
			PublicIncludePaths.AddRange(new string[] { Path.Combine(IceDogCVLibPath, "Includes") });

			// Add Library Path 
			PublicLibraryPaths.Add(LibPath);

			// Add Dependencies 
			if (!isdebug)
			{
				//Add Static Libraries
				PublicAdditionalLibraries.Add("DLL_EXP_r.lib");

				//Add Dynamic Libraries
				PublicDelayLoadDLLs.Add("DLL_EXP_r.dll");
			}
			else
			{
				//Add Static Libraries (Debug Version)
				PublicAdditionalLibraries.Add("DLL_EXP.lib");

				//Add Dynamic Libraries (Debug Version)
				PublicDelayLoadDLLs.Add("DLL_EXP.dll");
			}
		}

		Definitions.Add(string.Format("WITH_ICEDOGCVLIB_BINDING={0}", isLibrarySupported ? 1 : 0));

		return isLibrarySupported;
	}

	public bool LoadOpenCV(TargetInfo Target)
	{
		bool isLibrarySupported = false;

		// Create OpenCV Path 
		string OpenCVPath = Path.Combine(ThirdPartyPath, "OpenCV");

		// Get Library Path 
		string LibPath = "";
		bool isdebug = Target.Configuration == UnrealTargetConfiguration.Debug && BuildConfiguration.bDebugBuildsActuallyUseDebugCRT;
		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			LibPath = Path.Combine(OpenCVPath, "Libraries", "Win64");
			isLibrarySupported = true;
		}
		else if (Target.Platform == UnrealTargetPlatform.Win32)
		{
			LibPath = Path.Combine(OpenCVPath, "Libraries", "Win32");
			isLibrarySupported = true;
		}
		else
		{
			string Err = string.Format("{0} dedicated server is made to depend on {1}. We want to avoid this, please correct module dependencies.", Target.Platform.ToString(), this.ToString()); System.Console.WriteLine(Err);
		}

		if (isLibrarySupported)
		{
			//Add Include path 
			PublicIncludePaths.AddRange(new string[] { Path.Combine(OpenCVPath, "Includes") });

			// Add Library Path 
			PublicLibraryPaths.Add(LibPath);

			// Add Dependencies 
			if (!isdebug)
			{
				//Add Static Libraries
				PublicAdditionalLibraries.Add("opencv_ts300.lib");
				PublicAdditionalLibraries.Add("opencv_world300.lib");

				//Add Dynamic Libraries
				PublicDelayLoadDLLs.Add("opencv_world300.dll");
			}
			else
			{
				//Add Static Libraries (Debug Version)
				PublicAdditionalLibraries.Add("opencv_ts300d.lib");
				PublicAdditionalLibraries.Add("opencv_world300d.lib");

				//Add Dynamic Libraries (Debug Version)
				PublicDelayLoadDLLs.Add("opencv_world300d.dll");
			}
		}

		Definitions.Add(string.Format("WITH_OPENCV_BINDING={0}", isLibrarySupported ? 1 : 0));

		return isLibrarySupported;
	}
	public IceDogCV(TargetInfo Target)
	{
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "RHI", "RenderCore", "ShaderCore","UMG", "Slate", "SlateCore" });

		PrivateDependencyModuleNames.AddRange(new string[] { });

		LoadOpenCV(Target);

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");
		// if ((Target.Platform == UnrealTargetPlatform.Win32) || (Target.Platform == UnrealTargetPlatform.Win64))
		// {
		//		if (UEBuildConfiguration.bCompileSteamOSS == true)
		//		{
		//			DynamicallyLoadedModuleNames.Add("OnlineSubsystemSteam");
		//		}
		// }
	}
}
