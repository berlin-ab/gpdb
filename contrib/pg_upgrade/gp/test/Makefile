subdir=contrib/pg_upgrade/gp/test
top_srcdir=../../../..
top_builddir=../../../..

TARGETS = check_integration_test check_greenplum_test
PG_LIBS = $(libpq_pgport)

include $(top_srcdir)/src/Makefile.mock
include $(top_srcdir)/src/Makefile.global

test_flags = -I $(top_srcdir)/$(subdir)/../.. -I$(libpq_srcdir)


code_under_test = ../check_covering_aoindex.o \
	../definitions_utilities.o \
	../checks.o


pg_upgrade_dependencies = ../../server.o \
	../../util.o \
	../../relfilenode.o \
	../../info.o \
	../../aomd_filehandler.o \
	../../gpdb4_heap_convert.o \
	../../file.o \
	../../version_gp.o \
	../../parallel.o \
	../../file_gp.o \
	../../reporting.o


check_integration_test.t: $(CMOCKERY_OBJS) check_covering_aoindex_test.c
	make -C ../.. clean install
	
	$(CC) $(CFLAGS) $(CPPFLAGS) $(test_flags) \
		$(code_under_test) \
		$(pg_upgrade_dependencies) \
		$^ \
		$(PG_LIBS) \
		-o ./check_integration_test.t


check_greenplum_test.t: $(CMOCKERY_OBJS) check_greenplum_test.c
	make -C ../.. clean install

	$(CC) $(CFLAGS) $(CPPFLAGS) $(test_flags) \
		../check_greenplum.o ../definitions_utilities.o \
		$^ \
		$(PG_LIBS) \
		-o check_greenplum_test.t
