#ifndef _PCI_H_
#define _PCI_H_

#define PROC_BUS_PCI "/proc/bus/pci"
#define SYS_BUS_PCI "/sys/bus/pci"
#define PCIID_PATH DATADIR"/pci.ids:/usr/share/lshw/pci.ids:/usr/local/share/pci.ids:/usr/share/pci.ids:/etc/pci.ids:/usr/share/hwdata/pci.ids:/usr/share/misc/pci.ids"
//#define PCIID_PATH DATADIR"/pxi.ids"
#define PCI_CLASS_REVISION      0x08              /* High 24 bits are class, low 8 revision */
#define PCI_VENDOR_ID           0x00              /* 16 bits */
#define PCI_DEVICE_ID           0x02              /* 16 bits */
#define PCI_COMMAND             0x04              /* 16 bits */
#define PCI_REVISION_ID         0x08              /* Revision ID */
#define PCI_CLASS_PROG          0x09              /* Reg. Level Programming Interface */
#define PCI_CLASS_DEVICE        0x0a              /* Device class */
#define PCI_HEADER_TYPE         0x0e              /* 8 bits */
#define PCI_HEADER_TYPE_NORMAL     0
#define PCI_HEADER_TYPE_BRIDGE     1
#define PCI_HEADER_TYPE_CARDBUS    2
#define PCI_PRIMARY_BUS         0x18              /* Primary bus number */
#define PCI_SECONDARY_BUS       0x19              /* Secondary bus number */
#define PCI_STATUS              0x06              /* 16 bits */
#define PCI_LATENCY_TIMER       0x0d              /* 8 bits */
#define PCI_SEC_LATENCY_TIMER   0x1b              /* Latency timer for secondary interface */
#define PCI_CB_LATENCY_TIMER    0x1b              /* CardBus latency timer */
#define PCI_STATUS_66MHZ        0x20              /* Support 66 Mhz PCI 2.1 bus */
#define PCI_STATUS_CAP_LIST     0x10              /* Support Capability List */
#define PCI_COMMAND_IO          0x01              /* Enable response in I/O space */
#define PCI_COMMAND_MEMORY      0x02              /* Enable response in Memory space */
#define PCI_COMMAND_MASTER      0x04              /* Enable bus mastering */
#define PCI_COMMAND_SPECIAL     0x08              /* Enable response to special cycles */
#define PCI_COMMAND_INVALIDATE  0x10              /* Use memory write and invalidate */
#define PCI_COMMAND_VGA_PALETTE 0x20              /* Enable palette snooping */
#define PCI_COMMAND_PARITY      0x40              /* Enable parity checking */
#define PCI_COMMAND_WAIT        0x80              /* Enable address/data stepping */
#define PCI_COMMAND_SERR        0x100             /* Enable SERR */
#define PCI_COMMAND_FAST_BACK   0x200             /* Enable back-to-back writes */

#define PCI_MIN_GNT             0x3e              /* 8 bits */
#define PCI_MAX_LAT             0x3f              /* 8 bits */

#define PCI_CAPABILITY_LIST     0x34              /* Offset of first capability list entry */
#define PCI_CAP_LIST_ID            0              /* Capability ID */
#define PCI_CAP_ID_PM           0x01              /* Power Management */
#define PCI_CAP_ID_AGP          0x02              /* Accelerated Graphics Port */
#define PCI_CAP_ID_VPD          0x03              /* Vital Product Data */
#define PCI_CAP_ID_SLOTID       0x04              /* Slot Identification */
#define PCI_CAP_ID_MSI          0x05              /* Message Signalled Interrupts */
#define PCI_CAP_ID_CHSWP        0x06              /* CompactPCI HotSwap */
#define PCI_CAP_ID_PCIX         0x07              /* PCI-X */
#define PCI_CAP_ID_HT           0x08              /* HyperTransport */
#define PCI_CAP_ID_VNDR         0x09              /* Vendor specific */
#define PCI_CAP_ID_DBG          0x0A              /* Debug port */
#define PCI_CAP_ID_CCRC         0x0B              /* CompactPCI Central Resource Control */
#define PCI_CAP_ID_AGP3         0x0E              /* AGP 8x */
#define PCI_CAP_ID_EXP          0x10              /* PCI Express */
#define PCI_CAP_ID_MSIX         0x11              /* MSI-X */
#define PCI_CAP_LIST_NEXT          1              /* Next capability in the list */
#define PCI_CAP_FLAGS              2              /* Capability defined flags (16 bits) */
#define PCI_CAP_SIZEOF             4
#define PCI_FIND_CAP_TTL          48

