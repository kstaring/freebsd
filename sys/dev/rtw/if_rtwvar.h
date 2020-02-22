struct rtw_softc {
	char *fwname;
	const struct firmware	*fw_fp;

	int			memrid;
	struct resource		*mem;
	int			irqrid;
	struct resource		*irq;
};
