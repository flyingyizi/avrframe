# Default target: make program!
# Optional linker flags.
#  -Wl,...:   tell GCC to pass this to linker.
#  -Map:      create map file
#  --cref:    add cross reference to  map file
LDFLAGS = -Wl,-Map=$(ROOT_DIR)/$(BIN_DIR)/$(BIN).map,--cref

OBJS=*.o
ODIR=obj
$(ROOT_DIR)/$(BIN_DIR)/$(BIN).elf:$(ODIR)/$(OBJS)
	$(CC)  $(ALL_CFLAGS)   $(LDFLAGS)  $^  -o $@
	$(OBJCOPY) -O $(FORMAT) -R .eeprom $@ $(ROOT_DIR)/$(BIN_DIR)/$(BIN).hex 

