# <img src="./program_info/org.unmojang.FjordLauncher.svg" alt="Fjord Launcher logo" width="96"/> Fjord Launcher

Fjord Launcher is a **fork** of [Prism Launcher](https://github.com/PrismLauncher/PrismLauncher). It was based on [PollyMC](https://github.com/fn2006/PollyMC), which is now unmaintained. It is **not** endorsed by or affiliated with Prism Launcher or PollyMC.

## Advantages of this fork over Prism Launcher

- [Support for alternative auth servers](doc/alternative-auth-servers.md)

- Ability to download FTB modpacks from within the launcher

## Having a problem with the launcher?

You **will not** open an issue in the Prism Launcher repo.

You **will not** ask about Fjord Launcher in the Prism Launcher discord.

You **will** ask in #fjord-launcher in our [Matrix space](https://matrix.to/#/#unmojang:matrix.org).

You **will** open an issue [here](https://github.com/unmojang/FjordLauncher/issues).

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
flatpak install org.unmojang.FjordLauncher
```

### Arch Linux

Fjord Launcher is [available](https://aur.archlinux.org/packages?O=0&K=fjordlauncher) from the AUR:

```Shell
paru -S fjordlauncher-bin # binary package, recommended
paru -S fjordlauncher-git # or, build from source from latest commit
```

### Debian/Ubuntu

Install from the MPR with [Mist](https://docs.makedeb.org/using-the-mpr/mist-the-mpr-cli/#installing-mist):

```Shell
mist install fjordlauncher-bin # Binary package, recommended
mist install fjordlauncher-git # or, build from source from latest commit
```

### Nix

This repository contains a Nix flake:

```Shell
nix run github:unmojang/FjordLauncher
```

See [nix/README.md](nix/README.md) for details.

### Other Linux

AppImages are available in the [releases section](https://github.com/unmojang/FjordLauncher/releases/latest).

## Building

To build the launcher yourself, follow [the instructions on the Prism Launcher website](https://prismlauncher.org/wiki/development/build-instructions) but clone this repo instead.
