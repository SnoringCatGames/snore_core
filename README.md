# Snore Core

_This is still under development and not ready for use._

This is a GDExtension providing logic that is shared between Snoring Cat's other GDExtensions, such as [Surfacer](https://github.com/SnoringCatGames/surfacer) and [Scaffolder](https://github.com/SnoringCatGames/scaffolder).

## Building

This repo expects a **workspace-sibling layout** — see [bootstrapper](https://github.com/SnoringCatGames/bootstrapper) for the umbrella project and `scripts/bootstrap-workspace.ps1`, which clones all required sibling repos (godot-cpp, googletest, godot, the SnoringCat frameworks) into the workspace root in one go. Once siblings are in place, build with `scons sc_dev=yes sc_tests=yes` from this directory.

## Loading from a Godot project

Don't load `snore_core` as a standalone GDExtension. Godot 4 does not yet support one GDExtension depending on another (see [godot-rust/gdext#615](https://github.com/godot-rust/gdext/issues/615) and [godotengine/godot-proposals#13997](https://github.com/godotengine/godot-proposals/issues/13997)), so SnoreCore's symbols ship inside the [surf_scaf](https://github.com/SnoringCatGames/surf_scaf) bundle. Add `surf_scaf.gdextension` to your project's `addons/`; that pulls SnoreCore in transitively. Loading both `surf_scaf.gdextension` and a hypothetical `snore_core.gdextension` would double-register SnoreCore's classes.
