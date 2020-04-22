/**
 * Copyright (C) 2019 by K Staring <qdk@quickdekay.net>
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN
 * AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 * 
 */
#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/param.h>
#include <sys/sysctl.h>
#include <sys/sockio.h>
#include <sys/mbuf.h>
#include <sys/kernel.h>
#include <sys/socket.h>
#include <sys/systm.h>
#include <sys/malloc.h>
#include <sys/lock.h>
#include <sys/mutex.h>
#include <sys/module.h>
#include <sys/bus.h>
#include <sys/endian.h>
#include <sys/proc.h>
#include <sys/mount.h>
#include <sys/namei.h>
#include <sys/linker.h>
#include <sys/firmware.h>
#include <sys/taskqueue.h>

#include <machine/bus.h>
#include <machine/resource.h>
#include <sys/rman.h>

#include <dev/pci/pcireg.h>
#include <dev/pci/pcivar.h>

#include <net/bpf.h>
#include <net/if.h>
#include <net/if_var.h>
#include <net/if_arp.h>
#include <net/ethernet.h>
#include <net/if_dl.h>
#include <net/if_media.h>
#include <net/if_types.h>

#include <net80211/ieee80211_var.h>
#include <net80211/ieee80211_radiotap.h>
#include <net80211/ieee80211_input.h>
#include <net80211/ieee80211_regdomain.h>

#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/in_var.h>
#include <netinet/ip.h>
#include <netinet/if_ether.h>

#include <dev/rtw/if_rtwreg.h>
#include <dev/rtw/if_rtwvar.h>
#include <dev/rtw/if_rtw_ioctl.h>

MODULE_DEPEND(rtw, pci,  1, 1, 1);
MODULE_DEPEND(rtw, wlan, 1, 1, 1);
MODULE_DEPEND(rtw, firmware, 1, 1, 1);

static int rtw_probe(device_t);
static int rtw_attach(device_t);
static int rtw_detach(device_t);
static int rtw_shutdown(device_t);
static int rtw_suspend(device_t);
static int rtw_resume(device_t);

static device_method_t rtw_methods[] = {
	/* Device interface */
	DEVMETHOD(device_probe,		rtw_probe),
	DEVMETHOD(device_attach,	rtw_attach),
	DEVMETHOD(device_detach,	rtw_detach),
	DEVMETHOD(device_shutdown,	rtw_shutdown),
	DEVMETHOD(device_suspend,	rtw_suspend),
	DEVMETHOD(device_resume,	rtw_resume),

	DEVMETHOD_END
};

static driver_t rtw_driver = {
	"rtw",
	rtw_methods,
	sizeof (struct rtw_softc)
};

static devclass_t rtw_devclass;

DRIVER_MODULE(rtw, pci, rtw_driver, rtw_devclass, NULL, NULL);

MODULE_VERSION(rtw, 1);

static int
rtw_probe(device_t dev)
{
	char *nic_type;

	if (pci_get_vendor(dev) != 0x10ec)
		return (ENXIO);

	if (pci_get_device(dev) == 0xb822)
		nic_type = "Realtek 8822B 802.11ac PCI NIC";
	if (pci_get_device(dev) == 0xc822)
		nic_type = "Realtek 8822C 802.11ac PCI NIC";

	if (nic_type == NULL)
		return (ENXIO);

	device_set_desc(dev, nic_type);

	return (BUS_PROBE_DEFAULT);
}

static int
rtw_attach(device_t dev)
{
	int res = 0;
	struct rtw_softc *sc = device_get_softc(dev);

	switch (pci_get_device(dev)) {
	case 0xb822:
		sc->fwname = "rtw8822b_fw";
		break;
	case 0xc822:
		sc->fwname = "rtw8822c_fw";
		break;
	default:
		device_printf(dev, "rtw_attach: illegal device id: 0x%04x\n",
				pci_get_device(dev));
		return (ENXIO);
	}

	sc->fw_fp = firmware_get(sc->fwname);

	pci_enable_io(dev, SYS_RES_IOPORT);
	pci_enable_io(dev, SYS_RES_MEMORY);
        pci_enable_busmaster(dev);

	// rtw_pci_setup_resource
	// rtw_pci_io_mapping
        sc->memrid = PCIR_BAR(2);
        sc->mem = bus_alloc_resource_any(dev, SYS_RES_MEMORY, &sc->memrid,
		RF_ACTIVE);
        if (sc->mem == NULL) {
                device_printf(dev, "could not allocate memory resource\n");
		return (ENXIO);
        }

        sc->irqrid = 0;
        sc->irq = bus_alloc_resource_any(dev, SYS_RES_IRQ, &sc->irqrid,
            RF_ACTIVE | RF_SHAREABLE);
        if (sc->irq == NULL) {
                device_printf(dev, "could not allocate interrupt resource\n");
		return (ENXIO);
        }

	// rtw_chip_info_setup
	// rtw_chip_parameter_setup
uint8_t qb = *(uint8_t *)(sc->mem + 0x0100); 
uint16_t qw = *(uint16_t *)(sc->mem + 0x0080); 
printf("->qb=0x%02hhx qw=0x%04hx\n", qb, qw);

	// rtw_chip_efuse_info_setup
	// rtw_mac_power_on


	return (res);
}

static int
rtw_detach(device_t dev)
{
	struct rtw_softc *sc = device_get_softc(dev);

	if (sc->irq)
		bus_release_resource(dev, SYS_RES_IRQ, sc->irqrid, sc->irq);
	if (sc->mem)
		bus_release_resource(dev, SYS_RES_MEMORY, sc->memrid, sc->mem);

	return (0);
}

static int
rtw_shutdown(device_t dev)
{
//	struct rtw_softc *sc = device_get_softc(dev);

	return 0;
}

static int
rtw_suspend(device_t dev)
{
//	struct rtw_softc *sc = device_get_softc(dev);

	return 0;
}

static int
rtw_resume(device_t dev)
{
//	struct rtw_softc *sc = device_get_softc(dev);

	return 0;
}
