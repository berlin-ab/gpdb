#ifndef GPDB_CHECK_UTILS_H
#define GPDB_CHECK_UTILS_H

/*
 * contrib/pg_upgrade/gp/definitions.h
 *
 * Objects required to run Greenplum-specific checks.
 *
 * This file redeclares functions from pg_upgrade. Original versions are not
 * used in order to:
 * 1. Eliminate dependency of checks on utilities implementation;
 * 2. Facilitate unit test conduction;
 * 3. Preserve pg_upgrade functions originating from upstream.
 */


/*
 * Common upgrade-specific stuff. These are redefinitions in case upgrade is
 * not included
 */
#ifndef PG_UPGRADE_H

#include <unistd.h>
#include <assert.h>
#include <sys/stat.h>
#include <sys/time.h>

#include "postgres.h"
#include "libpq-fe.h"
#include "pqexpbuffer.h"

/* Use port in the private/dynamic port number range */
#define DEF_PGUPORT			50432

/* Allocate for null byte */
#define USER_NAME_SIZE		128

#define MAX_STRING			1024
#define LINE_ALLOC			4096
#define QUERY_ALLOC			8192

#define NUMERIC_ALLOC 100

#define MIGRATOR_API_VERSION	1

#define MESSAGE_WIDTH		60

#define GET_MAJOR_VERSION(v)	((v) / 100)


/* needs to be kept in sync with pg_class.h */
#define RELSTORAGE_EXTERNAL	'x'
#define RELSTORAGE_AOROWS	'a'
#define RELSTORAGE_AOCOLS	'c'


#define CLUSTER_NAME(cluster)	((cluster) == &old_cluster ? "old" : \
								 (cluster) == &new_cluster ? "new" : "none")

#define atooid(x)  ((Oid) strtoul((x), NULL, 10))

/* OID system catalog preservation added during PG 9.0 development */
#define TABLE_SPACE_SUBDIRS_CAT_VER 201001111
/* postmaster/postgres -b (binary_upgrade) flag added during PG 9.1 development */
/* In GPDB, it was introduced during GPDB 5.0 development. */
#define BINARY_UPGRADE_SERVER_FLAG_CAT_VER 301607301
/*
 *	Visibility map changed with this 9.2 commit,
 *	8f9fe6edce358f7904e0db119416b4d1080a83aa; pick later catalog version.
 */
#define VISIBILITY_MAP_CRASHSAFE_CAT_VER 201107031

/*
 * change in JSONB format during 9.4 beta
 */
#define JSONB_FORMAT_CHANGE_CAT_VER 201409291

/*
 * pg_multixact format changed in 9.3 commit 0ac5ad5134f2769ccbaefec73844f85,
 * ("Improve concurrency of foreign key locking") which also updated catalog
 * version to this value.  pg_upgrade behavior depends on whether old and new
 * server versions are both newer than this, or only the new one is.
 *
 * In GPDB: that upstream change was merged into GPDB in the big 9.3 merge
 * commit.
 */
#define MULTIXACT_FORMATCHANGE_CAT_VER 301809211

/*
 * Extra information stored for each Append-only table.
 * This is used to transfer the information from the auxiliary
 * AO table to the new cluster.
 */

/* To hold contents of pg_visimap_<oid> */
typedef struct
{
	int			segno;
	int64		first_row_no;
	char	   *visimap;		/* text representation of the "bit varying" field */
} AOVisiMapInfo;

typedef struct
{
	int			segno;
	int			columngroup_no;
	int64		first_row_no;
	char	   *minipage;		/* text representation of the "bit varying" field */
} AOBlkDir;

/* To hold contents of pg_aoseg_<oid> */
typedef struct
{
	int			segno;
	int64		eof;
	int64		tupcount;
	int64		varblockcount;
	int64		eofuncompressed;
	int64		modcount;
	int16		version;
	int16		state;
} AOSegInfo;

/* To hold contents of pf_aocsseg_<oid> */
typedef struct
{
	int         segno;
	int64		tupcount;
	int64		varblockcount;
	char       *vpinfo;
	int64		modcount;
	int16		state;
	int16		version;
} AOCSSegInfo;

