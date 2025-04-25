{
  lib,
  stdenv,
  cmake,
  cmark,
  darwin,
  extra-cmake-modules,
  gamemode,
  ghc_filesystem,
  jdk17,
  kdePackages,
  libnbtplusplus,
  ninja,
  self,
  stripJavaArchivesHook,
  tomlplusplus,
  zlib,

  msaClientID ? null,
  gamemodeSupport ? stdenv.hostPlatform.isLinux,
}:

assert lib.assertMsg (
  gamemodeSupport -> stdenv.hostPlatform.isLinux
) "gamemodeSupport is only available on Linux.";

let
  date =
    let
      # YYYYMMDD
      date' = lib.substring 0 8 self.lastModifiedDate;
      year = lib.substring 0 4 date';
      month = lib.substring 4 2 date';
      date = lib.substring 6 2 date';
    in
    if (self ? "lastModifiedDate") then
      lib.concatStringsSep "-" [
        year
        month
        date
      ]
    else
      "unknown";
in

stdenv.mkDerivation {
  pname = "fjordlauncher-unwrapped";
  version = "9.4-unstable-${date}";

  src = lib.fileset.toSource {
    root = ../.;
    fileset = lib.fileset.unions [
      ../CMakeLists.txt
      ../COPYING.md

      ../buildconfig
      ../cmake
      ../launcher
      ../libraries
      ../program_info
      ../tests
    ];
  };

  postUnpack = ''
    rm -rf source/libraries/libnbtplusplus
    ln -s ${libnbtplusplus} source/libraries/libnbtplusplus
  '';

  nativeBuildInputs = [
    cmake
    ninja
    extra-cmake-modules
    jdk17
    stripJavaArchivesHook
  ];

  buildInputs =
    [
      cmark
      ghc_filesystem
      kdePackages.qtbase
      kdePackages.qtnetworkauth
      kdePackages.quazip
      tomlplusplus
      zlib
    ]
    ++ lib.optionals stdenv.hostPlatform.isDarwin [ darwin.apple_sdk.frameworks.Cocoa ]
    ++ lib.optional gamemodeSupport gamemode;

  hardeningEnable = lib.optionals stdenv.hostPlatform.isLinux [ "pie" ];

  cmakeFlags =
    [
      # downstream branding
      (lib.cmakeFeature "Launcher_BUILD_PLATFORM" "nixpkgs")
    ]
    ++ lib.optionals (msaClientID != null) [
      (lib.cmakeFeature "Launcher_MSA_CLIENT_ID" (toString msaClientID))
    ]
    ++ lib.optionals (lib.versionOlder kdePackages.qtbase.version "6") [
      (lib.cmakeFeature "Launcher_QT_VERSION_MAJOR" "5")
    ]
    ++ lib.optionals stdenv.hostPlatform.isDarwin [
      # we wrap our binary manually
      (lib.cmakeFeature "INSTALL_BUNDLE" "nodeps")
      # disable built-in updater
      (lib.cmakeFeature "MACOSX_SPARKLE_UPDATE_FEED_URL" "''")
      (lib.cmakeFeature "CMAKE_INSTALL_PREFIX" "${placeholder "out"}/Applications/")
    ];

  doCheck = true;

  dontWrapQtApps = true;

  meta = {
    description = "Prism Launcher fork with support for alternative auth servers";
    longDescription = ''
      Allows you to have multiple, separate instances of Minecraft (each with
      their own mods, texture packs, saves, etc) and helps you manage them and
      their associated options with a simple interface.
    '';
    homepage = "https://github.com/unmojang/FjordLauncher";
    license = lib.licenses.gpl3Only;
    maintainers = with lib.maintainers; [
      evan-goode
    ];
    mainProgram = "fjordlauncher";
    platforms = lib.platforms.linux ++ lib.platforms.darwin;
  };
}
