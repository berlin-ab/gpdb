#include <setjmp.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "cmockery.h"
#include "postgres_fe.h"

/*
 * implements:
 */
#include "upgrade-helpers.h"
#define UPGRADE_BASE_PORT 50432


static void
copy_file_from_backup_to_datadir(char *filename, char *segment_path)
{
	system(psprintf(
			"cp gpdb6-data-copy/%s/%s gpdb6-data/%s/%s",
			segment_path, filename, segment_path, filename));
}

static void
copy_configuration_files_from_backup_to_datadirs(char *segment_path)
{
	char	   *files_to_copy[] = {
		"internal.auto.conf",
		"postgresql.conf",
		"pg_hba.conf",
		"postmaster.opts",
		"postgresql.auto.conf"
	};

	for (int i = 0; i < 5; i++)
	{
		char	   *filename = files_to_copy[i];

		copy_file_from_backup_to_datadir(filename, segment_path);
	}
}

static char *
base_upgrade_executable_string(char *segment_path, int old_gp_dbid, int new_gp_dbid)
{
	return psprintf(
		"./gpdb6/bin/pg_upgrade "
		"--link "
		"--old-bindir=./gpdb5/bin "
		"--new-bindir=./gpdb6/bin "
		"--old-datadir=./gpdb5-data/%s "
		"--new-datadir=./gpdb6-data/%s "
		"--old-port=%d "
		"--new-port=%d "
		"--old-gp-dbid=%d "
		"--new-gp-dbid=%d ", 
		segment_path, segment_path,
		UPGRADE_BASE_PORT + old_gp_dbid,
		UPGRADE_BASE_PORT + new_gp_dbid,
		old_gp_dbid, new_gp_dbid
		);
}

static void
upgradeMaster(char *segment_path, int old_gp_dbid, int new_gp_dbid)
{
	system(psprintf(
		"%s "
		"--mode=dispatcher ",
		base_upgrade_executable_string(segment_path, old_gp_dbid, new_gp_dbid)));
}

static void
copy_tablespace_from(char *tablespace_location_directory, int source_dbid, int destination_dbid)
{
	system(psprintf(
		"rsync -a --delete "
		"%s/%d/ "
		"%s/%d ",
		tablespace_location_directory,
		source_dbid,
		tablespace_location_directory,
		destination_dbid));
}

static void
update_symlinks_for_tablespaces_from(char *segment_path, char *new_tablespace_path)
{
	system(psprintf("find ./gpdb6-data/%s/pg_tblspc/* | xargs -I '{}' ln -sfn %s '{}'",
		segment_path,
		new_tablespace_path));
}

static void
copy_master_data_directory_into_segment_data_directory(char *segment_path)
{
	char *master_data_directory_path = "qddir/demoDataDir-1";

	system(psprintf(
		"rsync -a --delete "
		"./gpdb6-data/%s/ "
		"./gpdb6-data/%s ",
		master_data_directory_path,
		segment_path));
}

static void
upgradeMasterWithTablespaces(char *segment_path, int old_gp_dbid, int new_gp_dbid, char *mappingFilePath)
{
	system(psprintf(
		"%s "
		"--mode=dispatcher "
		"--old-tablespaces-file=%s ",
		base_upgrade_executable_string(segment_path, old_gp_dbid, new_gp_dbid), 
		mappingFilePath));
}

static void
upgradeSegment(char *segment_path, int old_gp_dbid, int new_gp_dbid)
{
	copy_master_data_directory_into_segment_data_directory(
		segment_path);

	copy_configuration_files_from_backup_to_datadirs(
		segment_path);

	system(psprintf(
		"%s "
		"--mode=segment ",
		base_upgrade_executable_string(segment_path, old_gp_dbid, new_gp_dbid)));
}

static void
upgradeSegmentWithTablespaces(char *segment_path, int old_gp_dbid, int new_gp_dbid, char *mappingFilePath, char *tablespace_location_directory)
{
	copy_master_data_directory_into_segment_data_directory(
		segment_path);

	copy_tablespace_from(tablespace_location_directory, 1, new_gp_dbid);

	system(psprintf(
		"%s "
		"--mode=segment "
		"--old-tablespaces-file=%s ",
		base_upgrade_executable_string(segment_path, old_gp_dbid, new_gp_dbid),
		mappingFilePath));

	update_symlinks_for_tablespaces_from(
		segment_path, psprintf("%s/%d", tablespace_location_directory, new_gp_dbid));

	copy_configuration_files_from_backup_to_datadirs(
		segment_path);
}

static void *
parallel_upgradeSegment0(void *state)
{
	upgradeSegment("dbfast1/demoDataDir0", 2, 2);
}

static void *
parallel_upgradeSegment1(void *state)
{
	upgradeSegment("dbfast2/demoDataDir1", 3, 3);
}

static void *
parallel_upgradeSegment2(void *state)
{
	upgradeSegment("dbfast3/demoDataDir2", 4, 4);
}


void
performUpgrade(void)
{
	upgradeMaster("qddir/demoDataDir-1", 1, 1);
	copy_configuration_files_from_backup_to_datadirs(
		"qddir/demoDataDir-1");

	pthread_t thread_segment_zero;
	pthread_t thread_segment_one;
	pthread_t thread_segment_two;

	pthread_create(&thread_segment_zero, NULL, parallel_upgradeSegment0, NULL);
	pthread_create(&thread_segment_one, NULL, parallel_upgradeSegment1, NULL);
	pthread_create(&thread_segment_two, NULL, parallel_upgradeSegment2, NULL);

	pthread_join(thread_segment_zero, NULL);
	pthread_join(thread_segment_one, NULL);
	pthread_join(thread_segment_two, NULL);
}

void
performUpgradeWithTablespaces(char *mappingFilePath, char *tablespace_location_directory)
{
	upgradeMasterWithTablespaces("qddir/demoDataDir-1", 1, 1, mappingFilePath);
	upgradeSegmentWithTablespaces("dbfast1/demoDataDir0", 2, 2, mappingFilePath, tablespace_location_directory);
	upgradeSegmentWithTablespaces("dbfast2/demoDataDir1", 3, 3, mappingFilePath, tablespace_location_directory);
	upgradeSegmentWithTablespaces("dbfast3/demoDataDir2", 4, 4, mappingFilePath, tablespace_location_directory);
}

void
performUpgradeCheckFailsWithError(char *error_message)
{
	char		buffer[2000];
	int			count = 0;
	char	   *master_data_directory_path = "qddir/demoDataDir-1";
	int old_master_dbid = 1;
	int new_master_dbid = 1;
	FILE	   *output_file;
	char	   *output;

	char *command = psprintf(
		"%s"
		"--check ",
		base_upgrade_executable_string(master_data_directory_path, 
			old_master_dbid, 
			new_master_dbid));

#ifndef WIN32
	output_file = popen(command, "r");

	while ((output = fgets(buffer, sizeof(buffer), output_file)) != NULL)
		if (strstr(output, error_message))
			count += 1;

	pclose(output_file);
#endif

	assert_true(count > 0);
}
