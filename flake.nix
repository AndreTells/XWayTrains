# flake.nix
{
  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-24.11";
    utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, utils }:
    utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; config.allowUnfree = true; };
      in
      {
        devShells.default = with pkgs; mkShell {

          packages = with pkgs; [
            clang
            gnumake
            clang-tools
            gdb
            valgrind
            man-pages
            man-db
            clang-manpages
          ];
          shellHook = ''
            echo "Entering dev shell"
          '';
        };
      }
    );
}
