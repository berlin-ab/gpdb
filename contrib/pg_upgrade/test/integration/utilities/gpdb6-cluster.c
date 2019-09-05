#include "stdlib.h"


#include "gpdb6-cluster.h"

static void copy_file_from_backup_to_datadir(char *filename, char *segment_path)
{
	char *buffer[2000];

	sprintf(buffer,
		"cp gpdb6-data-copy/%s/%s gpdb6-data/%s/%s",
		segment_path, filename, segment_path, filename);

	system(buffer);
}

static void
copy_configuration_files_from_backup_to_datadirs()
{
	char * files_to_copy[] = {
		"internal.auto.conf",
		"postgresql.conf",
		"pg_hba.conf",
		"postmaster.opts",
		"postgresql.auto.conf"
	};

	for (int i = 0; i < 5; i++) {
		char *filename = files_to_copy[i];
		copy_file_from_backup_to_datadir(filename, "qddir/demoDataDir-1");
		copy_file_from_backup_to_datadir(filename, "dbfast1/demoDataDir0");
		copy_file_from_backup_to_datadir(filename, "dbfast2/demoDataDir1");
		copy_file_from_backup_to_datadir(filename, "dbfast3/demoDataDir2");
	}
}

void
startGpdbSixCluster(void)
{
	copy_configuration_files_from_backup_to_datadirs();

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

