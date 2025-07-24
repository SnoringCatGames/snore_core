import glob
import os
import sys
from enum import Enum


default_lib_name = "SnoreCore"
default_addon_dir_name = "snore_core"


# Colors are disabled in non-TTY environments such as pipes. This means
# that if output is redirected to a file, it won't contain color codes.
# Colors are always enabled on continuous integration.
_colorize = bool(sys.stdout.isatty() or os.environ.get("CI"))


class ANSI(Enum):
    """
    Enum class for adding ansi colorcodes directly into strings.
    Automatically converts values to strings representing their
    internal value, or an empty string in a non-colorized scope.
    """

    RESET = "\x1b[0m"

    BOLD = "\x1b[1m"
    ITALIC = "\x1b[3m"
    UNDERLINE = "\x1b[4m"
    STRIKETHROUGH = "\x1b[9m"
    REGULAR = "\x1b[22;23;24;29m"

    BLACK = "\x1b[30m"
    RED = "\x1b[31m"
    GREEN = "\x1b[32m"
    YELLOW = "\x1b[33m"
    BLUE = "\x1b[34m"
    MAGENTA = "\x1b[35m"
    CYAN = "\x1b[36m"
    WHITE = "\x1b[37m"

    PURPLE = "\x1b[38;5;93m"
    PINK = "\x1b[38;5;206m"
    ORANGE = "\x1b[38;5;214m"
    GRAY = "\x1b[38;5;244m"

    def __str__(self) -> str:
        global _colorize
        return str(self.value) if _colorize else ""


def print_warning(*values: object) -> None:
    """Prints a warning message with formatting."""
    print(
        f"{ANSI.YELLOW}{ANSI.BOLD}WARNING:{ANSI.REGULAR}",
        *values,
        ANSI.RESET,
        file=sys.stderr,
    )


def print_error(*values: object) -> None:
    """Prints an error message with formatting."""
    print(
        f"{ANSI.RED}{ANSI.BOLD}ERROR:{ANSI.REGULAR}",
        *values,
        ANSI.RESET,
        file=sys.stderr,
    )


def pre_setup(
    ARGUMENTS: dict,
    Environment: object,
    Variables: object,
    Help: object,
    SConscript: object,
) -> object:
    localEnv = Environment(tools=["default"], PLATFORM="")

    # Build profiles can be used to decrease compile times.
    # You can either specify "disabled_classes", OR
    # explicitly specify "enabled_classes" which disables all other classes.
    # Modify the example file as needed and uncomment the line below or
    # manually specify the build_profile parameter when running SCons.

    # localEnv["build_profile"] = "build_profile.json"

    customs = ["custom.py"]
    customs = [os.path.abspath(path) for path in customs]

    opts = Variables(customs, ARGUMENTS)
    opts.Update(localEnv)

    Help(opts.GenerateHelpText(localEnv))

    env = localEnv.Clone()

    if not os.path.isdir("googletest"):
        print_error("googletest must be a submodule of the root repository.")
        sys.exit(1)
    if not os.path.isdir("godot-cpp"):
        print_error("godot-cpp must be a submodule of the root repository.")
        sys.exit(1)
    if not (os.path.isdir("godot-cpp") and os.listdir("godot-cpp")):
        print_error(
            """godot-cpp is not available within this folder, as Git submodules haven't been initialized.
    Run the following command to download godot-cpp:

        git submodule update --init --recursive"""
        )
        sys.exit(1)

    env = SConscript("godot-cpp/SConstruct", {"env": env, "customs": customs})

    env["is_debug_build"] = ARGUMENTS.get("target", "") in ["editor", "template_debug"]
    env["is_continuous_integration"] = ARGUMENTS.get("ci", "") == "yes"
    env["includes_tests"] = ARGUMENTS.get("tests", "") == "yes"

    if env["is_continuous_integration"]:
        env.Append(CPPDEFINES=["SC_CI_ENABLED"])

    if env["includes_tests"]:
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


