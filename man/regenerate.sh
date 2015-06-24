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

for LANG in */; do
  pushd "${LANG}"
  for ENGINE in anomaly bandit burst classifier clustering graph nearest_neighbor recommender regression; do
    ln -sf "jubatus_server.8" "juba${ENGINE}.8"
    ln -sf "jubatus_proxy.8"  "juba${ENGINE}_proxy.8"
  done
  popd
done
