# file      : build/import/libmysqlclient_r/stub.make
# author    : Boris Kolpackov <boris@kolpackov.net>
# copyright : Copyright (c) 2009-2011 Boris Kolpackov
# license   : GNU GPL v2; see accompanying LICENSE file

$(call include-once,$(scf_root)/import/libmysqlclient_r/configuration-rules.make,$(dcf_root))

libmysqlclient_r_installed :=

$(call -include,$(dcf_root)/import/libmysqlclient_r/configuration-dynamic.make)

ifdef libmysqlclient_r_installed

ifeq ($(libmysqlclient_r_installed),y)

$(call export,l: -lmysqlclient_r,cpp-options: )

else

$(call include-once,$(scf_root)/import/libmysqlclient_r/rules.make,$(dcf_root))

$(call export,\
  l: $(dcf_root)/import/libmysqlclient_r/mysqlclient_r.l,\
  cpp-options: $(dcf_root)/import/libmysqlclient_r/mysqlclient_r.l.cpp-options)

endif

else

.NOTPARALLEL:

endif
