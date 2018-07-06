#!/bin/bash
export DEST="./.exvim.cr_project"
export TOOLS="/home/corerain/.vim/tools/"
export TMP="${DEST}/_symbols"
export TARGET="${DEST}/symbols"
sh ${TOOLS}/shell/bash/update-symbols.sh
