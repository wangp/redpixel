# guibrella-ish rules for AGUP

AGUP_OBJS := agup.o agtk.o awin95.o aase.o
AGUP_LIB := agup.a

agup_lib: $(AGUP_LIB)

$(AGUP_LIB): $(AGUP_OBJS)
	$(AR) $(ARFLAGS) $@ $^
