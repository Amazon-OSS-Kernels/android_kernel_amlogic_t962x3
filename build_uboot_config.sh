################################################################################
# 
#  build_kernel_config.sh
#
#  Copyright (c) 2022 Amazon.com, Inc. or its affiliates. All Rights Reserved.
#
################################################################################

UBOOT_SUBPATH="bootable/bootloader/uboot-amlogic/t962x3/bl33"
UBOOT_DEFCONFIG_NAME="primrose"
export CONFIG_SYSTEM_AS_ROOT=systemroot

# Expected image files are seperated with ":"
UBOOT_IMAGES="build/u-boot.bin"

################################################################################
# NOTE: You must fill in the following with the path to a copy of an
#       gcc-linaro-aarch64-none-elf-4.8-2013.11_linux (aarch64-none-elf compiler) and
#       riscv-none-gcc/7.2.0-4-20180606-1631
################################################################################
CROSS_COMPILER_PATH=""
RISCV_COMPILER_PATH=""
