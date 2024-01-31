newoption {
  trigger = "game",
  value = "CS2",
  description = "Choose a particular game for dumping source 2 sdk",
  allowed = {
     { "CS2", "Counter-Strike 2" },
     { "DOTA2","Dota 2" },
     { "SBOX","S&BOX" },
     { "ARTIFACT2", "Artifact Foundry" },
     { "ARTIFACT1", "Artifact Classic" },
     { "UNDERLORDS", "Dota Underlords" },
     { "DESKJOB", "Aperture Desk Job" },
     { "HL_ALYX", "Half-Life: Alyx" },
     { "THE_LAB_ROBOT_REPAIR", "Portal: Aperture Robot Repair" }
  },
  default = "CS2"
}

workspace "source2gen"
  architecture "x64"
  startproject "source2gen"

  configurations
  {
    "Debug",
    "Release",
    "Dist"
  }

  outputdir = "%{cfg.buildcfg}"

  IncludeDir = {}

  CppVersion = "C++latest"
  MsvcToolset = "v143"
  WindowsSdkVersion = "10.0"
  
  function DeclareMSVCOptions()
    filter "system:windows"
    staticruntime "Off"
    systemversion (WindowsSdkVersion)
    toolset (MsvcToolset)
    cppdialect (CppVersion)

    defines
    {
      "_CRT_SECURE_NO_WARNINGS",
      "NOMINMAX",
      "WIN32_LEAN_AND_MEAN",
      "_WIN32_WINNT=0x601" -- Support Windows 7
    }
    
    disablewarnings
    {
        "4100", -- C4100: unreferenced formal parameter
        "4201", -- C4201: nameless struct/union
        "4307", -- C4307: integral constant overflow
        "4311", -- C4311: 'variable' : pointer truncation from 'type' to 'type'
        "4302", -- C4302: 'conversion' : truncation from 'type 1' to 'type 2'
        "4267", -- C4267: 'var' : conversion from 'size_t' to 'type', possible loss of data
        "4244"  -- C4244: 'conversion' conversion from 'type1' to 'type2', possible loss of data
    }
  end
   
  function DeclareDebugOptions()
    filter "configurations:Debug"
      defines { "_DEBUG" }
      symbols "On"
	  flags { "MultiProcessorCompile" }
    filter "not configurations:Debug"
      defines { "NDEBUG" }
  end

  project "source2gen"
    location "source2gen"
    kind "SharedLib"
    language "C++"
	  buildoptions { "/bigobj" } 

    targetdir ("bin/" .. outputdir)
    objdir ("bin/int/" .. outputdir .. "/%{prj.name}")

    defines
    {
    }
	
    files
    {
      ".clang-format",
      "include/**.h",
      "include/**.hpp",
      "src/**.cpp",
    }

    includedirs
    {
      "include",
      "src"
    }

    libdirs
    {
      "bin/lib"
    }

    DeclareMSVCOptions()
    DeclareDebugOptions()

    flags { "NoImportLib", "Maps", "MultiProcessorCompile" }

    filter "configurations:Debug"
      defines { "source2gen_DEBUG" }

    filter "configurations:Release"
      defines { "source2ge_RELEASE" }
      optimize "speed"
	  
    filter "configurations:Dist"
      flags { "LinkTimeOptimization", "FatalCompileWarnings" }
      defines { "source2ge_DIST" }
      optimize "speed"

    filter { "options:game=CS2" }
      defines { "CS2" }

    filter { "options:game=DOTA2" }
      defines { "DOTA2" }

    filter { "options:game=SBOX" }
      defines { "SBOX" }

    filter { "options:game=ARTIFACT2" }
      defines { "ARTIFACT2" }

    filter { "options:game=ARTIFACT1" }
      defines { "ARTIFACT1" }

    filter { "options:game=UNDERLORDS" }
      defines { "UNDERLORDS" }

    filter { "options:game=DESKJOB" }
      defines { "DESKJOB" }

    filter { "options:game=HL_ALYX" }
      defines { "HL_ALYX" }

    filter { "options:game=THE_LAB_ROBOT_REPAIR" }
      defines { "THE_LAB_ROBOT_REPAIR" }