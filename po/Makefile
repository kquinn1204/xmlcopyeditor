# Makefile for program source directory in GNU NLS utilities package.
# Copyright (C) 1995, 1996, 1997 by Ulrich Drepper <drepper@gnu.ai.mit.edu>
# Copyright (C) 2004-2008 Rodney Dawes <dobey.pwns@gmail.com>
#
# This file may be copied and used freely without restrictions.  It may
# be used in projects which are not available under a GNU Public License,
# but which still want to provide support for the GNU gettext functionality.
#
# - Modified by Owen Taylor <otaylor@redhat.com> to use GETTEXT_PACKAGE
#   instead of PACKAGE and to look for po2tbl in ./ not in intl/
#
# - Modified by jacob berkman <jacob@ximian.com> to install
#   Makefile.in.in and po2tbl.sed.in for use with glib-gettextize
#
# - Modified by Rodney Dawes <dobey.pwns@gmail.com> for use with intltool
#
# We have the following line for use by intltoolize:
# INTLTOOL_MAKEFILE

GETTEXT_PACKAGE = xmlcopyeditor
PACKAGE = xmlcopyeditor
VERSION = 1.3.1.0

SHELL = /bin/sh

srcdir = .
top_srcdir = ..
top_builddir = ..


prefix = /usr/local
exec_prefix = ${prefix}
datadir = ${datarootdir}
datarootdir = ${prefix}/share
libdir = ${exec_prefix}/lib
localedir = ${datarootdir}/locale
subdir = po
install_sh = ${SHELL} /home/kquinn/xml-copy-editor/xmlcopyeditor-1.3.1.0/install-sh
# Automake >= 1.8 provides $(MKDIR_P).
# Until it can be supposed, use the safe fallback:
mkdir_p = $(install_sh) -d

INSTALL = /usr/bin/install -c
INSTALL_DATA = ${INSTALL} -m 644

GMSGFMT = /usr/bin/msgfmt
MSGFMT = /usr/bin/msgfmt
XGETTEXT = /usr/bin/xgettext
INTLTOOL_UPDATE = /usr/bin/intltool-update
INTLTOOL_EXTRACT = /usr/bin/intltool-extract
MSGMERGE = INTLTOOL_EXTRACT="$(INTLTOOL_EXTRACT)" XGETTEXT="$(XGETTEXT)" srcdir=$(srcdir) $(INTLTOOL_UPDATE) --gettext-package $(GETTEXT_PACKAGE) --dist
GENPOT   = INTLTOOL_EXTRACT="$(INTLTOOL_EXTRACT)" XGETTEXT="$(XGETTEXT)" srcdir=$(srcdir) $(INTLTOOL_UPDATE) --gettext-package $(GETTEXT_PACKAGE) --pot

ALL_LINGUAS = 

PO_LINGUAS=$(shell if test -r $(srcdir)/LINGUAS; then grep -v "^\#" $(srcdir)/LINGUAS; else echo "$(ALL_LINGUAS)"; fi)

USER_LINGUAS=$(shell if test -n "$(LINGUAS)"; then LLINGUAS="$(LINGUAS)"; ALINGUAS="$(ALL_LINGUAS)"; for lang in $$LLINGUAS; do if test -n "`grep \^$$lang$$ $(srcdir)/LINGUAS 2>/dev/null`" -o -n "`echo $$ALINGUAS|tr ' ' '\n'|grep \^$$lang$$`"; then printf "$$lang "; fi; done; fi)

USE_LINGUAS=$(shell if test -n "$(USER_LINGUAS)" -o -n "$(LINGUAS)"; then LLINGUAS="$(USER_LINGUAS)"; else if test -n "$(PO_LINGUAS)"; then LLINGUAS="$(PO_LINGUAS)"; else LLINGUAS="$(ALL_LINGUAS)"; fi; fi; for lang in $$LLINGUAS; do printf "$$lang "; done)

POFILES=$(shell LINGUAS="$(PO_LINGUAS)"; for lang in $$LINGUAS; do printf "$$lang.po "; done)

