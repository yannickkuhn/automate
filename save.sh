#!/bin/sh

git config --global user.name "julienMoran"
git config --global user.email "julien.moran@gmail.com"
git config remote.origin.url https://yannickkuhn:polytech06@github.com/yannickkuhn/automate.git
git add *
git commit -a -m "Changements de julien"
git push origin master