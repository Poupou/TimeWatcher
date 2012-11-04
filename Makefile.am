SUBDIRS= src/

AM_DEFAULT_SOURCE_EXT = .cpp .hh .cc .h .c

distclean-local:
	cd $(top_srcdir) && find . -name ".*.swp[opqr]" -exec rm -f {} \;
	cd $(top_srcdir) && find . -name "*~" -exec rm -f {} \;

all-local:
	ln -sf ./src/timeWatcher timeWatcher

check-local:
	python tests/main.py



