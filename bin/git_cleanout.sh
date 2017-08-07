#!/bin/bash

if [ ! -d .git ]; then
    echo "No directory '.git' here. Is this a repo?"
    exit 1
fi

function git_cleanout_here() {
    if [ ! -e .git ]; then
        echo "No file/dir '.git' here. Is this a repo?"
        return 1
    fi
    rm -rf ./deps
    git clean -dfx
    git reset --hard
    git status
    git describe --always
}

export -f git_cleanout_here

git submodule foreach git_cleanout_here
git submodule update --init
git_cleanout_here

if [ -f .git/hooks/updateHooks.py ]; then
    .git/hooks/updateHooks.py
elif [ -f ./initialise_hooks ]; then
    ./initialise_hooks
else
    echo "Warning: Git hooks not initialised or updated."
fi