#define PCI_SID_ESR                2              /* Expansion Slot Register */
#define PCI_SID_ESR_NSLOTS      0x1f              /* Number of expansion slots available */


/* Device classes and subclasses */

#define PCI_CLASS_NOT_DEFINED        0x0000
#define PCI_CLASS_NOT_DEFINED_VGA    0x0001

#define PCI_BASE_CLASS_STORAGE       0x01
#define PCI_CLASS_STORAGE_SCSI       0x0100
#define PCI_CLASS_STORAGE_IDE        0x0101
#define PCI_CLASS_STORAGE_FLOPPY     0x0102
#define PCI_CLASS_STORAGE_IPI        0x0103
#define PCI_CLASS_STORAGE_RAID       0x0104
#define PCI_CLASS_STORAGE_SATA       0x0106
#define PCI_CLASS_STORAGE_SAS        0x0107
#define PCI_CLASS_STORAGE_OTHER      0x0180

#define PCI_BASE_CLASS_NETWORK       0x02
#define PCI_CLASS_NETWORK_ETHERNET   0x0200
#define PCI_CLASS_NETWORK_TOKEN_RING 0x0201
#define PCI_CLASS_NETWORK_FDDI       0x0202
#define PCI_CLASS_NETWORK_ATM        0x0203
#define PCI_CLASS_NETWORK_OTHER      0x0280

#define PCI_BASE_CLASS_DISPLAY       0x03
#define PCI_CLASS_DISPLAY_VGA        0x0300
#define PCI_CLASS_DISPLAY_XGA        0x0301
#define PCI_CLASS_DISPLAY_OTHER      0x0380
#define PCI_CLASS_3D_CONTROLLER      0x0302

// 3D controller [0302]: NVIDIA Corporation GP108M [GeForce MX250] [10de:1d13] (rev a1)


#define PCI_BASE_CLASS_MULTIMEDIA    0x04
#define PCI_CLASS_MULTIMEDIA_VIDEO   0x0400
#define PCI_CLASS_MULTIMEDIA_AUDIO   0x0401
#define PCI_CLASS_MULTIMEDIA_OTHER   0x0480

#define PCI_BASE_CLASS_MEMORY        0x05
#define PCI_CLASS_MEMORY_RAM         0x0500
#define PCI_CLASS_MEMORY_FLASH       0x0501
#define PCI_CLASS_MEMORY_OTHER       0x0580

#define PCI_BASE_CLASS_BRIDGE        0x06
#define PCI_CLASS_BRIDGE_HOST        0x0600
#define PCI_CLASS_BRIDGE_ISA         0x0601
#define PCI_CLASS_BRIDGE_EISA        0x0602
#define PCI_CLASS_BRIDGE_MC          0x0603
#define PCI_CLASS_BRIDGE_PCI         0x0604
#define PCI_CLASS_BRIDGE_PCMCIA      0x0605
#define PCI_CLASS_BRIDGE_NUBUS       0x0606
#define PCI_CLASS_BRIDGE_CARDBUS     0x0607
#define PCI_CLASS_BRIDGE_OTHER       0x0680

#define PCI_BASE_CLASS_COMMUNICATION     0x07
#define PCI_CLASS_COMMUNICATION_SERIAL   0x0700
#define PCI_CLASS_COMMUNICATION_PARALLEL 0x0701
#define PCI_CLASS_COMMUNICATION_MODEM    0x0703
#define PCI_CLASS_COMMUNICATION_OTHER    0x0780

