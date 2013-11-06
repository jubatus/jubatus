#!/bin/sh

rm -f version.ml
if [ ! $VERSION ]; then
    # Check if it is a git repository
    git status > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        HASH=`git log -1 --format="%H" -- .`
        VERSION=`git describe ${HASH}`/`git name-rev --name-only HEAD`
    else
        VERSION='undefined version'
    fi
fi

sed -e "s|%%VERSION%%|${VERSION}|" version.mlp > version.ml
