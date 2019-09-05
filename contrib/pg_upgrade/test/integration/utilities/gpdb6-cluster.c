#include "stdlib.h"

#include "gpdb6-cluster.h"

void
startGpdbSixCluster(void)
{
	system(""
		"source ./gpdb6/greenplum_path.sh; "
		"PGPORT=60000; "
		"MASTER_DATA_DIRECTORY=./gpdb6-data/qddir/demoDataDir-1; "
		"./gpdb6/bin/gpstart -a"
	);
}

void
stopGpdbSixCluster(void)
{
	system(""
	       "source ./gpdb6/greenplum_path.sh; \n"
	       "PGPORT=60000; \n"
	       "MASTER_DATA_DIRECTORY=./gpdb6-data/qddir/demoDataDir-1; \n"
	       "./gpdb6/bin/gpstop -a"
	);
}

