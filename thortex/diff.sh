#!/bin/sh 

git diff upstream/master | cat > upstream.patch
grep diff upstream.patch | sed -e 's/diff --git //;' | awk '{print $1}';


