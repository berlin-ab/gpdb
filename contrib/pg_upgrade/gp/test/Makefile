subdir=contrib/pg_upgrade/gp/test
top_srcdir=../../../..
top_builddir=../../../..

TARGETS = check_integration_test
PG_LIBS = $(libpq_pgport)

include $(top_srcdir)/src/Makefile.mock
include $(top_srcdir)/src/Makefile.global

test_flags = -I $(top_srcdir)/$(subdir)/../.. -I$(libpq_srcdir)

code_under_test = ../check_covering_aoindex.o \
	../definitions_utilities.o


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
	../../reporting.c


check_integration_test.t: $(CMOCKERY_OBJS) check_integration_test.c
	make -C ../.. install

	$(CC) $(CFLAGS) $(CPPFLAGS) $(test_flags) \
		$(code_under_test) \
		$(pg_upgrade_dependencies) \
		$^ \
		$(PG_LIBS_INTERNAL) \
		$(LDFLAGS) \
		$(LDFLAGS_EX) \
		$(PG_LIBS) \
		$(LIBS) \
		-o check_integration_test.t