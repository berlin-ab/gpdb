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

echo "Make"
make -j4 -s

echo "Make install"
make -s install

echo "Source greenplum path"
source ./greenplum-db-devel/greenplum_path.sh

echo "Start ssh daemon"
/start-sshd.bash

echo "Create the demo cluster"
make create-demo-cluster;

echo "Source the demo env"
source ./gpAux/gpdemo/gpdemo-env.sh

echo "Run installcheck-world"
make installcheck-world;

