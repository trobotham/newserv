include ../build.mk

CFLAGS+=$(INCDBAPI)
LDFLAGS+=$(LIBDBAPI)

.PHONY: all
all: proxyscan.so proxyscan_newsearch.so

proxyscan.so: proxyscan.o proxyscanext.o proxyscanalloc.o proxyscanconnect.o proxyscancache.o proxyscanqueue.o proxyscanhandlers.o proxyscandb.o

proxyscan_newsearch.so: proxyscan_newsearch.o pns-scan.o
