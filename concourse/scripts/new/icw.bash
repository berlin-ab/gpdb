#!/usr/bin/env bash

set -o errexit

sudo chown -R $(whoami) $(pwd)

export INSTALL_DIR="$PWD/greenplum-db-devel/"
export CCACHE_DIR="$PWD/.ccache"

./configure CC='ccache gcc' \
	    CXX='ccache g++' \
	    --prefix=${INSTALL_DIR} \
	    --with-perl \
	    --with-python \
	    --with-libxml \
	    --with-zstd \
	    --enable-mapreduce \
	    --disable-gpfdist \
	    ${CONFIGURE_FLAGS};

make -j4 -s
make -s install
source ./greenplum-db-devel/greenplum_path.sh

{
  ssh-keyscan localhost
  ssh-keyscan 0.0.0.0
  ssh-keyscan `hostname`
} >> "$HOME/.ssh/known_hosts"

make create-demo-cluster;
source ./gpAux/gpdemo/gpdemo-env.sh
make installcheck-world;

