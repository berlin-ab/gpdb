subdir=contrib/pg_upgrade/gp/test/integration
top_srcdir=../../../../..
top_builddir=../../../../..
pg_upgrade_directory=../../..
gp_checks_directory=../..


TARGETS = check_integration_test
PG_LIBS = $(libpq_pgport)


include $(top_srcdir)/src/Makefile.mock
include $(top_srcdir)/src/Makefile.global


test_flags = -I $(top_srcdir)/$(subdir)/../../.. \
	-I$(libpq_srcdir)


code_under_test = $(gp_checks_directory)/check_covering_aoindex.o \
	$(gp_checks_directory)/checks.o


pg_upgrade_dependencies = $(pg_upgrade_directory)/server.o \
	$(pg_upgrade_directory)/util.o \
	$(pg_upgrade_directory)/relfilenode.o \
	$(pg_upgrade_directory)/info.o \
	$(pg_upgrade_directory)/aomd_filehandler.o \
	$(pg_upgrade_directory)/gpdb4_heap_convert.o \
	$(pg_upgrade_directory)/file.o \
	$(pg_upgrade_directory)/version_gp.o \
	$(pg_upgrade_directory)/parallel.o \
	$(pg_upgrade_directory)/file_gp.o \
	$(pg_upgrade_directory)/reporting.o


check_integration_test.t: $(CMOCKERY_OBJS) check_covering_aoindex_test.c test_utils.c pg_upgrade_fakes.c
	make -C ../../.. clean install
	
	$(CC) $(CFLAGS) $(CPPFLAGS) $(test_flags) \
		$(code_under_test) \
		$(pg_upgrade_dependencies) \
		$^ \
		$(PG_LIBS) \
		-o ./check_integration_test.t
