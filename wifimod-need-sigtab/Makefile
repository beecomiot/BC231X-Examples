#############################################################
# Required variables for each makefile
# Discard this section from all parent makefiles
# Expected variables (with automatic defaults):
#   CSRCS (all "C" files in the dir)
#   SUBDIRS (all subdirs with a Makefile)
#   GEN_LIBS - list of libs to be generated ()
#   GEN_IMAGES - list of object file images to be generated ()
#   GEN_BINS - list of binaries to be generated ()
#   COMPONENTS_xxx - a list of libs/objs in the form
#     subdir/lib to be extracted and rolled up into
#     a generated lib/image xxx.a ()
#
TARGET = eagle
#FLAVOR = release
FLAVOR = debug

#EXTRA_CCFLAGS += -u

ifndef PDIR # {
GEN_IMAGES= eagle.app.v6.out
GEN_BINS= eagle.app.v6.bin
SPECIAL_MKTARGETS=$(APP_MKTARGETS)
SUBDIRS=    \
	user    \
	mydriver

endif # } PDIR

LDDIR = $(SDK_PATH)/ld
MYLIBS_DIR = mylibs

CCFLAGS += -Os

TARGET_LDFLAGS =		\
	-nostdlib		\
	-Wl,-EL \
	--longcalls \
	--text-section-literals

ifeq ($(FLAVOR),debug)
    TARGET_LDFLAGS += -g -O2
endif

ifeq ($(FLAVOR),release)
    TARGET_LDFLAGS += -g -O0
endif

COMPONENTS_eagle.app.v6 = \
	user/libuser.a  \
	mydriver/libmydriver.a

LINKFLAGS_eagle.app.v6 = \
	-L$(SDK_PATH)/lib        \
	-Wl,--gc-sections   \
	-nostdlib	\
    -T$(LD_FILE)   \
	-Wl,--no-check-sections	\
    -u call_user_start	\
	-Wl,-static						\
	-Wl,--start-group					\
	-lcirom \
	-lcrypto	\
	-lespconn	\
	-lespnow	\
	-lfreertos	\
	-lgcc					\
	-lhal					\
	-ljson	\
	-llwip	\
	-lmain	\
	-lmirom	\
	-lnet80211	\
	-lnopoll	\
	-lphy	\
	-lpp	\
	-lpwm	\
	-lsmartconfig	\
	-lspiffs	\
	-lssl	\
	-lwpa	\
	-lwps		\
	$(MYLIBS_DIR)/libbpclient.a   \
	$(MYLIBS_DIR)/libbps.a   \
	$(DEP_LIBS_eagle.app.v6)					\
	-Wl,--end-group 	

DEPENDS_eagle.app.v6 = \
                $(LD_FILE) \
                $(LDDIR)/eagle.rom.addr.v6.ld

#############################################################
# Configuration i.e. compile options etc.
# Target specific stuff (defines etc.) goes in here!
# Generally values applying to a tree are captured in the
#   makefile at its root level - these are then overridden
#   for a subtree within the makefile rooted therein
#

#UNIVERSAL_TARGET_DEFINES =		\

# Other potential configuration flags include:
#	-DTXRX_TXBUF_DEBUG
#	-DTXRX_RXBUF_DEBUG
#	-DWLAN_CONFIG_CCX
CONFIGURATION_DEFINES =	-DICACHE_FLASH

# DEFINES +=				\
# 	$(UNIVERSAL_TARGET_DEFINES)	\
# 	$(CONFIGURATION_DEFINES)

# DEFINES += -DDEBUG 
DEFINES += -DBPS_CPU32 
DEFINES += -DBP_CPU32 
DEFINES += -DBP_USE_STD
DEFINES += -DBPS_USE_STD

DEFINES += $(UNIVERSAL_TARGET_DEFINES)
DEFINES += $(CONFIGURATION_DEFINES)    



#############################################################
# Recursion Magic - Don't touch this!!
#
# Each subtree potentially has an include directory
#   corresponding to the common APIs applicable to modules
#   rooted at that subtree. Accordingly, the INCLUDE PATH
#   of a module can only contain the include directories up
#   its parent path, and not its siblings
#
# Required for each makefile to inherit from the parent
#

INCLUDES := $(INCLUDES) -I $(PDIR)include
INCLUDES := $(INCLUDES) -I $(PDIR)inc
INCLUDES := $(INCLUDES) -I $(PDIR)sig_table
INCLUDES := $(INCLUDES) -I $(PDIR)mydriver
INCLUDES := $(INCLUDES) -I $(PDIR)libbps_include
sinclude $(SDK_PATH)/Makefile

.PHONY: FORCE
FORCE:

