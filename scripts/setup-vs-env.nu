# setup-vs-env.nu — Import Visual Studio MSVC environment into Nushell
#
# Usage:
#   source scripts/setup-vs-env.nu
#   setup-vs-env              # defaults to x64
#   setup-vs-env x86          # for 32-bit
#
# After running this, cl.exe, link.exe, cmake.exe etc. will be in PATH
# and INCLUDE/LIB environment variables will be set for MSVC builds.

export def setup-vs-env [
    arch: string = "x64"  # x64 | x86 | arm64
] {
    let vswhere = "C:\\Program Files (x86)\\Microsoft Visual Studio\\Installer\\vswhere.exe"

    if not ($vswhere | path exists) {
        print $"(ansi red)Error:(ansi reset) vswhere.exe not found at ($vswhere)"
        return
    }

    let vs_path = (^$vswhere -latest -products * -property installationPath | str trim)

    if ($vs_path | is-empty) {
        print $"(ansi red)Error:(ansi reset) Visual Studio installation not found"
        return
    }

    let vcvars = match $arch {
        "x64" => $"($vs_path)\\VC\\Auxiliary\\Build\\vcvars64.bat"
        "x86" => $"($vs_path)\\VC\\Auxiliary\\Build\\vcvars32.bat"
        "arm64" => $"($vs_path)\\VC\\Auxiliary\\Build\\vcvarsarm64.bat"
        _ => {
            print $"(ansi red)Error:(ansi reset) Unknown architecture: ($arch). Use x64, x86, or arm64."
            return
        }
    }

    if not ($vcvars | path exists) {
        print $"(ansi red)Error:(ansi reset) vcvars script not found: ($vcvars)"
        return
    }

    # Create a temporary batch file that runs vcvars then dumps the environment via SET
    let tmp = (mktemp --suffix .bat)
    $"@echo off\r\ncall \"($vcvars)\" > nul\r\nset" | save -f $tmp

    let output = (^cmd /c $tmp | lines)
    rm -f $tmp

    # Parse SET output (NAME=VALUE) and load into current environment
    for line in $output {
        let eq = ($line | str index-of '=')
        if $eq >= 0 {
            let name = ($line | str substring ..<$eq)
            let value = ($line | str substring ($eq + 1)..)

            if ($name in [
                "PATH" "INCLUDE" "LIB" "LIBPATH"
                "VCINSTALLDIR" "DevEnvDir"
                "WindowsSDKVersion" "WindowsSdkDir" "UCRTVersion"
                "FrameworkDir" "FrameworkVersion" "NETFXSDKDir"
                "ExtensionSdkDir"
                "VSCMD_ARG_TGT_ARCH" "VCToolsVersion"
            ]) {
                $env.($name) = $value
            }
        }
    }

    print $"(ansi green)VS dev environment loaded:(ansi reset) ($vs_path) [($arch)]"
}
