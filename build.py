# File: build.py
# Copyright (C) 2023  Jason Allen
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>. aa

# Before running this script, you need to initialize the build environment
# One way to to this is with vcvarsall.bat:
#   $ vcvarsall.bat x64

import os
import shutil
import subprocess

# Build settings
useNinja = True
cleanPreviousBuild = True
buildDir = 'objects'
exportDir = 'export'

# Clean all files from previous builds
if (cleanPreviousBuild and os.path.exists(buildDir)):
    shutil.rmtree(buildDir)

# Configure project using from CMakeLists.txt
if (useNinja):
    subprocess.call(f'cmake -S . -B {buildDir} -G "Ninja" -DCMAKE_BUILD_TYPE=Release')
else:
    subprocess.call(f'cmake -S . -B {buildDir} -G "Visual Studio 17 2022" -T host=x64 -A x64')

# Build and install project
subprocess.call(f'cmake --build {buildDir} --config Release')
subprocess.call(f'cmake --install {buildDir} --prefix {exportDir}')
