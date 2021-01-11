#!/usr/bin/env python3

import argparse
import contextlib
import os
import pathlib
import platform
import shutil
import subprocess


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser()
    parser.add_argument('--clean', action='store_true', help='Clean build')
    parser.add_argument('--verbose', action='store_true', help='Enable verbose build output')
    parser.add_argument('--src-dir', help='Source directory. By default look for a parent "SaikoEngine" directory')
    parser.add_argument('--bld-dir', default='.', help='Build directory, use current dir if not specified')
    parser.add_argument('--compiler', choices=['gcc', 'clang', 'msvc'], help='Compiler, defaults to "msvc" on Windows and "clang" on Linux (one of: %(choices)s')
    parser.add_argument('--build-type', default='debug', choices=['debug', 'release'], help='Build type (one of: %(choices)s, default=%(default)s)')
    parser.add_argument('target', nargs='*', help='Target(s) to build')

    args = parser.parse_args()

    if not args.src_dir:
        args.src_dir = find_parent_dir(args.bld_dir, 'SaikoEngine')

    if args.compiler is None:
        if platform.system() == 'Windows':
            args.compiler = 'msvc'
        elif platform.system() == 'Linux':
            args.compiler = 'clang'
        else:
            parser.error(f'Invalid OS: {platform.system()}')

    return args


def find_parent_dir(root_dir: str, target_dir: str) -> str:
    root = pathlib.Path(root_dir).resolve()
    current = root
    while current.stem != target_dir and current.stem != '':
        current = current.parent
    if current.stem == '':
        raise RuntimeError(f'Could not parent directory named "{target_dir}" from "{root}"')

    return str(current)


def main():
    args = parse_args()

    if args.bld_dir:
        os.chdir(os.path.expanduser(args.bld_dir))

    if args.clean:
        clean(args.bld_dir)

    if should_run_conan(args.bld_dir):
        print('--- Running Conan ---')
        subprocess.run(['conan', 'install', args.src_dir, '--profile', f'{args.compiler}_{args.build_type}'], check=True)
    if should_run_cmake(args.bld_dir):
        print('--- Running CMake ---')
        subprocess.run(['cmake', args.src_dir], check=True)

    bld_cmd = ['cmake', '--build', args.bld_dir, '--config', args.build_type]
    if args.clean:
        bld_cmd.append('--clean-first')
    if args.verbose:
        bld_cmd.append('--verbose')
    if args.target:
        for tgt in args.target:
            bld_cmd.extend(['--target', tgt])
    print('--- Running build ---')
    subprocess.run(bld_cmd, check=True, stderr=subprocess.STDOUT)


def clean(bld_dir: str):
    print('--- Cleaning ---')
    with contextlib.suppress(FileNotFoundError):
        os.remove(os.path.join(bld_dir, 'CMakeCache.txt'))
    with contextlib.suppress(FileNotFoundError):
        shutil.rmtree(os.path.join(bld_dir, 'CMakeFiles'))


def should_run_conan(bld_dir: str) -> bool:
    return not os.path.exists(os.path.join(bld_dir, 'conaninfo.txt'))


def should_run_cmake(bld_dir: str) -> bool:
    return not os.path.exists(os.path.join(bld_dir, 'CMakeCache.txt'))


if __name__ == '__main__':
    main()
