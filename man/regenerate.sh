#!/bin/bash -uex

BRANCH="${1:-master}"
WEBSITE_DIR="tmp-website"

rm -rf "${WEBSITE_DIR}"
git clone https://github.com/jubatus/website.git "${WEBSITE_DIR}"
pushd "${WEBSITE_DIR}"
[ ! -z "${BRANCH}" ] && git checkout "${BRANCH}"
omake man
popd
cp -a "${WEBSITE_DIR}"/build/man/* .
rm -rf "${WEBSITE_DIR}"
