#!/bin/bash
export DEST="./.exvim.poya_API"
export TOOLS="/home/pi/.vim/tools/"
export TMP="${DEST}/_ID"
export TARGET="${DEST}/ID"
sh ${TOOLS}/shell/bash/update-idutils.sh
