#!/bin/bash
export DEST="./.exvim.cr_project"
export TOOLS="/home/corerain/.vim/tools/"
export TMP="${DEST}/_ID"
export TARGET="${DEST}/ID"
sh ${TOOLS}/shell/bash/update-idutils.sh
