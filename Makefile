#-------------------------------------------------------------------------------
# Local Pokemon Home — devkitA64 / libnx Makefile
#-------------------------------------------------------------------------------
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

APP_TITLE       := Local Pokemon Home
APP_AUTHOR      := Lewis
APP_VERSION     := 0.1.0

CFLAGS   := -g -Wall -O2 -ffunction-sections -fdata-sections \
            -D__SWITCH__ -DAPP_VERSION=\"$(APP_VERSION)\" -fPIE
CFLAGS   += $(INCLUDE)
CXXFLAGS := $(CFLAGS) -std=c++17 -fno-exceptions -fno-rtti
LDFLAGS  := -specs=$(DEVKITPRO)/libnx/switch.specs -g $(ARCH) \
            -Wl,-Map,$(notdir $*.map)

LIBS := -lSDL2 -lEGL -lGLESv2 -lglapi -ldrm_nouveau -lnx

LIBDIRS  := $(PORTLIBS) $(LIBNX)

ifneq ($(BUILD),$(notdir $(CURDIR)))
export OUTPUT := $(CURDIR)/$(TARGET)
export TOPDIR := $(CURDIR)
export VPATH := $(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
                $(foreach dir,$(DATA),$(CURDIR)/$(dir))
export DEPSDIR := $(CURDIR)/$(BUILD)
CFILES   := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES   := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
BINFILES := $(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))
export OFILES_BIN := $(addsuffix .o,$(BINFILES))
export OFILES := $(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o) $(OFILES_BIN)
export HFILES := $(CPPFILES:.cpp=.h) $(CFILES:.c=.h)
export INCLUDE := $(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
                  $(foreach dir,$(LIBDIRS),-I$(dir)/include) \
                  -I$(CURDIR)/$(BUILD)
export LIBPATHS := $(foreach dir,$(LIBDIRS),-L$(dir)/lib)
.PHONY: $(BUILD) clean all
all: $(BUILD)
$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile
clean:
	@rm -fr $(BUILD) $(TARGET).nro $(TARGET).elf $(TARGET).nacp
else
DEPENDS := $(OFILES:.o=.d)
$(OUTPUT).nro : $(OUTPUT).elf $(OUTPUT).nacp
$(OUTPUT).elf : $(OFILES)
	$(CXX) $(LDFLAGS) $^ $(LIBPATHS) $(LIBS) -o $@
%.o : %.cpp
	$(CXX) $(CXXFLAGS) -MMD -MP -MF $(DEPSDIR)/$*.d $(CXXFLAGS) -c $< -o $@
%.o : %.c
	$(CC) $(CFLAGS) -MMD -MP -MF $(DEPSDIR)/$*.d $(CFLAGS) -c $< -o $@
-include $(DEPENDS)
endif
