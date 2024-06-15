# <img src="./program_info/org.unmojang.FjordLauncher.svg" alt="Fjord Launcher logo" width="96"/> Fjord Launcher Patched

Fjord Launcher Patched is a **fork** of [Fjord Launcher](https://github.com/unmojang/FjordLauncher) which is the **fork** of [Prism Launcher](https://github.com/PrismLauncher/PrismLauncher) which is the **fork** of [MultiMC](https://github.com/MultiMC/Launcher). 

[Fjord Launcher](https://github.com/unmojang/FjordLauncher) is based on [PollyMC](https://github.com/fn2006/PollyMC), which is now unmaintained. It is **not** endorsed by or affiliated with Prism Launcher or PollyMC or MultiMC.

# Advantages of this fork over Prism Launcher

- [Support for alternative auth servers](doc/alternative-auth-servers.md)

- Offline and authlib-injector accounts do not require an Microsoft account

- Ability to download FTB modpacks from within the launcher

# Having a problem with the launcher?

You **will not** open an issue in the Prism Launcher repo.

You **will not** ask about Fjord Launcher in the Prism Launcher discord.

You **will not** ask in #fjord-launcher in their [Matrix space](https://matrix.to/#/#unmojang:matrix.org).

#### You **will** open an issue [here](https://github.com/ZiQu-dev/FjordLauncherPatched/issues) or you could hit me up on discord if you know me.


# Installation

## Windows

Run exe with administrator and extraxt software whenever you want to. After that run **fjordlauncher.exe**.

You can also use the "*.7z release*" to unzip it without admin rights if you know how to use an archiver.

## Linux

Just run executable. Should be running without errors. However if you ran into problems, it should be easily fixable by installing Qt6Core5Compat library. _Well... It works on my machine._

## MacOS

I tried compiling it for Apple Silicon, but basiaclly my C++ compiler on my M1 Macbook said ___f*ck you___ so there's no MacOS version :c

However I wish you good luck and having fun trying to compile it yourself on this _sh*thole_ of the platform.

_Trust me I also thought it was 20 minut adventure which was in and out._

# Building

To build the launcher yourself, follow [the instructions on the Prism Launcher website](https://prismlauncher.org/wiki/development/build-instructions) but clone this repo instead.
