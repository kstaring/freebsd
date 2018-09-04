/*-
 * SPDX-License-Identifier: BSD-2-Clause-FreeBSD
 *
 * Copyright (c) 2018 K Staring
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY NETAPP, INC ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL NETAPP, INC OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $FreeBSD$
 */

#ifndef	_PCI_HDA_H_
#define	_PCI_HDA_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <assert.h>

#include <sys/types.h>
#include <sys/queue.h>
#include <sys/kernel.h>

#include "hda_reg.h"

extern u_int32_t hda_vendor_id;
extern u_int32_t hda_machine_id;

#define HDA_FIFO_SIZE			0x100

struct hda_softc;
struct hda_codec_class;

struct hda_codec_inst {
	struct hda_codec_class *codec;
	struct hda_softc *hda;
	struct hda_ops *hops;
	uint8_t cad;
	void *priv;
};

struct hda_codec_class {
	char *name;
	int (*init)(struct hda_codec_inst *hci, const char *play, const char *rec, const char *opts);
	int (*reset)(struct hda_codec_inst *hci);
	int (*command)(struct hda_codec_inst *hci, uint32_t cmd_data);
	int (*notify)(struct hda_codec_inst *hci, uint8_t run, uint8_t stream, uint8_t dir);
};

struct hda_ops {
	int (*signal)(struct hda_codec_inst *hci);
	int (*response)(struct hda_codec_inst *hci, uint32_t response, uint8_t unsol);
	int (*transfer)(struct hda_codec_inst *hci, uint8_t stream, uint8_t dir, void *buf, size_t count);
};

#define HDA_EMUL_SET(x)		DATA_SET(hda_codec_class_set, x);

#endif	/* _PCI_HDA_H_ */
