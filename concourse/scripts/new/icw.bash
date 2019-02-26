#!/usr/bin/env bash

export CC=$(which gcc)
export CXX=$(which g++)
export INSTALL_DIR="$PWD/greenplum-db-devel/"

./configure --prefix=${INSTALL_DIR} \
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
make create-demo-cluster;
source ./gpAux/gpdemo/gpdemo-env.sh
make installcheck-world;

