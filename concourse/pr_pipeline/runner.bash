#!/usr/bin/env bash

set -o errexit

export GPDB_INSTALL_DIR="$PWD/greenplum-db-devel/"
export CCACHE_DIR="$PWD/.ccache"


regain_ownership_of_working_directory() {
    sudo chown -R $(whoami) $(pwd)
}


fetch_and_build_xerces_c() {
    git clone https://github.com/greenplum-db/gp-xerces /tmp/xerces
    (
	    set -e
	    cd /tmp/xerces
	    ./configure CC='ccache gcc' \
			CXX='ccache g++' \
			--disable-network

    )
    make -j 4 -s -C /tmp/xerces
    sudo make install -C /tmp/xerces
}


fetch_and_build_orca() {
    local orca_code_url
    orca_code_url=$(
	sed -E -n -e '/gporca/s,.*https://github.com/greenplum-db/gporca/releases/download/v(([[:digit:]]|\.)+)/bin_orca_centos5_release.tar.gz.*,https://github.com/greenplum-db/gporca/archive/v\1.tar.gz,p' <gpAux/releng/releng.mk
	)
    mkdir /tmp/orca
    wget -O - "${orca_code_url}" | tar zx --strip-components=1 -C /tmp/orca

    cmake -GNinja -H/tmp/orca -B/tmp/orca/build
    ninja -C /tmp/orca/build
    sudo ninja install -C /tmp/orca/build
}


install_gpdb() {
    ./configure CC='ccache gcc' \
		CXX='ccache g++' \
		--prefix=${GPDB_INSTALL_DIR} \
		--with-perl \
		--with-python \
		--with-libxml \
		--with-zstd \
		--enable-mapreduce \
		${CONFIGURE_FLAGS};

    make -j4 -s

    make -s install
}


install_demo_cluster() {
    source ./greenplum-db-devel/greenplum_path.sh
    make create-demo-cluster;
    source ./gpAux/gpdemo/gpdemo-env.sh
}


_main() {
    regain_ownership_of_working_directory
    fetch_and_build_xerces_c
    fetch_and_build_orca
    install_gpdb
    /start-sshd.bash
    install_demo_cluster
    
    make $MAKE_TASK;    
}


_main "$@"
