LDSCRIPT := $(SRCTREE)/board/$(VENDOR)/$(BOARD)/u-boot.lds

sinclude $(OBJTREE)/board/$(VENDOR)/$(BOARD)/config.tmp

PLATFORM_CPPFLAGS += -D__UBOOT__ -D_MX6Q_

ifndef TEXT_BASE
	TEXT_BASE = 0x27800000
endif
