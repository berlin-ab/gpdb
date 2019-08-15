subdir=contrib/pg_upgrade/gp/test
top_srcdir=../../../..
top_builddir=../../../..

TARGETS = check_integration_test
PG_LIBS = $(libpq_pgport)

include $(top_srcdir)/src/Makefile.mock
include $(top_srcdir)/src/Makefile.global

test_flags = -I $(top_srcdir)/$(subdir)/../.. -I$(libpq_srcdir)


code_under_test = ../check_covering_aoindex.o \
	../definitions_utilities.o \


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


check_integration_test.t: $(CMOCKERY_OBJS) $(pg_upgrade_dependencies) check_covering_aoindex_test.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(test_flags) \
		$(code_under_test) \
		$^ \
		$(PG_LIBS) \
		-o check_covering_aoindex.t
