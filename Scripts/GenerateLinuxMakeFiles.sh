#!/usr/bin/env bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

# Set the root directory of the project as the current working directory
pushd $SCRIPT_DIR/../
./Premake/bin/Linux/premake5 gmake2
# Restore the previous working directory
popd
read -p "Press [Enter] to exit the script"