DISTFILES = Makefile.in.in POTFILES.in $(POFILES)
EXTRA_DISTFILES = ChangeLog POTFILES.skip Makevars LINGUAS

POTFILES = \
	../data/xmlcopyeditor.appdata.xml.in \
	../data/xmlcopyeditor.desktop.in \
	../src/aboutdialog.cpp \
	../src/aboutdialog.h \
	../src/associatedialog.cpp \
	../src/associatedialog.h \
	../src/binaryfile.cpp \
	../src/binaryfile.h \
	../src/casehandler.cpp \
	../src/casehandler.h \
	../src/catalogresolver.cpp \
	../src/catalogresolver.h \
	../src/commandpanel.cpp \
	../src/commandpanel.h \
	../src/contexthandler.cpp \
	../src/contexthandler.h \
	../src/dtd2schema.cpp \
	../src/dtd2schema.h \
	../src/exportdialog.cpp \
	../src/exportdialog.h \
	../src/findreplacepanel.cpp \
	../src/findreplacepanel.h \
	../src/getword.cpp \
	../src/getword.h \
	../src/globalreplacedialog.cpp \
	../src/globalreplacedialog.h \
	../src/housestyle.cpp \
	../src/housestyle.h \
	../src/housestylereader.cpp \
	../src/housestylereader.h \
	../src/housestylewriter.cpp \
	../src/housestylewriter.h \
	../src/insertpanel.cpp \
	../src/insertpanel.h \
	../src/locationpanel.cpp \
	../src/locationpanel.h \
	../src/mp3album.cpp \
	../src/mp3album.h \
	../src/myhtmlpane.cpp \
	../src/myhtmlpane.h \
	../src/myipc.cpp \
	../src/myipc.h \
	../src/mynotebook.cpp \
	../src/mynotebook.h \
	../src/myprintout.cpp \
	../src/myprintout.h \
	../src/mypropertysheet.cpp \
	../src/mypropertysheet.h \
	../src/nocasecompare.cpp \
	../src/nocasecompare.h \
	../src/parserdata.h \
	../src/pathresolver.cpp \
	../src/pathresolver.h \
	../src/readfile.cpp \
	../src/readfile.h \
	../src/replace.cpp \
	../src/replace.h \
	../src/rule.cpp \
	../src/rule.h \
	../src/stringset.h \
	../src/styledialog.cpp \
	../src/styledialog.h \
	../src/threadreaper.cpp \
	../src/threadreaper.h \
	../src/validationthread.cpp \
	../src/validationthread.h \
	../src/wrapaspell.cpp \
	../src/wrapaspell.h \
	../src/wrapdaisy.cpp \
	../src/wrapdaisy.h \
	../src/wrapexpat.cpp \
	../src/wrapexpat.h \
	../src/wraplibxml.cpp \
	../src/wraplibxml.h \
	../src/wrapregex.cpp \
	../src/wrapregex.h \
	../src/wraptempfilename.cpp \
	../src/wraptempfilename.h \
	../src/wrapxerces.cpp \
	../src/wrapxerces.h \
	../src/xercescatalogresolver.cpp \
	../src/xercescatalogresolver.h \
	../src/xmlassociatedtd.cpp \
	../src/xmlassociatedtd.h \
	../src/xmlassociatexsd.cpp \
	../src/xmlassociatexsd.h \
	../src/xmlassociatexsl.cpp \
	../src/xmlassociatexsl.h \
	../src/xmlcopyeditorcopy.h \
	../src/xmlcopyeditor.cpp \
	../src/xmlcopyeditor.h \
	../src/xmlcopyeditor_private.h \
	../src/xmlcopyimg.cpp \
	../src/xmlcopyimg.h \
	../src/xmlctrl.cpp \
	../src/xmlctrl.h \
	../src/xmldoc.cpp \
	../src/xmldoc.h \
	../src/xmlencodinghandler.cpp \
	../src/xmlencodinghandler.h \
	../src/xmlencodingspy.cpp \
	../src/xmlencodingspy.h \
	../src/xmlfilterreader.cpp \
	../src/xmlfilterreader.h \
	../src/xmlparseschemans.cpp \
	../src/xmlparseschemans.h \
	../src/xmlprodnote.cpp \
	../src/xmlprodnote.h \
	../src/xmlpromptgenerator.cpp \
	../src/xmlpromptgenerator.h \
	../src/xmlrulereader.cpp \
	../src/xmlrulereader.h \
	../src/xmlschemagenerator.cpp \
	../src/xmlschemagenerator.h \
	../src/xmlschemalocator.cpp \
	../src/xmlschemalocator.h \
	../src/xmlschemaparser.cpp \
	../src/xmlschemaparser.h \
	../src/xmlshallowvalidator.cpp \
	../src/xmlshallowvalidator.h \
	../src/xmlsuppressprodnote.cpp \
	../src/xmlsuppressprodnote.h \
	../src/xmlutf8reader.cpp \
	../src/xmlutf8reader.h \
	../src/xmlwordcount.cpp \
	../src/xmlwordcount.h \
	../src/xsllocator.cpp \
	../src/xsllocator.h

