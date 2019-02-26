#!/usr/bin/env bash

export CC=$(which gcc)
export CXX=$(which g++)
export INSTALL_DIR="$PWD/greenplum-db-devel/"

./configure --with-perl \
	    --with-python \
	    --with-libxml \
	    --disable-gpfdist \
	    --enable-mapreduce \
	    --with-zstd \
	    --prefix=${INSTALL_DIR} ${CONFIGURE_FLAGS};

make -j4 -s
make -s install
source ./greenplum-db-devel/greenplum_path.sh
make create-demo-cluster;
source ./gpAux/gpdemo/gpdemo-env.sh
make installcheck-world;

