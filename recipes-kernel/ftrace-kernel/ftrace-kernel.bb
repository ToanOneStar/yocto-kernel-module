SUMMARY = "A simple GPIO kernel module"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = "file://gpio-ftrace-demo.c \
           file://Makefile"

S = "${WORKDIR}"

inherit module

EXTRA_OEMAKE:append:class-target = " KERNEL_SRC=${STAGING_KERNEL_DIR}"
KERNEL_MODULE_PROBE = "gpio-ftrace-demo"