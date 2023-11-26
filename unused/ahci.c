#include "ahci.h"
#include "../kernel/cpu.h"
#include "../kernel/string.h"
#include "../kernel/stdio.h"

#define SATA_SIG_ATAPI             0xEB140101
#define SATA_SIG_SEMB              0xC33C0101
#define SATA_SIG_PM                0x96690101
#define AHCI_DEV_NULL             0
#define AHCI_DEV_SATA             1
#define AHCI_DEV_SATAPI           4
#define AHCI_DEV_SEMB             2
#define AHCI_DEV_PM               3
#define HBA_PORT_DET_PRESENT      3
#define HBA_PORT_IPM_ACTIVE       1
#define HBA_PxIS_TFES              (1 << 30)
#define ATA_CMD_READ_DMA_EX        0x25
#define ATA_CMD_WRITE_DMA_EX       0x35
#define HBA_PxCMD_CR               (1 << 15)
#define HBA_PxCMD_FR               (1 << 14)
#define HBA_PxCMD_FRE              (1 << 4)
#define HBA_PxCMD_ST               (1 << 0)
#define ATA_DEV_BUSY               0x80
#define ATA_DEV_DRQ                0x08

HBA_MEM *glob_abar;
extern int portio_fd;

static void *sendpage(void)
{
	/* FIXME: This is fucking cursed. ~Anton */
	static uint8_t buf[4096 * 128];
	static uint8_t *p = buf;
	uint8_t *n = (uint8_t *)((((uint64_t)p) >> 12) << 12) + 4096;
	p += 4096;
	printf("ALLOC PAGE %016x\n", n);
	return n;
}

/* https://forum.osdev.org/viewtopic.php?f=1&t=40718&start=0 */

void port_rebase(HBA_PORT *port, struct port_data *pdata)
{
	port->cmd &= ~HBA_PxCMD_ST;
	port->cmd &= ~HBA_PxCMD_FRE;
	for(;;)
	{
		if(port->cmd & HBA_PxCMD_FR)
		{
			continue;
		}

		if(port->cmd & HBA_PxCMD_CR)
		{
			continue;
		}

		break;
	}

	void *mapped_clb = sendpage();
	memset(mapped_clb, 0, 4096);
	port->clb = (uint64_t)mapped_clb;
	port->clbu = 0;
	pdata->clb = mapped_clb;

	void *mapped_fb = sendpage();
	memset(mapped_fb, 0, 4096);
	port->fb = (uint64_t)mapped_fb;
	port->fbu = 0;
	pdata->fb = mapped_fb;

	HBA_CMD_HEADER *cmdheader = (HBA_CMD_HEADER *)mapped_clb;
	for(int i = 0; i < 32; ++i)
	{
		cmdheader[i].prdtl = 8;
		void *ctba_buf = sendpage();
		memset(ctba_buf, 0, 4096);
		pdata->ctba[i] = ctba_buf;
		cmdheader[i].ctba = (uint64_t)ctba_buf;
		cmdheader[i].ctbau = 0;
	}

	pdata->port = port;
	while(port->cmd & HBA_PxCMD_CR) {}
	port->cmd |= HBA_PxCMD_FRE;
	port->cmd |= HBA_PxCMD_ST;
}

void probe_port(HBA_MEM *abar, struct port_data *pdtable)
{
	//printf("here!\n");
	glob_abar = abar;
	int i;
	uint32_t pi = abar->pi;
	for(i = 0; i < 32; ++i, pi >>= 1)
	{
			//printf("i = %d!\n", i);
		if(!(pi & 1))
		{
			continue;
		}

			//printf("lol\n");

		HBA_PORT *port = &abar->ports[i];
		int32_t ssts = port->ssts;
		int8_t ipm = (ssts >> 8) & 0x0F;
		int8_t det = ssts & 0x0F;


		if(det != HBA_PORT_DET_PRESENT ||
			ipm != HBA_PORT_IPM_ACTIVE ||
			port->sig != 1)
		{
			continue;
		}

		/* ITS SA(N)TA */
		//printf("found sata device: %d\n", i);
		//port_rebase(abar->ports, &pdtable[i]); /* BUG ??? */
	}

}

