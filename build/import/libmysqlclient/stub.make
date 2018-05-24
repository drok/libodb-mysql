# file      : build/import/libmysqlclient/stub.make
# copyright : Copyright (c) 2009-2018 Code Synthesis Tools CC
# license   : GNU GPL v2; see accompanying LICENSE file

$(call include-once,$(scf_root)/import/libmysqlclient/configuration-rules.make,$(dcf_root))

libmysqlclient_installed :=

$(call -include,$(dcf_root)/import/libmysqlclient/configuration-dynamic.make)

ifdef libmysqlclient_installed

ifeq ($(libmysqlclient_installed),y)

$(call export,l: -lmysqlclient,cpp-options: )

else

$(call include-once,$(scf_root)/import/libmysqlclient/rules.make,$(dcf_root))

$(call export,\
  l: $(dcf_root)/import/libmysqlclient/mysqlclient.l,\
  cpp-options: $(dcf_root)/import/libmysqlclient/mysqlclient.l.cpp-options)

endif

else

.NOTPARALLEL:

endif
