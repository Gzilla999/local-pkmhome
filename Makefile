#---------------------------------------------------------------------------------
# local-pkmhome — devkitA64 / libnx
#---------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITPRO)),)
$(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>/devkitpro")
endif

TOPDIR ?= $(CURDIR)
include $(DEVKITPRO)/libnx/switch_rules

TARGET      := local-pkmhome
BUILD       := build
SOURCES     := src src/platform src/storage src/saves src/pokemon src/ui
DATA        := assets
INCLUDES    := include

APP_TITLE   := Local Pokemon Home
APP_AUTHOR  := Lewis
APP_VERSION := 0.1.0

LIBS        := -lSDL2 -lEGL -lglapi -ldrm_nouveau -lnx
LIBDIRS     := $(PORTLIBS) $(LIBNX)
