# file      : build/import/libmysqlclient_r/rules.make
# copyright : Copyright (c) 2009-2018 Code Synthesis Tools CC
# license   : GNU GPL v2; see accompanying LICENSE file

$(dcf_root)/import/libmysqlclient_r/%: root := $(libmysqlclient_r_root)

ifeq ($(libmysqlclient_r_type),archive)

$(dcf_root)/import/libmysqlclient_r/mysqlclient_r.l: $(libmysqlclient_r_root)/libmysql/.libs/libmysqlclient_r.a
	@echo $< >$@
else

$(dcf_root)/import/libmysqlclient_r/mysqlclient_r.l: $(libmysqlclient_r_root)/libmysql/.libs/libmysqlclient_r.so
	@echo $< >$@
	@echo rpath:$(root)/libmysql/.libs >>$@
endif

$(dcf_root)/import/libmysqlclient_r/mysqlclient_r.l.cpp-options:
	@ln -s $(root)/include $(dcf_root)/import/libmysqlclient_r/mysql
	@echo include: -I$(dcf_root)/import/libmysqlclient_r >$@

ifndef %foreign%

disfigure::
	$(call message,rm $(dcf_root)/import/libmysqlclient_r/mysqlclient_r.l,\
rm -f $(dcf_root)/import/libmysqlclient_r/mysqlclient_r.l)
	$(call message,,rm -f $(dcf_root)/import/libmysqlclient_r/mysql)
	$(call message,,rm -f $(dcf_root)/import/libmysqlclient_r/mysqlclient_r.l.cpp-options)

endif
