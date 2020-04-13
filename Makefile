SBN_CLIENT_SRC = ./fsw/src
SBN_CLIENT_INC = -I./fsw/public_inc
SBN_INC = -I../sbn/fsw/public_inc -I../sbn/fsw/platform_inc -I../sbn/fsw/src

# cfe_platform_cfg.h is build specific, and comes from the defs folder.
# Link the platform cfg you want (such as x86-64_platform_cfg.h) as cfe_defs/cfe_platform_cfg.h
# The same goes for cfe_defs/cfe_mission_cfg.h, cfe_defs/cfe_msgids.h, cfe_defs/cfe_perfids.h
CFE_DEFS = -I./cfe_defs

CFE_INC = -I../../cfe/fsw/cfe-core/src/inc
OSAL_INC = -I../../osal/src/os/inc
OSAL_BSP_INC = -I../../osal/src/bsp/pc-linux/config
PSP_INC = -I../../psp/fsw/inc
PSP_BSP_INC = -I../../psp/fsw/pc-linux/inc

LIBS = -lpthread

SC_OBJS := sbn_client.a
SC_OBJS += sbn_client_ingest.a
SC_OBJS += sbn_client_init.a
SC_OBJS += sbn_client_minders.a
SC_OBJS += sbn_client_utils.a
SC_OBJS += sbn_client_wrappers.a

A_FILES := $(patsubst %.c,%.a,$(wildcard $(SBN_CLIENT_SRC)/*.c))

all: libsbn_client.so

libsbn_client.so: $(A_FILES)
	gcc -shared $^ -o libsbn_client.so

%.a : %.c
	gcc -Wall -Werror -c -fPIC $< $(SBN_CLIENT_INC) $(CFE_DEFS) $(CFE_INC) $(OSAL_INC) $(OSAL_BSP_INC) $(PSP_INC) $(PSP_BSP_INC) $(SBN_INC) $(LIBS) -o $@
	objcopy --redefine-syms=unwrap_symbols.txt $@

clean:
	rm -f $(A_FILES)
	rm -f libsbn_client.so
