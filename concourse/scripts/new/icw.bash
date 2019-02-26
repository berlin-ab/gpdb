#!/usr/bin/env bash

set -o errexit

sudo chown -R $(whoami) $(pwd)

export INSTALL_DIR="$PWD/greenplum-db-devel/"
export CCACHE_DIR="$PWD/.ccache"

fetch_and_build_xerces_c() {
    git clone https://github.com/greenplum-db/gp-xerces /tmp/xerces
    (
	    set -e
	    cd /tmp/xerces
	    ./configure CC='ccache gcc' CXX='ccache g++' --disable-network
    )
    make_install -C /tmp/xerces
}

fetch_and_build_orca() {
    fetch_and_build_xerces_c
    
    local orca_code_url
    orca_code_url=$(
	sed -E -n -e '/gporca/s,.*https://github.com/greenplum-db/gporca/releases/download/v(([[:digit:]]|\.)+)/bin_orca_centos5_release.tar.gz.*,https://github.com/greenplum-db/gporca/archive/v\1.tar.gz,p' <gpAux/releng/releng.mk
	)
    mkdir /tmp/orca
    wget -O - "${orca_code_url}" | tar zx --strip-components=1 -C /tmp/orca

    cmake -GNinja -H/tmp/orca -B/tmp/orca/build
    ninja install -C /tmp/orca/build
}

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

