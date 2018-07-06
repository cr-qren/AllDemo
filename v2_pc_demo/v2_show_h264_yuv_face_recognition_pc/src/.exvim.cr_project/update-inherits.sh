#!/bin/bash
export DEST="./.exvim.cr_project"
export TOOLS="/home/corerain/.vim/tools/"
export TMP="${DEST}/_inherits"
export TARGET="${DEST}/inherits"
sh ${TOOLS}/shell/bash/update-inherits.sh