#define PCI_BASE_CLASS_SYSTEM        0x08
#define PCI_CLASS_SYSTEM_PIC         0x0800
#define PCI_CLASS_SYSTEM_DMA         0x0801
#define PCI_CLASS_SYSTEM_TIMER       0x0802
#define PCI_CLASS_SYSTEM_RTC         0x0803
#define PCI_CLASS_SYSTEM_OTHER       0x0880

#define PCI_BASE_CLASS_INPUT         0x09
#define PCI_CLASS_INPUT_KEYBOARD     0x0900
#define PCI_CLASS_INPUT_PEN          0x0901
#define PCI_CLASS_INPUT_MOUSE        0x0902
#define PCI_CLASS_INPUT_OTHER        0x0980

#define PCI_BASE_CLASS_DOCKING       0x0a
#define PCI_CLASS_DOCKING_GENERIC    0x0a00
#define PCI_CLASS_DOCKING_OTHER      0x0a01

#define PCI_BASE_CLASS_PROCESSOR     0x0b
#define PCI_CLASS_PROCESSOR_386      0x0b00
#define PCI_CLASS_PROCESSOR_486      0x0b01
#define PCI_CLASS_PROCESSOR_PENTIUM  0x0b02
#define PCI_CLASS_PROCESSOR_ALPHA    0x0b10
#define PCI_CLASS_PROCESSOR_POWERPC  0x0b20
#define PCI_CLASS_PROCESSOR_CO       0x0b40

#define PCI_BASE_CLASS_SERIAL        0x0c
#define PCI_CLASS_SERIAL_FIREWIRE    0x0c00
#define PCI_CLASS_SERIAL_ACCESS      0x0c01
#define PCI_CLASS_SERIAL_SSA         0x0c02
#define PCI_CLASS_SERIAL_USB         0x0c03
#define PCI_CLASS_SERIAL_FIBER       0x0c04

#define PCI_CLASS_OTHERS             0xff

#define PCI_ADDR_MEM_MASK (~(pciaddr_t) 0xf)
#define PCI_BASE_ADDRESS_0              0x10              /* 32 bits */
#define PCI_BASE_ADDRESS_SPACE          0x01              /* 0 = memory, 1 = I/O */
#define PCI_BASE_ADDRESS_SPACE_IO       0x01
#define PCI_BASE_ADDRESS_SPACE_MEMORY   0x00
#define PCI_BASE_ADDRESS_MEM_TYPE_MASK  0x06
#define PCI_BASE_ADDRESS_MEM_TYPE_32    0x00              /* 32 bit address */
#define PCI_BASE_ADDRESS_MEM_TYPE_1M    0x02              /* Below 1M [obsolete] */
#define PCI_BASE_ADDRESS_MEM_TYPE_64    0x04              /* 64 bit address */
#define PCI_BASE_ADDRESS_MEM_PREFETCH   0x08              /* prefetchable? */
#define PCI_BASE_ADDRESS_MEM_MASK       (~0x0fUL)
#define PCI_BASE_ADDRESS_IO_MASK        (~0x03UL)

#define PCI_SUBSYSTEM_VENDOR_ID      0x2c
#define PCI_SUBSYSTEM_ID             0x2e

#define PCI_CB_SUBSYSTEM_VENDOR_ID   0x40
#define PCI_CB_SUBSYSTEM_ID          0x42

#define PCI_VENDOR_ID_INTEL		0x8086
#define PCI_VENDOR_ID_AMD		0x1002
#define PCI_VENDOR_ID_JJW		0x0731
#define PCI_VENDOR_ID_NVIDIA	0x10DE
#define PCI_VENDOR_ID_ZX        0x1d17   //"zx", zx_chip_ids
#define PCI_VENDOR_ID_LONGSOON  0x0014

#endif
