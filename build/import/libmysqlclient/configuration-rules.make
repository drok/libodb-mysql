# file      : build/import/libmysqlclient/configuration-rules.make
# copyright : Copyright (c) 2009-2012 Boris Kolpackov
# license   : GNU GPL v2; see accompanying LICENSE file

$(dcf_root)/import/libmysqlclient/configuration-dynamic.make: | $(dcf_root)/import/libmysqlclient/.
	$(call message,,$(scf_root)/import/libmysqlclient/configure $@)

ifndef %foreign%

disfigure::
	$(call message,rm $(dcf_root)/import/libmysqlclient/configuration-dynamic.make,\
rm -f $(dcf_root)/import/libmysqlclient/configuration-dynamic.make)

endif
