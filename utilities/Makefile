SUBDIRS := $(wildcard */.)

all: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

install: $(SUBDIRS)
	$(MAKE) -C $@ install

.PHONY: all $(SUBDIRS) install

