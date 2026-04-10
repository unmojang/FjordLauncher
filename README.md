# <img src="./program_info/org.unmojang.FjordLauncher.svg" alt="Fjord Launcher logo" width="96"/> Fjord Launcher

Fjord Launcher is a soft **fork** of [Prism Launcher](https://github.com/PrismLauncher/PrismLauncher) with the goal of adding support for alternative authentication servers such as [Drasl](https://github.com/unmojang/drasl), [Ely.by](https://ely.by/), and [Blessing Skin](https://github.com/bs-community/blessing-skin-server). An early version of Fjord Launcher's patchset was proposed to Prism Launcher in [PrismLauncher/PrismLauncher#543](https://github.com/PrismLauncher/PrismLauncher/pull/543). It received positive feedback, but the maintainers did not reach agreement about whether it should be merged.

I plan to someday try again to upstream these changes, since I believe most Prism Launcher users and developers agree that alternative authentication servers should be supported, with appropriate safeguards against piracy.

**Akin to Prism Launcher, Fjord Launcher requires adding a Microsoft account before adding an offline or authlib-injector account.**

See [doc/alternative-auth-servers.md](doc/alternative-auth-servers.md) for a non-exhaustive list of alternative authentication servers.

## Having a problem with the launcher?

**Do not** open an issue in the Prism Launcher repo, and **do not** ask about Fjord Launcher in the Prism Launcher Discord server. Instead, ask in #fjord-launcher in our [Matrix space](https://matrix.to/#/#unmojang:matrix.org), or [open an issue](https://github.com/unmojang/FjordLauncher/issues), in this repository.

## Installation

### Windows

#### [Scoop](https://scoop.sh) (recommended)

```PowerShell
scoop bucket add unmojang https://github.com/unmojang/scoop-unmojang
scoop install unmojang/fjordlauncher
```

#### Windows (Manual)

You can get installers or portable builds from the [releases section](https://github.com/unmojang/FjordLauncher/releases/latest), MSVC builds are recommended over MinGW builds, but there's no real difference.

### macOS

#### [Homebrew](https://brew.sh) (recommended)

```Shell
brew tap unmojang/homebrew-unmojang
brew install --cask fjordlauncher
```

#### macOS (Manual)

There are builds for macOS in the [releases section](https://github.com/unmojang/FjordLauncher/releases/latest).

### Flatpak

```Shell
flatpak remote-add --user --if-not-exists unmojang https://unmojang.github.io/unmojang-flatpak/index.flatpakrepo
flatpak install org.kde.Platform/x86_64/6.10
flatpak install org.unmojang.FjordLauncher
```

### Arch Linux

Fjord Launcher is [available](https://aur.archlinux.org/packages?O=0&K=fjordlauncher) from the AUR:

```Shell
paru -S fjordlauncher
paru -S fjordlauncher-git # build latest Git commit from source
```

`fjordlauncher` is available in [Chaotic-AUR](https://aur.chaotic.cx/).

The `fjordlauncher-bin` AUR package is broken; see [https://github.com/unmojang/FjordLauncher/issues/20](https://github.com/unmojang/FjordLauncher/issues/20).

### Nix

This repository contains a Nix flake:

```Shell
nix run github:unmojang/FjordLauncher
```

See [nix/README.md](nix/README.md) for details.

### Gentoo

Install from the [GURU repository](https://wiki.gentoo.org/wiki/Project:GURU/Information_for_End_Users):

```Shell
echo 'games-action/fjordlauncher' >> /etc/portage/package.accept_keywords
emerge -av games-action/fjordlauncher
```

### Other Linux

AppImages are available in the [releases section](https://github.com/unmojang/FjordLauncher/releases/latest).

## Building

To build the launcher yourself, follow the [instructions on the Prism Launcher website](https://prismlauncher.org/wiki/development/build-instructions), but clone this repo instead.

## Notes

- You can easily use a custom version of authlib-injector on an instance. Select the instance in the main window, click "Edit" (or Ctrl+I/Command+I), go to the Version tab, click "Add Agents", and select your authlib-injector JAR. If your JAR is not correctly identified as authlib-injector, make sure the `Agent-Class` field in the JAR's MANIFEST.MF is `moe.yushi.authlibinjector.Premain`.