typedef struct
{
	int16		attlen;
	char		attalign;
	bool		is_numeric;
} AttInfo;

typedef enum
{
	HEAP,
	AO,
	AOCS,
	FSM
} RelType;

/*
 * Each relation is represented by a relinfo structure.
 */
typedef struct
{
	/* Can't use NAMEDATALEN;  not guaranteed to fit on client */
	char	   *nspname;		/* namespace name */
	char	   *relname;		/* relation name */
	Oid			reloid;			/* relation oid */
	char		relstorage;
	Oid			relfilenode;	/* relation relfile node */
	/* GPDB_96_MERGE_FIXME: indtable and toastheap are backported from 9.6. */
	Oid			indtable;		/* if index, OID of its table, else 0 */
	Oid			toastheap;		/* if toast table, OID of base table, else 0 */
	/* relation tablespace path, or "" for the cluster default */
	char	   *tablespace;
	bool		nsp_alloc;
	bool		tblsp_alloc;

	RelType		reltype;

	/* Extra information for append-only tables */
	AOSegInfo  *aosegments;
	AOCSSegInfo *aocssegments;
	int			naosegments;
	AOVisiMapInfo *aovisimaps;
	int			naovisimaps;
	AOBlkDir   *aoblkdirs;
	int			naoblkdirs;

	/* Extra information for heap tables */
	bool		gpdb4_heap_conversion_needed;
	bool		has_numerics;
	AttInfo	   *atts;
	int			natts;
} RelInfo;

typedef struct
{
	RelInfo    *rels;
	int			nrels;
} RelInfoArr;

/*
 * Structure to store database information
 */
typedef struct
{
	Oid			db_oid;			/* oid of the database */
	char	   *db_name;		/* database name */
	char		db_tablespace[MAXPGPATH];		/* database default tablespace
												 * path */
	RelInfoArr	rel_arr;		/* array of all user relinfos */

	char	   *reserved_oids;	/* as a string */
} DbInfo;

typedef struct
{
	DbInfo	   *dbs;			/* array of db infos */
	int			ndbs;			/* number of db infos */
} DbInfoArr;

/*
 * The following structure is used to hold pg_control information.
 * Rather than using the backend's control structure we use our own
 * structure to avoid pg_control version issues between releases.
 */
typedef struct
{
	uint32		ctrl_ver;
	uint32		cat_ver;
	char		nextxlogfile[25];
	uint32		chkpnt_nxtxid;
	uint32		chkpnt_nxtepoch;
	uint32		chkpnt_nxtoid;
	uint32		chkpnt_nxtmulti;
	uint32		chkpnt_nxtmxoff;
	uint32		chkpnt_oldstMulti;
	uint32		align;
	uint32		blocksz;
	uint32		largesz;
	uint32		walsz;
	uint32		walseg;
	uint32		ident;
	uint32		index;
	uint32		toast;
	bool		date_is_int;
	bool		float8_pass_by_value;
	bool		data_checksum_version;
	char	   *lc_collate;
	char	   *lc_ctype;
	char	   *encoding;
} ControlData;

/*
 * cluster
 *
 *	information about each cluster
 */
typedef struct
{
	ControlData controldata;	/* pg_control information */
	DbInfoArr	dbarr;			/* dbinfos array */
	char	   *pgdata;			/* pathname for cluster's $PGDATA directory */
	char	   *pgconfig;		/* pathname for cluster's config file
								 * directory */
	char	   *bindir;			/* pathname for cluster's executable directory */
	char	   *pgopts;			/* options to pass to the server, like pg_ctl
								 * -o */
	char	   *sockdir;		/* directory for Unix Domain socket, if any */
	unsigned short port;		/* port number where postmaster is waiting */
	uint32		major_version;	/* PG_VERSION of cluster */
	char		major_version_str[64];	/* string PG_VERSION of cluster */
	uint32		bin_version;	/* version returned from pg_ctl */
	Oid			pg_database_oid;	/* OID of pg_database relation */
	Oid			install_role_oid;		/* OID of connected role */
	Oid			role_count;		/* number of roles defined in the cluster */
	const char *tablespace_suffix;		/* directory specification */

	char	   *global_reserved_oids; /* OID preassign calls for shared objects */
} ClusterInfo;

