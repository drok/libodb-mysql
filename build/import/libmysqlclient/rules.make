# file      : build/import/libmysqlclient/rules.make
# copyright : Copyright (c) 2009-2019 Code Synthesis Tools CC
# license   : GNU GPL v2; see accompanying LICENSE file

$(dcf_root)/import/libmysqlclient/%: root := $(libmysqlclient_root)

ifeq ($(libmysqlclient_type),archive)

$(dcf_root)/import/libmysqlclient/mysqlclient.l: $(libmysqlclient_root)/libmysql/.libs/libmysqlclient.a
	@echo $< >$@
else

$(dcf_root)/import/libmysqlclient/mysqlclient.l: $(libmysqlclient_root)/libmysql/.libs/libmysqlclient.so
	@echo $< >$@
	@echo rpath:$(root)/libmysql/.libs >>$@
endif

$(dcf_root)/import/libmysqlclient/mysqlclient.l.cpp-options:
	@ln -s $(root)/include $(dcf_root)/import/libmysqlclient/mysql
	@echo include: -I$(dcf_root)/import/libmysqlclient >$@

ifndef %foreign%

disfigure::
	$(call message,rm $(dcf_root)/import/libmysqlclient/mysqlclient.l,\
rm -f $(dcf_root)/import/libmysqlclient/mysqlclient.l)
	$(call message,,rm -f $(dcf_root)/import/libmysqlclient/mysql)
	$(call message,,rm -f $(dcf_root)/import/libmysqlclient/mysqlclient.l.cpp-options)

endif