uint32_t pciConfigReadWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset)
{
	uint32_t address;
	uint32_t lbus  = bus;
	uint32_t lslot = slot;
	uint32_t lfunc = func;
	uint64_t tmp = 0;

	address = (lbus << 16) | (lslot << 11) |
		(lfunc << 8) | (offset & 0xfc) | ((uint32_t)0x80000000);

	outl(0xCF8, address);
	if(offset == 0x24)
	{
		tmp = inl(0xCFC);
	}
	else
	{
		tmp = (uint16_t)((inl(0xCFC) >> ((offset & 2) * 8)) & 0xffff);
	}

	return tmp;
}

uint64_t checkAllBuses(void)
{
	uint64_t bar5;
	uint16_t vendor_id, device_id;
	int bus, device;

	for(bus = 0; bus < 256; ++bus)
	{
		for(device = 0; device < 32; ++device)
		{
			if((vendor_id = pciConfigReadWord(bus, device, 0, 0)) == 0xFFFF)
			{
				continue;
			}

			device_id = pciConfigReadWord(bus, device, 0, 2);
			printf("PCI [%04X:%04X]\n", vendor_id, device_id);

			if((vendor_id != 0x8086) && (device_id) != 0x2922)
			{
				continue;
			}

			if((bar5 = pciConfigReadWord(bus, device, 0, 0x24)) != 0)
			{
				return bar5;
			}
		}
	}

	return 0;
}

int find_cmdslot(HBA_PORT *port)
{
	uint32_t slots = (port->sact | port->ci);
	int cmdslots = (glob_abar->cap & 0x0f00) >> 8;
	for(int i = 0; i < cmdslots; ++i)
	{
		if((slots & 1) == 0)
		{
			return i;
		}

		slots >>= 1;
	}

	return -1;
}

uint8_t read_sata(struct port_data *pdata, uint32_t startl, uint32_t starth, uint32_t count,char *buf)
{
	pdata->port->is = (uint32_t)-1;
	int spin = 0;
	int slot = find_cmdslot(pdata->port);
	uint64_t buf_phys = (uint64_t)buf;

	if(slot == -1)
	{
		return 0;
	}

	HBA_CMD_HEADER *cmdheader = (HBA_CMD_HEADER *)pdata->clb;
	cmdheader += slot;
	cmdheader->cfl = sizeof(FIS_REG_H2D) / sizeof(uint32_t);
	cmdheader->w = 0;
	cmdheader->prdtl = (uint16_t)((count - 1) >> 4) + 1;

	HBA_CMD_TBL *cmdtbl = (HBA_CMD_TBL *)pdata->ctba[slot];
	int i;
	for(i = 0; i < cmdheader->prdtl - 1; ++i)
	{
		cmdtbl->prdt_entry[i].dba = (uint32_t)(buf_phys & 0xffffffff);
		cmdtbl->prdt_entry[i].dbau = (uint32_t)((buf_phys >> 32) & 0xffffffff);
		cmdtbl->prdt_entry[i].dbc = 8 * 1024;
		cmdtbl->prdt_entry[i].i = 1;
		buf += 4 * 1024;
		count -= 16;
	}

	cmdtbl->prdt_entry[i].dba = (uint32_t) (buf_phys & 0xffffffff);
	cmdtbl->prdt_entry[i].dbau = (uint32_t) ( (buf_phys >> 32) & 0xffffffff);

	cmdtbl->prdt_entry[i].dbc = count << 9;
	cmdtbl->prdt_entry[i].i = 1;

	FIS_REG_H2D *cmdfis = (FIS_REG_H2D *)(&cmdtbl->cfis);

	cmdfis->fis_type = FIS_TYPE_REG_H2D;
	cmdfis->c = 1;
	cmdfis->command = ATA_CMD_READ_DMA_EX;

	cmdfis->lba0 = (uint8_t)startl;
	cmdfis->lba1 = (uint8_t)(startl >> 8);
	cmdfis->lba2 = (uint8_t)(startl >> 16);
	cmdfis->device = 1 << 6;

	cmdfis->lba3 = (uint8_t)(startl >> 24);
	cmdfis->lba4 = (uint8_t)starth;
	cmdfis->lba5 = (uint8_t)(starth >> 8);

	cmdfis->countl = (count & 0xff);
	cmdfis->counth = (count >> 8);

	while((pdata->port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)))
	{
		if(++spin == 1000000)
		{
			return 0;
		}
	}

	pdata->port->ci = (1 << slot);
	for(;;)
	{
		if((pdata->port->ci & (1 << slot)) == 0)
		{
			break;
		}

		if(pdata->port->is & HBA_PxIS_TFES)
		{
			return 0;
		}
	}

	if(pdata->port->is & HBA_PxIS_TFES)
	{
		return 0;
	}

	return 1;
}

