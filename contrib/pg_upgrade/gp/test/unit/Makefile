subdir=contrib/pg_upgrade/gp/test/unit
top_srcdir=../../../../..
top_builddir=../../../../..


TARGETS = check_greenplum_test


include $(top_srcdir)/src/Makefile.mock
include $(top_srcdir)/src/Makefile.global


test_flags = -I $(top_srcdir)/$(subdir)/../../..


check_greenplum_test.t: $(CMOCKERY_OBJS) check_greenplum_test.c
	make -C ../../.. clean install

	$(CC) $(CFLAGS) $(CPPFLAGS) $(test_flags) \
		../../check_greenplum_internal.o \
		$^ \
		-o check_greenplum_test.t