/*
 * Enumeration to denote link modes
 */
typedef enum
{
	TRANSFER_MODE_COPY,
	TRANSFER_MODE_LINK
} transferMode;

/*
 * Enumeration to denote checksum modes
 */
typedef enum
{
	CHECKSUM_NONE = 0,
	CHECKSUM_ADD,
	CHECKSUM_REMOVE
} checksumMode;

typedef enum
{
	DISPATCHER = 0,
	SEGMENT
} segmentMode;

/*
 *	UserOpts
*/
typedef struct
{
	bool		check;			/* TRUE -> ask user for permission to make
								 * changes */
	transferMode transfer_mode; /* copy files or link them? */
	int			jobs;			/* number of processes/threads to use */
	char	   *socketdir;		/* directory to use for Unix sockets */

	bool		progress;
	segmentMode	segment_mode;
	checksumMode checksum_mode;

} UserOpts;

ClusterInfo old_cluster;

extern UserOpts user_opts;

/*
 * Enumeration to denote pg_log modes
 */
typedef enum
{
	PG_VERBOSE,
	PG_STATUS,
	PG_REPORT,
	PG_WARNING,
	PG_FATAL
} eLogType;

/*
 * connectToServer()
 *
 *	Connects to the desired database on the designated server.
 *	If the connection attempt fails, this function logs an error
 *	message and calls exit() to kill the program.
 */
PGconn *
connectToServer(ClusterInfo *cluster, const char *db_name);

/*
 * executeQueryOrDie()
 *
 *	Formats a query string from the given arguments and executes the
 *	resulting query.  If the query fails, this function logs an error
 *	message and calls exit() to kill the program.
 */
PGresult *
executeQueryOrDie(PGconn *conn, const char *fmt,...)
__attribute__((format(PG_PRINTF_ATTRIBUTE, 2, 3)));

void
report_status(eLogType type, const char *fmt,...)
__attribute__((format(PG_PRINTF_ATTRIBUTE, 2, 3)));

void
pg_log(eLogType type, const char *fmt,...)
__attribute__((format(PG_PRINTF_ATTRIBUTE, 2, 3)));

void
pg_fatal(const char *fmt,...)
__attribute__((format(PG_PRINTF_ATTRIBUTE, 1, 2), noreturn));

/*
 * prep_status
 *
 *	Displays a message that describes an operation we are about to begin.
 *	We pad the message out to MESSAGE_WIDTH characters so that all of the "ok" and
 *	"failed" indicators line up nicely.
 *
 *	A typical sequence would look like this:
 *		prep_status("about to flarb the next %d files", fileCount );
 *
 *		if(( message = flarbFiles(fileCount)) == NULL)
 *		  report_status(PG_REPORT, "ok" );
 *		else
 *		  pg_log(PG_FATAL, "failed - %s\n", message );
 */
void
prep_status(const char *fmt,...)
__attribute__((format(PG_PRINTF_ATTRIBUTE, 1, 2)));

/*
 * Set the current check as "successful"
 */
void		check_ok(void);

#endif /* PG_UPGRADE_H */


/*
 * Greenplum-specific stuff
 */

/*
 * Check function definition.
 * Such functions actually perform checks on Greenplum
 */
typedef bool (*check_function)(void);

/*
 * A list of functions to be run to conduct tests
 */
extern check_function GP_CHECKS_LIST[];
extern size_t GP_CHECKS_LIST_LENGTH;

/*
 * gp_check_failure()
 *
 * Check if failure happened during check execution
 */
void
gp_check_failure(const char *restrict fmt,...)
__attribute__((format(PG_PRINTF_ATTRIBUTE, 1, 2)));

/*
 * Conducts check using provided check function
 */
void
conduct_check(bool (*check_function) (void))
__attribute__((nonnull(1)));


#endif //GPDB_CHECK_UTILS_H
