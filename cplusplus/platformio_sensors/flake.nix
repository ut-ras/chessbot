{
  description = "Scratch dir for various testing";
  inputs = {
custom-nixpkgs.url = "https://github.com/NixOS/nixpkgs/archive/3592b10a67b518700002f1577e301d73905704fe.tar.gz";
  };

  outputs = {custom-nixpkgs, ...}: let
    system = "x86_64-linux";
    pkgs = import custom-nixpkgs { system = system; config.allowUnfree = true; };
  mypython = pkgs.python3.withPackages (ps: with ps; [ platformio ]);
lib = pkgs.lib;
  in {

    devShells.${system}.default = (pkgs.mkShell {
  NIX_LD_LIBRARY_PATH = lib.makeLibraryPath [
    pkgs.stdenv.cc.cc
    # ...
  ];
  NIX_LD = lib.fileContents "${pkgs.stdenv.cc}/nix-support/dynamic-linker";
  name =  "platformio-fancy";
  packages = with pkgs; [
    # for pio cli and vscode extension
    platformio
    mypython
    # for running openocd manually
    openocd
    # for running openocd via pio cli or vscode extension
    libusb1 # libusb-1.0.so.0
    hidapi # libhidapi-hidraw.so.0

(vscode-with-extensions.override {
    vscodeExtensions = with vscode-extensions; [
	vscodevim.vim
	ms-vscode.cpptools
    ] ++ pkgs.vscode-utils.extensionsFromVscodeMarketplace [
      {
        name = "platformio-ide";
        publisher = "platformio";
        version = "3.3.3";
        sha256 = "sha256-pcWKBqtpU7DVpiT7UF6Zi+YUKknyjtXFEf5nL9+xuSo=";
      }
];

  })];
});
  };
}