CATALOGS=$(shell LINGUAS="$(USE_LINGUAS)"; for lang in $$LINGUAS; do printf "$$lang.gmo "; done)

.SUFFIXES:
.SUFFIXES: .po .pox .gmo .mo .msg .cat

AM_DEFAULT_VERBOSITY = 1
INTLTOOL_V_MSGFMT = $(INTLTOOL__v_MSGFMT_$(V))
INTLTOOL__v_MSGFMT_= $(INTLTOOL__v_MSGFMT_$(AM_DEFAULT_VERBOSITY))
INTLTOOL__v_MSGFMT_0 = @echo "  MSGFMT" $@;

.po.pox:
	$(MAKE) $(GETTEXT_PACKAGE).pot
	$(MSGMERGE) $* $(GETTEXT_PACKAGE).pot -o $*.pox

.po.mo:
	$(INTLTOOL_V_MSGFMT)$(MSGFMT) -o $@ $<

.po.gmo:
	$(INTLTOOL_V_MSGFMT)file=`echo $* | sed 's,.*/,,'`.gmo \
	  && rm -f $$file && $(GMSGFMT) -o $$file $<

.po.cat:
	sed -f ../intl/po2msg.sed < $< > $*.msg \
	  && rm -f $@ && gencat $@ $*.msg


all: all-yes

all-yes: $(CATALOGS)
all-no:

$(GETTEXT_PACKAGE).pot: $(POTFILES)
	$(GENPOT)

install: install-data
install-data: install-data-yes
install-data-no: all
install-data-yes: all
	linguas="$(USE_LINGUAS)"; \
	for lang in $$linguas; do \
	  dir=$(DESTDIR)$(localedir)/$$lang/LC_MESSAGES; \
	  $(mkdir_p) $$dir; \
	  if test -r $$lang.gmo; then \
	    $(INSTALL_DATA) $$lang.gmo $$dir/$(GETTEXT_PACKAGE).mo; \
	    echo "installing $$lang.gmo as $$dir/$(GETTEXT_PACKAGE).mo"; \
	  else \
	    $(INSTALL_DATA) $(srcdir)/$$lang.gmo $$dir/$(GETTEXT_PACKAGE).mo; \
	    echo "installing $(srcdir)/$$lang.gmo as" \
		 "$$dir/$(GETTEXT_PACKAGE).mo"; \
	  fi; \
	  if test -r $$lang.gmo.m; then \
	    $(INSTALL_DATA) $$lang.gmo.m $$dir/$(GETTEXT_PACKAGE).mo.m; \
	    echo "installing $$lang.gmo.m as $$dir/$(GETTEXT_PACKAGE).mo.m"; \
	  else \
	    if test -r $(srcdir)/$$lang.gmo.m ; then \
	      $(INSTALL_DATA) $(srcdir)/$$lang.gmo.m \
		$$dir/$(GETTEXT_PACKAGE).mo.m; \
	      echo "installing $(srcdir)/$$lang.gmo.m as" \
		   "$$dir/$(GETTEXT_PACKAGE).mo.m"; \
	    else \
	      true; \
	    fi; \
	  fi; \
	done

