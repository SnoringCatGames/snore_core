# Snore Core

_This is still under development and not ready for use._

This is a GDExtension providing logic that is shared between Snoring Cat's other GDExtensions, such as [Surfacer](https://github.com/SnoringCatGames/surfacer) and [Scaffolder](https://github.com/SnoringCatGames/scaffolder).

## Building

This repo expects a **workspace-sibling layout** — see [bootstrapper](https://github.com/SnoringCatGames/bootstrapper) for the umbrella project and `scripts/bootstrap-workspace.ps1`, which clones all required sibling repos (godot-cpp, googletest, godot, the SnoringCat frameworks) into the workspace root in one go. Once siblings are in place, build with `scons sc_dev=yes sc_tests=yes` from this directory.