#if 0
uint8_t write_sata(struct port_data *pdata, uint32_t startl, uint32_t starth, uint32_t count, char *buf)
{
	pdata->port->is = (uint32_t)-1;
	int spin = 0;
	int slot = find_cmdslot(pdata->port);
	uint64_t buf_phys = (uint64_t)buf;
	HBA_CMD_HEADER* cmdheader = (HBA_CMD_HEADER*)pdata->clb;
	cmdheader += slot;

	cmdheader->cfl = sizeof(FIS_REG_H2D)/sizeof(uint32_t);
	cmdheader->w = 1;
	cmdheader->prdtl = (uint16_t)((count-1)>>4) + 1;

	HBA_CMD_TBL *cmdtbl = (HBA_CMD_TBL*) pdata->ctba[slot];

	memset(cmdtbl, 0, sizeof(HBA_CMD_TBL) +
		(cmdheader->prdtl-1)*sizeof(HBA_PRDT_ENTRY));

	int i;

	for(i=0; i<cmdheader->prdtl-1; i++)
	{
		cmdtbl->prdt_entry[i].dba = (uint32_t) (buf_phys & 0xffffffff);
		cmdtbl->prdt_entry[i].dbau = (uint32_t) ( ( (buf_phys) >> 32) & 0xffffffff);
		cmdtbl->prdt_entry[i].dbc = 8*1024-1;

		buf += 8*1024;
		count -= 16;
	}


	cmdtbl->prdt_entry[i].dba = (uint32_t) (buf_phys & 0xffffffff);
	cmdtbl->prdt_entry[i].dbau = (uint32_t) ( (buf_phys >> 32) & 0xffffffff);


	cmdtbl->prdt_entry[i].dbc = (count<<9)-1;

	FIS_REG_H2D *cmdfis = (FIS_REG_H2D*)(&cmdtbl->cfis);

	cmdfis->fis_type = FIS_TYPE_REG_H2D;
	cmdfis->c = 1;
	cmdfis->command = ATA_CMD_WRITE_DMA_EX;

	cmdfis->lba0 = (uint8_t)startl;
	cmdfis->lba1 = (uint8_t)(startl>>8);
	cmdfis->lba2 = (uint8_t)(startl>>16);
	cmdfis->device = 1<<6;

	cmdfis->lba3 = (uint8_t)(startl>>24);
	cmdfis->lba4 = (uint8_t)starth;
	cmdfis->lba5 = (uint8_t)(starth>>8);

	cmdfis->countl = (count & 0xff);
	cmdfis->counth = (count >> 8);

	while((pdata->port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000)
	{
		spin++;
	}
	if(spin == 1000000)
	{
		return 0;
	}

	pdata->port->ci = (1<<slot);
	while(1)
	{
		if((pdata->port->ci & (1<<slot)) == 0)
			break;
		if(pdata->port->is & HBA_PxIS_TFES)
		{
			return 0;
		}
	}

	if(pdata->port->is & HBA_PxIS_TFES)
	{
		return 0;
	}

	return 1;
}
#endif
