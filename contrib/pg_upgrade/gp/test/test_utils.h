#ifndef GPDB_TEST_UTILS_H
#define GPDB_TEST_UTILS_H

/*
 * Utility functions to conduct integration tests
 */

#include "../../pg_upgrade.h"
#include "libpq-fe.h"


extern ClusterInfo old_cluster;
extern ClusterInfo new_cluster;
extern OSInfo os_info;
extern UserOpts user_opts;

/*
 * Execute a query in normal mode
 */
void
executeQuery(PGconn *conn, const char *fmt,...);

void
setup_old_cluster();

char *
get_database_name();

void setup_os_info();

void
enable_utility_mode();

void
disable_utility_mode();

#endif //GPDB_TEST_UTILS_H
