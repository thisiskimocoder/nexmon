NEXMON_CHIP=CHIP_VER_BCM43436b0
NEXMON_CHIP_NUM=`$(NEXMON_ROOT)/buildtools/scripts/getdefine.sh $(NEXMON_CHIP)`
NEXMON_FW_VERSION=FW_VER_9_88_0_0
NEXMON_FW_VERSION_NUM=`$(NEXMON_ROOT)/buildtools/scripts/getdefine.sh $(NEXMON_FW_VERSION)`

NEXMON_ARCH=armv7-m

RAM_FILE=fw_bcm43436b0.bin
RAMSTART=0x0
RAMSIZE=0x80000

ROM_FILE=rom.bin
ROMSTART=0x800000
ROMSIZE=0x915AC

WLC_UCODE_WRITE_BL_HOOK_ADDR=0x4E818
HNDRTE_RECLAIM_0_END_PTR=0x2F0C
HNDRTE_RECLAIM_0_END=0x66D00

PATCHSIZE=0x1000
PATCHSTART=$$(($(HNDRTE_RECLAIM_0_END) - $(PATCHSIZE)))

# original ucode start and size
UCODESTART=0x5AC3C
UCODESTART_PTR=0x4E828
UCODESIZE=0xBA91
UCODESIZE_PTR=0x4E824

# original template ram start and size
TEMPLATERAMSTART_PTR=0x5AAC4
TEMPLATERAMSTART=0x666D0
TEMPLATERAMSIZE=0x630

FP_DATA_END_PTR=0x42474
FP_CONFIG_BASE_PTR_1=0x418D0
FP_CONFIG_END_PTR_1=0x418D4
FP_CONFIG_BASE_PTR_2=0x41A14
FP_CONFIG_END_PTR_2=0x41A18
# can start at the end of the firmware, it will be overwritten after it is read
FP_CONFIG_BASE=0x66E80
FP_DATA_BASE=0x1000
FP_CONFIG_SIZE=0xC00
FP_CONFIG_ORIGBASE=0x1800
FP_CONFIG_ORIGEND=0x1AF8