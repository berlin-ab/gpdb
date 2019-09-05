#include "stdlib.h"

/*
 * implements:
 */
#include "upgrade-helpers.h"

void
upgradeMaster()
{
	system(""
	       "./gpdb6/bin/pg_upgrade "
	       "--mode=dispatcher "
	       "--old-bindir=./gpdb5/bin "
	       "--new-bindir=./gpdb6/bin "
	       "--old-datadir=./gpdb5-data/qddir/demoDataDir-1 "
	       "--new-datadir=./gpdb6-data/qddir/demoDataDir-1 "
	);
}

void
upgradeContentId0()
{
	system("rsync -a --delete ./gpdb6-data/qddir/demoDataDir-1/ ./gpdb6-data/dbfast1/demoDataDir0");

	system(""
	       "./gpdb6/bin/pg_upgrade "
	       "--mode=segment "
	       "--old-bindir=./gpdb5/bin "
	       "--new-bindir=./gpdb6/bin "
	       "--old-datadir=./gpdb5-data/dbfast1/demoDataDir0 "
	       "--new-datadir=./gpdb6-data/dbfast1/demoDataDir0 "
	);
}

void
upgradeContentId1()
{
	system("rsync -a --delete ./gpdb6-data/qddir/demoDataDir-1/ ./gpdb6-data/dbfast2/demoDataDir1");

	system(""
	       "./gpdb6/bin/pg_upgrade "
	       "--mode=segment "
	       "--old-bindir=./gpdb5/bin "
	       "--new-bindir=./gpdb6/bin "
	       "--old-datadir=./gpdb5-data/dbfast2/demoDataDir1 "
	       "--new-datadir=./gpdb6-data/dbfast2/demoDataDir1 "
	);
}

void
upgradeContentId2()
{
	system("rsync -a --delete ./gpdb6-data/qddir/demoDataDir-1/ ./gpdb6-data/dbfast3/demoDataDir2");

	system(""
	       "./gpdb6/bin/pg_upgrade "
	       "--mode=segment "
	       "--old-bindir=./gpdb5/bin "
	       "--new-bindir=./gpdb6/bin "
	       "--old-datadir=./gpdb5-data/dbfast3/demoDataDir2 "
	       "--new-datadir=./gpdb6-data/dbfast3/demoDataDir2 "
	);
}
