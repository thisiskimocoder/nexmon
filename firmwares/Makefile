# Automatically find all subdirectories with a Makefile
SUBDIRS := $(dir $(wildcard */Makefile))

# Default target to build all subdirectories
all: $(SUBDIRS)

# Rule to process each subdirectory
$(SUBDIRS): FORCE
	@printf "\033[0;31m  EXECUTING MAKE FOR CHIP VERSION\033[0m %s\n" $@
	$(Q)$(MAKE) -C $@

# Declare FORCE as a phony target to prevent conflicts
.PHONY: FORCE
FORCE:
