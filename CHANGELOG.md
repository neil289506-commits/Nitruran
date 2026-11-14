# Changelog

All notable changes to Nitruran are listed here. Nitruran is a fork of
[Prism Launcher](https://github.com/PrismLauncher/PrismLauncher) (GPL-3.0); entries below describe what differs
from upstream. Current base: upstream `develop` at `1a6438dcf` (12.0.0-pre1 plus later upstream commits).

## [Unreleased]

### Added
- The in-app **updater now follows this repository** (`Launcher_UPDATER_GITHUB_REPO`) instead of Prism Launcher's.
  Release assets must contain `Windows-MSVC` in their name for the updater to accept them, so they are now
  named `Nitruran-Windows-MSVC-Setup.exe` and `Nitruran-Windows-MSVC-Portable.zip`. The app version is
  12.0.1, so the next release must be tagged higher than the 12.0.0 reported by v11.1.1 builds (for example
  `v12.0.1`). Read from the updater's source; not yet exercised end to end.
- **AES-encrypted default CurseForge key**: the source can carry the key as AES-128-CTR text
  (`Launcher_CURSEFORGE_API_KEY_ENCRYPTED` / `_DECRYPT_KEY`, produced by `tools/encrypt_secret.py`) so that
  automated scrapers do not find a plain key. This is bot-proofing only — the decryption key ships with the
  source and binary. A key passed at build time (the `CURSEFORGE_API_KEY` secret) overrides it, and users can
  override both under Settings → API.
- Built-in **Nitruran theme** (Modrinth-app-inspired dark palette) that styles every dialog — settings, new
  instance, instance window — not just the main window. It is the default for new installs; existing installs keep
  their current theme until changed under Settings → Appearance.

### Changed
- First-run / sidebar sync: **accounts are no longer ticked by default**, because they are tied to the app that
  signed them in and normally need a fresh sign-in.
- The `CURSEFORGE_API_KEY` secret is now optional in CI (it overrides the embedded default when set).
- Number fields no longer show stray spin arrows under the new theme (use keyboard or mouse wheel).

### Fixed
- The release job of the Windows workflow can be re-run when a release already exists: it now uploads and
  overwrites the assets instead of failing at "Create release".

## [v11.1.1] - 2026-11-13

### Added
- **New interface**: left sidebar (Home, Discover, Library, Sync, Accounts, Settings), a Home page with
  "Jump back in", and a Library page with instance cards (one-click play / stop, context menu with
  Edit instance). The main window is replaced; Prism's core logic (launching, instances, Java, mod loaders,
  accounts) is used as is.
- **Blocked CurseForge files → identical copy on Modrinth.** When a CurseForge author disables third-party
  downloads, Nitruran looks for the byte-identical file (same SHA-1) on Modrinth and downloads that copy.
  Prism already did this for modpack installs; Nitruran adds it to the **mod browser** (blocked versions used to
  be hidden) and to **update checks** (used to be a manual-download list). Only Modrinth's public API and CDN
  are used, and only exact matches are accepted. Controlled by the existing "fall back to Modrinth for blocked
  mods" setting.
- **First-run sync** from Prism Launcher and mlcmLauncher: offers to copy instances (with groups), accounts,
  icons and a few settings (memory, Java path, JVM arguments, language, console options). Nothing that already
  exists is overwritten. Also available any time from the sidebar.
- Interface text in **English, 繁體中文 and 简体中文** for Nitruran's own strings.
- **Windows installer and portable ZIP** built by GitHub Actions (MSVC, Qt 6.11.2).
- Project website (GitHub Pages) and unit tests for the sync and Modrinth-mirror logic.

### Changed
- Branding: name **Nitruran**, new icon, binary `nitruran.exe`, data folder `%APPDATA%\Nitruran` (no longer
  shared with Prism Launcher).
- The CurseForge API key is no longer defaulted to Prism's key; it is supplied at build time through the
  `CURSEFORGE_API_KEY` secret. The Microsoft client ID is supplied through the `MSA_CLIENT_ID` secret.
- Upstream-only GitHub workflows were removed; Nitruran has its own Windows workflow.

### Fixed
- Windows resource files (icon / version info) for the renamed binary in CI.

## Known issues

- Discover (new-instance dialog), settings and the instance window are still Prism's pages, only restyled.
- Screenshots in the README and on the website are design previews rendered with sample data.
- Accounts copied from another launcher were signed in with that launcher's client ID and may need signing
  in again.
- Windows only. Linux and macOS builds are not provided.

## Build from source

These steps mirror `.github/workflows/windows.yml`, which is the path that is actually exercised in CI. The local
command lines below are adapted from it and have not been run on a separate machine.

### Requirements
- Windows 10/11 x64
- Visual Studio 2022 with "Desktop development with C++" (MSVC), run from a *Developer PowerShell for VS 2022*
- CMake 3.25 or newer and Ninja
- Qt **6.11.2** (MSVC 2022 64-bit) with the *Qt Image Formats* and *Qt Network Authorization* modules
  (upstream requires Qt 6.8 or newer; CI uses 6.11.2)
- JDK 17 (needed to build the bundled Java components)
- Git (the repository uses submodules, including vcpkg)
- Optional, for the installer: NSIS (`makensis`)

### Defaults and overrides
- `Launcher_MSA_CLIENT_ID` — defaults to Nitruran's approved client ID. If you fork Nitruran, pass your own
  Mojang-approved application ID; do not reuse this one for a different app.
- `Launcher_CURSEFORGE_API_KEY` — optional: your own CurseForge API key. Without it the AES-encrypted default
  in `CMakeLists.txt` is used (if one is set); users can also enter a key under Settings → API.

### Steps
```powershell
git clone --recursive https://github.com/neil289506-commits/Nitruran.git
cd Nitruran

# Resource files for the renamed binary (CI does the same)
foreach ($s in "rc.in","qrc.in","manifest.in","ico") {
  Copy-Item "program_info\prismlauncher.$s" "program_info\nitruran.$s"
}

cmake --preset default `
  -D CMAKE_PREFIX_PATH="C:\Qt\6.11.2\msvc2022_64" `
  -D VCPKG_HOST_TRIPLET=x64-windows -D VCPKG_TARGET_TRIPLET=x64-windows `
  -D Launcher_MSA_CLIENT_ID="<your approved client id>" `        # optional: overrides the default
  -D Launcher_CURSEFORGE_API_KEY="<your CurseForge key>" `       # optional: overrides the default
  -D Launcher_APP_BINARY_NAME=nitruran

cmake --build --preset default --config Release
ctest --preset default --build-config Release
cmake --install build --config Release      # result is in .\install
```

Notes:
- The first configure builds dependencies through vcpkg and can take a while.
- Use the `default` preset locally. The `ci` preset sets an artifact name (which switches the updater on) and
  treats warnings as errors; CI overrides that with `-D Launcher_WARNING_AS_ERROR=OFF`.
- Portable build: `cmake --install build --config Release --prefix install-portable --component portable`
  after copying `install` to `install-portable`.
- GitHub Actions: run the "Windows build" workflow. Repository secrets `MSA_CLIENT_ID` and `CURSEFORGE_API_KEY`
  are optional overrides. Pushing a tag that starts with `v` also publishes a release.

[Unreleased]: https://github.com/neil289506-commits/Nitruran/compare/v11.1.1...HEAD
[v11.1.1]: https://github.com/neil289506-commits/Nitruran/releases/tag/v11.1.1
