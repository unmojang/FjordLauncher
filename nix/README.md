# Fjord Launcher Nix Packaging

## Installing a development release (flake)

We use [garnix](https://garnix.io/) to build and cache our development builds.
If you want to avoid rebuilds you may add the garnix cache to your substitutors, or use `--accept-flake-config`
to temporarily enable it when using `nix` commands.

Example (NixOS):

```nix
{...}:
{
  nix.settings = {
    trusted-substituters = [
      "https://cache.garnix.io"
    ];

    trusted-public-keys = [
      "cache.garnix.io:CTFPyKSLcx5RMJKfLo5EEPUObbA78b0YQ2DTCJXqr9g="
    ];
  };
}
```

### Using the overlay

After adding `github:unmojang/FjordLauncher` to your flake inputs, you can add the `default` overlay to your nixpkgs instance.

Example:

```nix
{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    fjordlauncher = {
      url = "github:unmojang/FjordLauncher";
      # Optional: Override the nixpkgs input of fjordlauncher to use the same revision as the rest of your flake
      # Note that overriding any input of fjordlauncher may break reproducibility
      # inputs.nixpkgs.follows = "nixpkgs";
    };
  };

  outputs = {nixpkgs, fjordlauncher}: {
    nixosConfigurations.foo = nixpkgs.lib.nixosSystem {
      system = "x86_64-linux";

      modules = [
        ({pkgs, ...}: {
          nixpkgs.overlays = [fjordlauncher.overlays.default];

          environment.systemPackages = [pkgs.fjordlauncher];
        })
      ];
    };
  }
}
```

### Installing the package directly

Alternatively, if you don't want to use an overlay, you can install Fjord Launcher directly by installing the `fjordlauncher` package.
This way the installed package is fully reproducible.

Example:

```nix
{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    fjordlauncher = {
      url = "github:unmojang/FjordLauncher";
      # Optional: Override the nixpkgs input of fjordlauncher to use the same revision as the rest of your flake
      # Note that overriding any input of fjordlauncher may break reproducibility
      # inputs.nixpkgs.follows = "nixpkgs";
    };
  };

  outputs = {nixpkgs, fjordlauncher}: {
    nixosConfigurations.foo = nixpkgs.lib.nixosSystem {
      system = "x86_64-linux";

      modules = [
        ({pkgs, ...}: {
          environment.systemPackages = [fjordlauncher.packages.${pkgs.system}.fjordlauncher];
        })
      ];
    };
  }
}
```

### Installing the package ad-hoc (`nix shell`, `nix run`, etc.)

You can simply call the default package of this flake.

Example:

```shell
nix run github:unmojang/FjordLauncher

nix shell github:unmojang/FjordLauncher

nix profile install github:unmojang/FjordLauncher
```

## Installing a development release (without flakes)

We use [garnix](https://garnix.io/) to build and cache our development builds.
If you want to avoid rebuilds you may add the garnix cache to your substitutors.

Example (NixOS):

```nix
{...}:
{
  nix.settings = {
    trusted-substituters = [
      "https://cache.garnix.io"
    ];

    trusted-public-keys = [
      "cache.garnix.io:CTFPyKSLcx5RMJKfLo5EEPUObbA78b0YQ2DTCJXqr9g="
    ];
  };
}
```

### Using the overlay (`fetchTarball`)

We use flake-compat to allow using this Flake on a system that doesn't use flakes.

Example:

```nix
{pkgs, ...}: {
  nixpkgs.overlays = [(import (builtins.fetchTarball "https://github.com/unmojang/FjordLauncher/archive/develop.tar.gz")).overlays.default];

  environment.systemPackages = [pkgs.fjordlauncher];
}
```

### Installing the package directly (`fetchTarball`)

Alternatively, if you don't want to use an overlay, you can install Fjord Launcher directly by installing the `fjordlauncher` package.
This way the installed package is fully reproducible.

Example:

```nix
{pkgs, ...}: {
  environment.systemPackages = [(import (builtins.fetchTarball "https://github.com/unmojang/FjordLauncher/archive/develop.tar.gz")).packages.${pkgs.system}.fjordlauncher];
}
```

### Installing the package ad-hoc (`nix-env`)

You can add this repository as a channel and install its packages that way.

Example:

```shell
nix-channel --add https://github.com/unmojang/FjordLauncher/archive/develop.tar.gz fjordlauncher

nix-channel --update fjordlauncher

nix-env -iA fjordlauncher.fjordlauncher
```

## Package variants

Both Nixpkgs and this repository offer the following packages:

- `fjordlauncher` - Preferred build using Qt 6
- `fjordlauncher-qt5` - Legacy build using Qt 5 (i.e. for Qt 5 theming support)

Both of these packages also have `-unwrapped` counterparts, that are not wrapped and can therefore be customized even further than what the wrapper packages offer.

### Customizing wrapped packages

The wrapped packages (`fjordlauncher` and `fjordlauncher-qt5`) offer some build parameters to further customize the launcher's environment.

The following parameters can be overridden:

- `msaClientID` (default: `null`, requires full rebuild!) Client ID used for Microsoft Authentication
- `gamemodeSupport` (default: `true`) Turn on/off support for [Feral GameMode](https://github.com/FeralInteractive/gamemode)
- `jdks` (default: `[ jdk17 jdk8 ]`) Java runtimes added to `FJORDLAUNCHER_JAVA_PATHS` variable
- `additionalLibs` (default: `[ ]`) Additional libraries that will be added to `LD_LIBRARY_PATH`
