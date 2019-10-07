#include <setjmp.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cmockery.h"

/*
 * implements:
 */
#include "upgrade-helpers.h"


static void
copy_file_from_backup_to_datadir(char *filename, char *segment_path)
{
	char		buffer[2000];

	sprintf(buffer,
			"cp gpdb6-data-copy/%s/%s gpdb6-data/%s/%s",
			segment_path, filename, segment_path, filename);

	system(buffer);
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

static void
upgradeMaster(char *segment_path, int old_gp_dbid, int new_gp_dbid)
{
	char		buffer[2000];

	sprintf(buffer, ""
			"./gpdb6/bin/pg_upgrade "
			"--mode=dispatcher "
			"--link "
			"--old-bindir=./gpdb5/bin "
			"--new-bindir=./gpdb6/bin "
			"--old-datadir=./gpdb5-data/%s "
			"--new-datadir=./gpdb6-data/%s "
			"--old-gp-dbid=%d "
			"--new-gp-dbid=%d ",
			segment_path, segment_path, old_gp_dbid, new_gp_dbid);

	system(buffer);
}

static void
copy_tablespace_from(char *tablespace_location_directory, int source_dbid, int destination_dbid)
{
	char buffer[2000];

	sprintf(buffer,
	        "rsync -a --delete "
	        "%s/%d/ "
	        "%s/%d ",
	        tablespace_location_directory,
	        source_dbid,
	        tablespace_location_directory,
	        destination_dbid);
	system(buffer);
}

static void
update_symlinks_for_tablespaces_from(char *segment_path, char *new_tablespace_path)
{
	char buffer[2000];

	sprintf(buffer, "find ./gpdb6-data/%s/pg_tblspc/* | xargs -I '{}' ln -sfn %s '{}'",
		segment_path,
		new_tablespace_path);

	system(buffer);
}

static void
copy_master_data_directory_into_segment_data_directory(char *segment_path)
{
	char buffer[2000];
	char *master_data_directory_path = "qddir/demoDataDir-1";

	sprintf(buffer,
			"rsync -a --delete "
			"./gpdb6-data/%s/ "
			"./gpdb6-data/%s ",
			master_data_directory_path,
			segment_path);
	
	system(buffer);
}

static void
upgradeMasterWithTablespaces(char *segment_path, int old_gp_dbid, int new_gp_dbid, char *mappingFilePath)
{
	char		buffer[2000];

	sprintf(buffer, ""
			"./gpdb6/bin/pg_upgrade --retain "
			"--mode=dispatcher "
			"--link "
			"--old-bindir=./gpdb5/bin "
			"--new-bindir=./gpdb6/bin "
			"--old-datadir=./gpdb5-data/%s "
			"--new-datadir=./gpdb6-data/%s "
			"--old-gp-dbid=%d "
			"--new-gp-dbid=%d "
			"--old-tablespaces-file=%s ",
			segment_path, segment_path, old_gp_dbid, new_gp_dbid, mappingFilePath);

	system(buffer);
}

static void
upgradeSegment(char *segment_path, int old_gp_dbid, int new_gp_dbid)
{
	char		buffer[2000];

	copy_master_data_directory_into_segment_data_directory(
		segment_path);

	sprintf(buffer, ""
			"./gpdb6/bin/pg_upgrade --retain "
			"--mode=segment "
			"--link "
			"--old-bindir=./gpdb5/bin "
			"--new-bindir=./gpdb6/bin "
			"--old-datadir=./gpdb5-data/%s "
			"--new-datadir=./gpdb6-data/%s "
			"--old-gp-dbid=%d "
			"--new-gp-dbid=%d ",
			segment_path, segment_path, old_gp_dbid, new_gp_dbid);

	copy_configuration_files_from_backup_to_datadirs(
		segment_path);

	system(buffer);
}

static void
upgradeSegmentWithTablespaces(char *segment_path, int old_gp_dbid, int new_gp_dbid, char *mappingFilePath, char *tablespace_location_directory)
{
	char		buffer[2000];
	char new_tablespace_path[1000];

	sprintf(new_tablespace_path, "%s/%d", tablespace_location_directory, new_gp_dbid);

	copy_master_data_directory_into_segment_data_directory(
		segment_path);

	copy_tablespace_from(tablespace_location_directory, 1, new_gp_dbid);

	sprintf(buffer, ""
			"./gpdb6/bin/pg_upgrade --retain "
			"--mode=segment "
			"--link "
			"--old-bindir=./gpdb5/bin "
			"--new-bindir=./gpdb6/bin "
			"--old-datadir=./gpdb5-data/%s "
			"--new-datadir=./gpdb6-data/%s "
			"--old-gp-dbid=%d "
			"--new-gp-dbid=%d "
			"--old-tablespaces-file=%s ",
			segment_path, segment_path, old_gp_dbid, new_gp_dbid, mappingFilePath);

	system(buffer);

	update_symlinks_for_tablespaces_from(
		segment_path, new_tablespace_path);

	copy_configuration_files_from_backup_to_datadirs(
		segment_path);
}

void
performUpgrade(void)
{
	upgradeMaster("qddir/demoDataDir-1", 1, 1);
	copy_configuration_files_from_backup_to_datadirs(
		"qddir/demoDataDir-1");
	
	upgradeSegment("dbfast1/demoDataDir0", 2, 2);
	upgradeSegment("dbfast2/demoDataDir1", 3, 3);
	upgradeSegment("dbfast3/demoDataDir2", 4, 4);
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
	FILE	   *output_file;
	char	   *output;

	sprintf(buffer, ""
			"./gpdb6/bin/pg_upgrade "
			"--check "
			"--old-bindir=./gpdb5/bin "
			"--new-bindir=./gpdb6/bin "
			"--old-datadir=./gpdb5-data/%s "
			"--new-datadir=./gpdb6-data/%s ",
			master_data_directory_path, master_data_directory_path);

#ifndef WIN32
	output_file = popen(buffer, "r");

	while ((output = fgets(buffer, sizeof(buffer), output_file)) != NULL)
		if (strstr(output, error_message))
			count += 1;

	pclose(output_file);
#endif

	assert_true(count > 0);
}
