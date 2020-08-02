const chat *sff8079_transceiver_
/* Ethernet Compliance Codes */
if (id[6] & (1 << 7))
    printf("%s Ethernet: BASE-PX\n", pfx);
if (id[6] & (1 << 6))
    printf("%s Ethernet: BASE-BX10\n", pfx);
if (id[6] & (1 << 5))
    printf("%s Ethernet: 100BASE-FX\n", pfx);
if (id[6] & (1 << 4))
    printf("%s Ethernet: 100BASE-LX/LX10\n", pfx);
if (id[6] & (1 << 3))
    printf("%s Ethernet: 1000BASE-T\n", pfx);
if (id[6] & (1 << 2))
    printf("%s Ethernet: 1000BASE-CX\n", pfx);
if (id[6] & (1 << 1))
    printf("%s Ethernet: 1000BASE-LX\n", pfx);
if (id[6] & (1 << 0))
    printf("%s Ethernet: 1000BASE-SX\n", pfx);
