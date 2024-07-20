{
  inputs,
  self,
  ...
}: {
  perSystem = {
    lib,
    pkgs,
    ...
  }: {
    packages = let
      ourPackages = lib.fix (final: self.overlays.default final pkgs);
    in {
      inherit
        (ourPackages)
        fjordlauncher-unwrapped
        fjordlauncher
        ;
      default = ourPackages.fjordlauncher;
    };
  };

  flake = {
    overlays.default = final: prev: let
      version = builtins.substring 0 8 self.lastModifiedDate or "dirty";
    in {
      fjordlauncher-unwrapped = prev.callPackage ./pkg {
        inherit (inputs) libnbtplusplus;
        inherit version;
      };

      fjordlauncher = prev.qt6Packages.callPackage ./pkg/wrapper.nix {
        inherit (final) fjordlauncher-unwrapped;
      };
    };
  };
}
