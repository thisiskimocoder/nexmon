NEXMON_CHIP=CHIP_VER_BCM4389c1
NEXMON_CHIP_NUM=`$(NEXMON_ROOT)/buildtools/scripts/getdefine.sh $(NEXMON_CHIP)`
NEXMON_FW_VERSION=FW_VER_20_101_57_r1035009
NEXMON_FW_VERSION_NUM=`$(NEXMON_ROOT)/buildtools/scripts/getdefine.sh $(NEXMON_FW_VERSION)`

NEXMON_ARCH=armv7-a

RAM_FILE=fw_bcmdhd.bin
RAMSTART=0x00200000
RAMSIZE=0x002A0000

ROM_FILE=rom.bin
ROMSTART=0x00001000
ROMSIZE=0x001DA000

# ucode start and size
UCODE0START=0x3724E4
UCODE0SIZE=0x157B0
UCODE0START_PTR=0x350330
UCODE0SIZE_PTR=0x35032C

UCODE1START=0x387C98
UCODE1SIZE=0x146B8
UCODE1START_PTR=0x350328
UCODE1SIZE_PTR=0x350324

UCODE2START=0x39C354
UCODE2SIZE=0x4BC8
UCODE2START_PTR=0x350338
UCODE2SIZE_PTR=0x350334

HNDRTE_RECLAIM_3_END_PTR=0x275058
HNDRTE_RECLAIM_3_ORIG_END=0x3A8CD8

WLC_UCODE_WRITE_BL_HOOK_ADDR=0x3502DA

# safe restore
TEMPLATERAM0START_PTR=0x372360
TEMPLATERAM0START=0x3A0F20
TEMPLATERAM0SIZE=0x1AC4

TEMPLATERAM1START_PTR=0x372368
TEMPLATERAM1START=0x3A29E4
TEMPLATERAM1SIZE=0x2DCC

TEMPLATERAM2START_PTR=0x37235C
TEMPLATERAM2START=0x3A57B0
TEMPLATERAM2SIZE=0x22C4

TEMPLATERAM3START_PTR=0x372364
TEMPLATERAM3START=0x3A7A74
TEMPLATERAM3SIZE=0x1264

# rom patches
FP_DATA_BASE=0x208000
FP_DATA_END=0x20F800
FP_DATA_LAST_PTR=0x32EEAC
FP_CONFIG_ORIGBASE=0x3A8CD8
FP_CONFIG_ORIGEND=0x3AA7A0
# hnd_tcam_load_default
FP_CONFIG_BASE_PTR_1=0x32F03C
FP_CONFIG_END_PTR_1=0x32F038
 # hnd_tcam_verify
FP_CONFIG_BASE_PTR_2=0x32EE5C
FP_CONFIG_END_PTR_2=0x32EE60
# hnd_tcam_stat
FP_CONFIG_BASE_PTR_3=0x32EF98
FP_CONFIG_END_PTR_3=0x32EF9C
# hnd_tcam_reclaim
FP_CONFIG_BASE_PTR_4=0x348794
FP_CONFIG_END_PTR_4=0x348798

N_EXTRA_FP=10
FP_CONFIG_SIZE=$$(($(FP_CONFIG_ORIGEND) - $(FP_CONFIG_ORIGBASE) + 8 * $(N_EXTRA_FP)))
FP_CONFIG_BASE=$$(($(FP_CONFIG_ORIGBASE) - 8 * $(N_EXTRA_FP)))

PATCHSIZE=0x5000
PATCHSTART=$$(($(FP_CONFIG_BASE) - $(PATCHSIZE)))

VERSION_PTR_1=0x27ED34
VERSION_PTR_2=0x348C88
VERSION_PTR_3=0x34A77C
VERSION_PTR_4=0x346D38
DATETIME_PTR=0x27ED38
