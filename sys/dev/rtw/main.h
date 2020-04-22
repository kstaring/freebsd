/* SPDX-License-Identifier: GPL-2.0 OR BSD-3-Clause */
/* Copyright(c) 2018-2019  Realtek Corporation
 */

#ifndef __RTW_MAIN_H
#define __RTW_MAIN_H

#include <asm/types.h>

#define RTW_MAX_MAC_ID_NUM		32
#define RTW_MAX_SEC_CAM_NUM		32
#define MAX_PG_CAM_BACKUP_NUM		8

#define RFREG_MASK			0xfffff
#define INV_RF_DATA			0xffffffff
#define TX_PAGE_SIZE_SHIFT		7

#define RTW_CHANNEL_WIDTH_MAX		3
#define RTW_RF_PATH_MAX			4
#define HW_FEATURE_LEN			13

#define RTW_TP_SHIFT			18 /* bytes/2s --> Mbps */

struct rtw_dev {
};

struct rtw_sta_info {
};

#endif // __RTW_MAIN_H
