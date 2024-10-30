# Define compiler based on OS
ifeq ($(HOSTUNAME), Darwin)
    CC := gcc-mp-5
    LDFLAGS := -L/opt/local/lib -largp
else
    CC := gcc
endif

# General CFLAGS
CFLAGS := -std=c99 -Wall -Wno-unused-result -O0 -D_BSD_SOURCE

# Target
ucodeext: ucodeext.o
	$(CC) $(LDFLAGS) -o $@ $^

# Object file rule
ucodeext.o: ucodeext.c
	$(CC) $(CFLAGS) -o $@ -c $<

# Clean rule
clean:
	rm -f *.o ucodeext

# Force target to ensure clean works as expected
.PHONY: clean
