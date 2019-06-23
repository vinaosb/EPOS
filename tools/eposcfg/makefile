# EPOS Configuration Query Tool Makefile

include	../../makedefs

all: install

eposcfg: eposcfg.cc $(APP)/$(APPLICATION)_traits.h
		$(TCXX) $(TCXXFLAGS) $<
		$(TLD) $(TLDFLAGS) -o $@ eposcfg.o

install: eposcfg
		$(INSTALL) -m 775 eposcfg $(BIN)

clean:
		$(CLEAN) *.o eposcfg
