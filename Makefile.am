noinst_LTLIBRARIES =

include $(top_srcdir)/common.mk

# thrift compiler command. Note that the --out value for the output
# directory.
THRIFT_BUILD = $(THRIFT) --gen cpp --out . $<

####
# Remember to run the generate_thrift_automake.py script once we add a .thrift
# file OR a new service to an existing .thrift file.

# AM_MAKEFLAGS = -W$(top_srcdir)/src/thrift.mk
# $(top_srcdir)/src/thrift.mk:
# 	./generate_thrift_automake.py *.thrift > thrift.mk
####
include thrift.mk


bin_PROGRAMS = prop_logic_main
prop_logic_main_SOURCES = prop_logic_main.cc
prop_logic_main_LDADD = libproposition.la

noinst_LTLIBRARIES += libproposition.la
libproposition_la_SOURCES = proposition.cc proposition.h

noinst_LTLIBRARIES += libbdd_tree.la
libbdd_tree_la_SOURCES = bdd_tree.cc bdd_tree.h


BUILT_SOURCES = parser.h

AM_YFLAGS = -d
AM_LFLAGS = -olex.yy.c

bin_PROGRAMS += wiki_main
wiki_main_SOURCES = lexer.ll parser.y wiki_main.cc expression.c
wiki_main_LDADD = libproposition.la libbdd_tree.la -lre2 -lleveldb -lsnappy

bin_PROGRAMS += buddy
buddy_SOURCES = buddy.cc

clean-local:
	-rm -f lexer.c lexer.cc lexer.h lexer.hh parser.c parser.cc parser.h parser.hh
