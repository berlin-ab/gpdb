subdir=contrib/pg_upgrade/test/unit
top_srcdir=../../../..
top_builddir=../../../..
pg_upgrade_directory=../..

OBJS = tablespace_test.o old_tablespace_file_contents_test.o csv_parser_test.o

TARGETS = tablespace_test old_tablespace_file_contents_test csv_parser_test

include $(top_srcdir)/src/Makefile.global
include $(top_srcdir)/src/Makefile.mock

override CPPFLAGS += -I$(pg_upgrade_directory) \
	-I$(libpq_srcdir)


test_dependencies = $(CMOCKERY_OBJS)


debugging_flags = -Og -g -fsanitize=address

tablespace_test.t: tablespace_test.o $(pg_upgrade_directory)/tablespace.o $(test_dependencies)
	$(CC) $(CFLAGS) $(debugging_flags) $^ $(libpq_pgport) $(LDFLAGS) -o $@

old_tablespace_file_contents_test.t: old_tablespace_file_contents_test.o $(pg_upgrade_directory)/old_tablespace_file_contents.o $(test_dependencies)
	$(CC) $(CFLAGS) $(debugging_flags) $^ $(libpq_pgport) $(LDFLAGS) -o $@

csv_parser_test.t: csv_parser_test.o $(pg_upgrade_directory)/csv_parser.o $(test_dependencies)
	$(CC) $(CFLAGS) $(debugging_flags) $^ $(libpq_pgport) $(LDFLAGS) -o $@
