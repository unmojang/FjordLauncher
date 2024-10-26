{
  description = "Prism Launcher fork with support for alternative auth servers";

  nixConfig = {
    extra-substituters = [ "https://unmojang.cachix.org" ];
    extra-trusted-public-keys = [
      "unmojang.cachix.org-1:OfHnbBNduZ6Smx9oNbLFbYyvOWSoxb2uPcnXPj4EDQY="
    ];
  };

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

    libnbtplusplus = {
      url = "github:PrismLauncher/libnbtplusplus";
      flake = false;
    };

    nix-filter.url = "github:numtide/nix-filter";

    /*
      Inputs below this are optional and can be removed

      ```
      {
        inputs.fjordlauncher = {
          url = "github:unmojang/FjordLauncher";
          inputs = {
            flake-compat.follows = "";
          };
        };
      }
      ```
    */

    flake-compat = {
      url = "github:edolstra/flake-compat";
      flake = false;
    };
  };

  outputs =
    {
      self,
      nixpkgs,
      libnbtplusplus,
      nix-filter,
      ...
    }:
    let
      inherit (nixpkgs) lib;

      # While we only officially support aarch and x86_64 on Linux and MacOS,
      # we expose a reasonable amount of other systems for users who want to
      # build for most exotic platforms
      systems = lib.systems.flakeExposed;

      forAllSystems = lib.genAttrs systems;
      nixpkgsFor = forAllSystems (system: nixpkgs.legacyPackages.${system});
    in
    {
      checks = forAllSystems (
        system:
        let
          checks' = nixpkgsFor.${system}.callPackage ./nix/checks.nix { inherit self; };
        in
        lib.filterAttrs (_: lib.isDerivation) checks'
      );

      devShells = forAllSystems (
        system:
        let
          pkgs = nixpkgsFor.${system};
        in
        {
          default = pkgs.mkShell {
            inputsFrom = [ self.packages.${system}.fjordlauncher-unwrapped ];
            buildInputs = with pkgs; [
              ccache
              ninja
            ];
          };
        }
      );

      formatter = forAllSystems (system: nixpkgsFor.${system}.nixfmt-rfc-style);

      overlays.default =
        final: prev:
        let
          version = builtins.substring 0 8 self.lastModifiedDate or "dirty";
        in
        {
          fjordlauncher-unwrapped = prev.callPackage ./nix/unwrapped.nix {
            inherit
              libnbtplusplus
              nix-filter
              self
              version
              ;
          };

          fjordlauncher = final.callPackage ./nix/wrapper.nix { };
        };

      packages = forAllSystems (
        system:
        let
          pkgs = nixpkgsFor.${system};

          # Build a scope from our overlay
          fjordPackages = lib.makeScope pkgs.newScope (final: self.overlays.default final pkgs);

          # Grab our packages from it and set the default
          packages = {
            inherit (fjordPackages) fjordlauncher-unwrapped fjordlauncher;
            default = fjordPackages.fjordlauncher;
          };
        in
        # Only output them if they're available on the current system
        lib.filterAttrs (_: lib.meta.availableOn pkgs.stdenv.hostPlatform) packages
      );

      # We put these under legacyPackages as they are meant for CI, not end user consumption
      legacyPackages = forAllSystems (
        system:
        let
          fjordPackages = self.packages.${system};
          legacyPackages = self.legacyPackages.${system};
        in
        {
          fjordlauncher-debug = fjordPackages.fjordlauncher.override {
            fjordlauncher-unwrapped = legacyPackages.fjordlauncher-unwrapped-debug;
          };

          fjordlauncher-unwrapped-debug = fjordPackages.fjordlauncher-unwrapped.overrideAttrs {
            cmakeBuildType = "Debug";
            dontStrip = true;
          };
        }
      );
    };
}
