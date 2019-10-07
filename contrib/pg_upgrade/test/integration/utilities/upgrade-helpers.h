
#ifndef PG_UPGRADE_INTEGRATION_TEST_UPGRADE_HELPERS
#define PG_UPGRADE_INTEGRATION_TEST_UPGRADE_HELPERS

void generateFilespaceToTablespaceMap(char *mapping_filename, char *target_filespace_location);
void performUpgrade(void);
void performUpgradeWithTablespaces(char *mappingFilePath, char *tablespace_location_directory);
void performUpgradeCheckFailsWithError(char *message);

#endif							/* PG_UPGRADE_INTEGRATION_TEST_UPGRADE_HELPERS */
