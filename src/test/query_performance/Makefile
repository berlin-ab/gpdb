subdir = src/test/query_performance
top_builddir = ../../..
include $(top_builddir)/src/Makefile.global


REGRESS_OPTS = --init-file=../regress/init_file --schedule query_performance_schedule --dbname query_performance_regression


installcheck:
	$(pg_regress_installcheck) $(REGRESS_OPTS) 