def post_setup(
    env: object,
    cpp_paths: list[str],
    sources: list[str],
    libs: list[str],
    lib_paths: list[str],
    lib_name: str,
    addon_dir_name: str,
    Default: object,
) -> None:
    if env["is_debug_build"]:
        try:
            doc_data = env.GodotCPPDocData(
                "src/gen/doc_data.gen.cpp", source=glob.glob("doc_classes/*.xml")
            )
            sources.append(doc_data)
        except AttributeError:
            print(
                "Not including class reference as we're targeting a pre-4.3 baseline."
            )

    # Filter-out generated files.
    sources_copy = sources.copy()
    sources = [f for f in sources_copy if not str(f).endswith(".gen.cpp")]

    env.Append(CPPPATH=cpp_paths)
    env.Append(LIBS=libs)
    env.Append(LIBPATH=lib_paths)

    lib_filename = create_lib_filename(env, lib_name, True)
    library = env.SharedLibrary(
        "bin/{}/{}".format(env["platform"], lib_filename),
        source=sources,
    )

    addon_platform_dir_name = create_addon_platform_dir_name(env, addon_dir_name)
    copy = env.Install(addon_platform_dir_name, library)

    default_args = [library, copy]
    Default(*default_args)


def set_up(
    env: object,
    cpp_paths: list[str],
    sources: list[str],
    libs: list[str],
    lib_paths: list[str],
    snore_core_addon_dir_name: str,
    is_setup_for_self=False,
) -> None:
    if is_setup_for_self:
        cpp_paths.extend(["src/"])
        sources.extend(glob.glob("src/**/*.cpp", recursive=True))
    else:
        cpp_paths.extend([snore_core_addon_dir_name + "/src/"])
        # Use a DLL rather than statically including .cpp files.
        # sources.extend([])
        lib_filename = create_lib_filename(env, default_lib_name, False)
        libs.extend([lib_filename])
        addon_platform_dir_name = create_addon_platform_dir_name(
            env, snore_core_addon_dir_name
        )
        lib_paths.extend([addon_platform_dir_name])
        # FIXME: ------------------------------
        print_warning(">>>>>>>>>>>>>>>>>>>>>")
        print_warning(">> " + addon_platform_dir_name)
        print_warning(">> " + lib_filename)

    if env["includes_tests"]:
        cpp_paths.extend(
            [
                "googletest/googletest/",
                "googletest/googletest/include/",
                "googletest/googlemock/",
                "googletest/googlemock/include/",
            ]
        )

        googletest_sources = (
            [
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
        sources.extend(
            [x for x in googletest_sources if str(x) not in googletest_exclusions]
        )


def create_symlink(includes_shared_library=False) -> None:
    """
    Make the SnoreCore GDScript addon files (and maybe the GDExtension shared library) accessible from the root module's demo.
    """
    original_path = os.path.abspath("snore_core/demo/addons/snore_core")
    link_path = os.path.abspath("demo/addons/snore_core")
    if not os.path.lexists(link_path):
        os.symlink(original_path, link_path, target_is_directory=True)


def create_lib_filename(
    env: object, lib_name: str, includes_shared_lib_suffix=True
) -> str:
    # .dev doesn't inhibit compatibility, so we don't need to key it.
    # .universal just means "compatible with all relevant arches" so we don't need to key it.
    suffix = env["suffix"].replace(".dev", "").replace(".universal", "")
    shared_lib_suffix = includes_shared_lib_suffix and env.subst("$SHLIBSUFFIX") or ""
    return "{}{}{}{}".format(
        env.subst("$SHLIBPREFIX"), lib_name, suffix, shared_lib_suffix
    )


def create_addon_platform_dir_name(env: object, addon_dir_name: str) -> str:
    return "demo/addons/{}/bin/{}/".format(addon_dir_name, env["platform"])
