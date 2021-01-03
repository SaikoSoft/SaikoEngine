((nil . (
         ;; WSL has a bug where EXEs hang inside of Emacs.
         ;; Piping to `tee` works around the bug.
         ;; `pipefail` is needed to prevent `tee` from swallowing error codes.
         ;; https://github.com/microsoft/WSL/issues/4412
         (projectile-project-compilation-cmd . "set -o pipefail && cmd /c 'python C:\\code\\SaikoEngine\\src\\tools\\build.py --src-dir=C:\\code\\SaikoEngine --bld-dir=C:\\code\\SaikoEngine\\bld\\msvc --build-type=debug | tee'")
         )))
