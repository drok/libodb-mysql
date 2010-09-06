# file      : odb/mysql/makefile
# author    : Boris Kolpackov <boris@codesynthesis.com>
# copyright : Copyright (c) 2009-2010 Code Synthesis Tools CC
# license   : GNU GPL v2; see accompanying LICENSE file

include $(dir $(lastword $(MAKEFILE_LIST)))../../build/bootstrap.make

cxx :=                 \
connection.cxx         \
connection-factory.cxx \
database.cxx           \
exceptions.cxx         \
transaction.cxx        \
transaction-impl.cxx   \
statement.cxx          \
query.cxx              \
traits.cxx

cxx_tun := $(cxx)
cxx_obj := $(addprefix $(out_base)/,$(cxx_tun:.cxx=.o))
cxx_od  := $(cxx_obj:.o=.o.d)

odb_mysql.l             := $(out_base)/odb-mysql.l
odb_mysql.l.cpp-options := $(out_base)/odb-mysql.l.cpp-options

default := $(out_base)/
dist    := $(out_base)/.dist
clean   := $(out_base)/.clean

# Import.
#
$(call import,\
  $(scf_root)/import/libodb/stub.make,\
  l: odb.l,cpp-options: odb.l.cpp-options)

#$(call import,\
#  $(scf_root)/import/libmysqlclient/stub.make,\
#  l: mysql.l,cpp-options: mysql.l.cpp-options)

$(call import,\
  $(scf_root)/import/libmysqlclient_r/stub.make,\
  l: mysql.l,cpp-options: mysql.l.cpp-options)

# Build.
#
$(odb_mysql.l): $(cxx_obj) $(odb.l) $(mysql.l)
$(odb_mysql.l.cpp-options): value := -I$(out_root) -I$(src_root)
$(odb_mysql.l.cpp-options): $(odb.l.cpp-options) $(mysql.l.cpp-options)

$(cxx_obj) $(cxx_od): $(odb_mysql.l.cpp-options) $(out_base)/details/config.h

$(call include-dep,$(cxx_od))

$(out_base)/details/config.h:
	@echo '// file      : odb/mysql/details/config.h'                >$@
	@echo '// author    : automatically generated'                  >>$@
	@echo ''                                                        >>$@
	@echo '#ifndef ODB_MYSQL_DETAILS_CONFIG_H'                      >>$@
	@echo '#define ODB_MYSQL_DETAILS_CONFIG_H'                      >>$@
	@echo ''                                                        >>$@
	@echo '#define LIBODB_MYSQL_INCLUDE_LONG 1'                     >>$@
	@echo ''                                                        >>$@
	@echo '#endif // ODB_MYSQL_DETAILS_CONFIG_H'                    >>$@

# Convenience alias for default target.
#
$(out_base)/: $(odb_mysql.l)

# Dist.
#
$(dist): export sources := $(cxx)
$(dist): export headers = $(subst $(src_base)/,,$(shell find $(src_base) \
-name '*.hxx' -o -name '*.ixx' -o -name '*.txx'))
$(dist): export extra_dist := libodb-mysql-vc9.vcproj \
libodb-mysql-vc10.vcxproj libodb-mysql-vc10.vcxproj.filters
$(dist): export interface_version = $(shell sed -e \
's/^\([0-9]*\.[0-9]*\).*/\1/' $(src_root)/version)

$(dist):
	$(call dist-data,$(sources) $(headers) details/config.h.in)
	$(call meta-vc9proj,$(src_base)/libodb-mysql-vc9.vcproj)
	$(call meta-vc10proj,$(src_base)/libodb-mysql-vc10.vcxproj)
	$(call meta-automake)

# Clean.
#
$(clean): $(odb_mysql.l).o.clean    \
  $(odb_mysql.l.cpp-options).clean  \
  $(addsuffix .cxx.clean,$(cxx_obj)) \
  $(addsuffix .cxx.clean,$(cxx_od))
	$(call message,rm $$1,rm -f $$1,$(out_base)/details/config.h)

# Generated .gitignore.
#
ifeq ($(out_base),$(src_base))
$(odb_mysql.l): | $(out_base)/.gitignore

$(out_base)/.gitignore: files := details/config.h
$(clean): $(out_base)/.gitignore.clean

$(call include,$(bld_root)/git/gitignore.make)
endif

# How to.
#
$(call include,$(bld_root)/dist.make)
$(call include,$(bld_root)/meta/vc9proj.make)
$(call include,$(bld_root)/meta/vc10proj.make)
$(call include,$(bld_root)/meta/automake.make)

$(call include,$(bld_root)/cxx/o-l.make)
$(call include,$(bld_root)/cxx/cxx-o.make)
$(call include,$(bld_root)/cxx/cxx-d.make)
