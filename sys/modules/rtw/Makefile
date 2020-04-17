# $FreeBSD$

.PATH: ${SRCTOP}/sys/dev/rtw

CFLAGS += -I${SRCTOP}/sys/compat/linuxkpi/common/include
KMOD    = if_rtw
SRCS    = if_rtw.c
#SRCS   += rtw8822b_table.c rtw8822b_table.h rtw8822c_table.c rtw8822c_table.c
SRCS   += device_if.h bus_if.h pci_if.h

.include <bsd.kmod.mk>
