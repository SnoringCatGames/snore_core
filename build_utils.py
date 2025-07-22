import glob
import os
import sys

from methods import print_error


def pre_setup(ARGUMENTS: dict, Environment: object, Variables: object, Help: object, SConscript: object) -> object:
    localEnv = Environment(tools=["default"], PLATFORM="")

    # Build profiles can be used to decrease compile times.
    # You can either specify "disabled_classes", OR
    # explicitly specify "enabled_classes" which disables all other classes.
    # Modify the example file as needed and uncomment the line below or
    # manually specify the build_profile parameter when running SCons.

    # localEnv["build_profile"] = "build_profile.json"

    customs = ['custom.py']
    customs = [os.path.abspath(path) for path in customs]

    opts = Variables(customs, ARGUMENTS)
    opts.Update(localEnv)

    Help(opts.GenerateHelpText(localEnv))

    env = localEnv.Clone()

    if not os.path.isdir('googletest'):
        print_error("googletest must be a submodule of the root repository.")
        sys.exit(1)
    if not os.path.isdir('godot-cpp'):
        print_error("godot-cpp must be a submodule of the root repository.")
        sys.exit(1)
    if not (os.path.isdir("godot-cpp") and os.listdir("godot-cpp")):
        print_error("""godot-cpp is not available within this folder, as Git submodules haven't been initialized.
    Run the following command to download godot-cpp:

        git submodule update --init --recursive""")
        sys.exit(1)

    env = SConscript("godot-cpp/SConstruct", {"env": env, "customs": customs})

    env['is_debug_build'] = ARGUMENTS.get("target", "") in ["editor", "template_debug"]
    env['is_continuous_integration'] = ARGUMENTS.get("ci", "") == "yes"
    env['includes_tests'] = ARGUMENTS.get("tests", "") == "yes"

    if env['is_continuous_integration']:
        env.Append(CPPDEFINES=["SC_CI_ENABLED"])

    if env['includes_tests']:
        env.Append(CPPDEFINES=["SC_TESTS_ENABLED"])

    # Enable C++23.
    # if env.get("is_msvc", False):
    #     env["CXXFLAGS"].remove("/std:c++17")
    #     env["CXXFLAGS"].insert(0, "/std:c++23preview")
    #     env["CXXFLAGS"].insert(0, "/Zc:preprocessor")
    # else:
    #     env["CXXFLAGS"].remove("-std=c++17")
    #     env["CXXFLAGS"].insert(0, "-std=c++23")

    return env


def post_setup(env: object, cpp_paths: list[str], sources: list[str], lib_name: str, addon_dir_name: str, Default: object) -> None:
    if env['is_debug_build']:
        try:
            doc_data = env.GodotCPPDocData("src/gen/doc_data.gen.cpp", source=glob.glob("doc_classes/*.xml"))
            sources.append(doc_data)
        except AttributeError:
            print("Not including class reference as we're targeting a pre-4.3 baseline.")
    
    # Filter-out generated files.
    sources_copy = sources.copy()
    sources = [f for f in sources_copy if not str(f).endswith(".gen.cpp")]

    env.Append(CPPPATH=cpp_paths)

    # .dev doesn't inhibit compatibility, so we don't need to key it.
    # .universal just means "compatible with all relevant arches" so we don't need to key it.
    suffix = env['suffix'].replace(".dev", "").replace(".universal", "")

    lib_filename = "{}{}{}{}".format(env.subst('$SHLIBPREFIX'), lib_name, suffix, env.subst('$SHLIBSUFFIX'))

    library = env.SharedLibrary(
        "bin/{}/{}".format(env['platform'], lib_filename),
        source=sources,
    )

    copy = env.Install("demo/addons/{}/bin/{}/".format(addon_dir_name, env["platform"]), library)

    default_args = [library, copy]
    Default(*default_args)


def set_up(env: object, cpp_paths: list[str], sources: list[str], addon_dir_name: str, is_setup_for_self = False) -> None:
    if is_setup_for_self:
        cpp_paths.extend([
            "src/",
        ])
        sources.extend(
            glob.glob("src/**/*.cpp", recursive=True)
        )
    else:
        cpp_paths.extend([
            addon_dir_name + "/src/",
        ])
        sources.extend(
            glob.glob(addon_dir_name + "/src/snore_core/**/*.cpp", recursive=True)
        )

    if env['includes_tests']:
        cpp_paths.extend([
            "googletest/googletest/",
            "googletest/googletest/include/",
            "googletest/googlemock/",
            "googletest/googlemock/include/",
        ])

        googletest_sources = ([
            "googletest/googletest/src/gtest-all.cc",
            "googletest/googlemock/src/gmock-all.cc",
        ]
            # glob.glob("googletest/googletest/src/*.cc") +
            # glob.glob("googletest/googlemock/src/*.cc")
        )
        googletest_exclusions = [
            # "googletest/googletest/src/gtest-all.cc",
            # "googletest/googletest/src/gtest_main.cc",
            # "googletest/googlemock/src/gmock-all.cc",
            # "googletest/googlemock/src/gmock_main.cc",
        ]
        sources.extend([x for x in googletest_sources if str(x) not in googletest_exclusions])
