{
  pkgs ? import <nixos-unstable> { },
}:
pkgs.mkShell {
  nativeBuildInputs = with pkgs; [
    inkscape
    icoutils
    imagemagick
    nodePackages_latest.svgo
  ];
}