# Empty stubs to satisfy archaic automake needs
dvi info ctags tags CTAGS TAGS ID:

# Define this as empty until I found a useful application.
install-exec installcheck:

uninstall:
	linguas="$(USE_LINGUAS)"; \
	for lang in $$linguas; do \
	  rm -f $(DESTDIR)$(localedir)/$$lang/LC_MESSAGES/$(GETTEXT_PACKAGE).mo; \
	  rm -f $(DESTDIR)$(localedir)/$$lang/LC_MESSAGES/$(GETTEXT_PACKAGE).mo.m; \
	done

check: all $(GETTEXT_PACKAGE).pot
	rm -f missing notexist
	srcdir=$(srcdir) $(INTLTOOL_UPDATE) -m
	if [ -r missing -o -r notexist ]; then \
	  exit 1; \
	fi

mostlyclean:
	rm -f *.pox $(GETTEXT_PACKAGE).pot *.old.po cat-id-tbl.tmp
	rm -f .intltool-merge-cache

clean: mostlyclean

distclean: clean
	rm -f Makefile Makefile.in POTFILES stamp-it
	rm -f *.mo *.msg *.cat *.cat.m *.gmo

maintainer-clean: distclean
	@echo "This command is intended for maintainers to use;"
	@echo "it deletes files that may require special tools to rebuild."
	rm -f Makefile.in.in

distdir = ../$(PACKAGE)-$(VERSION)/$(subdir)
dist distdir: $(DISTFILES)
	dists="$(DISTFILES)"; \
	extra_dists="$(EXTRA_DISTFILES)"; \
	for file in $$extra_dists; do \
	  test -f $(srcdir)/$$file && dists="$$dists $(srcdir)/$$file"; \
	done; \
	for file in $$dists; do \
	  test -f $$file || file="$(srcdir)/$$file"; \
	  ln $$file $(distdir) 2> /dev/null \
	    || cp -p $$file $(distdir); \
	done

update-po: Makefile
	$(MAKE) $(GETTEXT_PACKAGE).pot
	tmpdir=`pwd`; \
	linguas="$(USE_LINGUAS)"; \
	for lang in $$linguas; do \
	  echo "$$lang:"; \
	  result="`$(MSGMERGE) -o $$tmpdir/$$lang.new.po $$lang`"; \
	  if $$result; then \
	    if cmp $(srcdir)/$$lang.po $$tmpdir/$$lang.new.po >/dev/null 2>&1; then \
	      rm -f $$tmpdir/$$lang.new.po; \
            else \
	      if mv -f $$tmpdir/$$lang.new.po $$lang.po; then \
	        :; \
	      else \
	        echo "msgmerge for $$lang.po failed: cannot move $$tmpdir/$$lang.new.po to $$lang.po" 1>&2; \
	        rm -f $$tmpdir/$$lang.new.po; \
	        exit 1; \
	      fi; \
	    fi; \
	  else \
	    echo "msgmerge for $$lang.gmo failed!"; \
	    rm -f $$tmpdir/$$lang.new.po; \
	  fi; \
	done

Makefile POTFILES: stamp-it
	@if test ! -f $@; then \
	  rm -f stamp-it; \
	  $(MAKE) stamp-it; \
	fi

stamp-it: Makefile.in.in $(top_builddir)/config.status POTFILES.in
	cd $(top_builddir) \
	  && CONFIG_FILES=$(subdir)/Makefile.in CONFIG_HEADERS= CONFIG_LINKS= \
	       $(SHELL) ./config.status

# Tell versions [3.59,3.63) of GNU make not to export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
