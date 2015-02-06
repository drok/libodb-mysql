# file      : build/export/libodb-mysql/stub.make
# copyright : Copyright (c) 2009-2015 Code Synthesis Tools CC
# license   : GNU GPL v2; see accompanying LICENSE file

$(call include-once,$(src_root)/odb/mysql/makefile,$(out_root))

$(call export,\
  l: $(out_root)/odb/mysql/odb-mysql.l,\
  cpp-options: $(out_root)/odb/mysql/odb-mysql.l.cpp-options)
