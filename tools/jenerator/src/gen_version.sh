#!/bin/sh

if [ ! $VERSION ]; then
    # Check if it is a git repository
    git status > /dev/null 2>&1
    if [ $? -eq 0 ]; then
        VERSION=`git describe`/`git name-rev --name-only HEAD`
    else
        VERSION='undefined version'
    fi
fi

sed -e "s|%%VERSION%%|${VERSION}|" version.mlp > version.ml