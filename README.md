<p align="center"><img src="docs/assets/logo.svg" width="96" alt="Nitruran logo"></p>

# Nitruran

A Minecraft launcher with a modern, Modrinth-App-inspired interface, running on the
[Prism Launcher](https://github.com/PrismLauncher/PrismLauncher) core.

> Early preview — Windows first.

* Interface: left sidebar, instance cards, Home / Library (more pages in progress).
* Core: Prism Launcher's instance, Java, mod-loader, account and launch logic, unchanged.
* Open source under GPL-3.0.

## Screenshots

> Design previews rendered with the real Nitruran style and card-painting code, using **sample instances**.
> Real in-app captures will replace them once the first CI build is running.

| Home | Library |
|---|---|
| ![Home](docs/assets/screenshot-home.png) | ![Library](docs/assets/screenshot-library.png) |
| **Instance menu** | **Compact window** |
| ![Instance menu](docs/assets/screenshot-menu.png) | ![Compact window](docs/assets/screenshot-compact.png) |

## Build
GitHub Actions builds Windows (MSVC, Qt 6.11.2) — see `.github/workflows/windows.yml`.
Add the repository secret `MSA_CLIENT_ID` (your approved Microsoft/Mojang application ID) first.

## Credits and licence
Nitruran is a fork of Prism Launcher (GPL-3.0, see `COPYING.md` and `LICENSE`), whose copyright
notices are kept. The interface layout and colours follow Modrinth's open-source app design tokens;
Modrinth's name and logo are **not** used. Nitruran is not affiliated with or endorsed by Mojang,
Microsoft, Prism Launcher, Modrinth or CurseForge.
