#include "shutdown.h"
#include <stdio.h>
#include "cpu.h"

#define SLP_EN  (1 << 13)
void shutdown(void) // by Napalm and Falkman
{
    unsigned int i, j, len, count, found, *ptr, *rsdp = 0, *rsdt = 0, *facp = 0, *dsdt = 0;
    unsigned char *data, slp_typ[2];

    // find acpi RSDP table pointer
    for (ptr = (unsigned int *) 0x000E0000; ptr < (unsigned int *) 0x000FFFFF; ptr++) {
        if (*ptr == ' DSR' && *(ptr + 1) == ' RTP') { // "RSD PTR "
            rsdp = ptr;
            break;
        }
    }
    if (!rsdp) goto haltonly;

    // find RSDT table pointer
    ptr = (unsigned int *) *(ptr + 4);

    rsdt = ptr;
    count = (*(ptr + 1) - 36) / 4;

    // find FACP table pointer
    ptr += 9; // skip RSDT entries
    for (i = 0; i < count; i++) {
        for (j = 0; j < 24; j++) {
            if (*(unsigned int *) *ptr == 'PCAF') { // "FACP"
                facp = (unsigned int *) *ptr;
                i = count;
                break;
            }
        }
    }
    if (!facp) {
        printk("Error: Could not find FACP table.\n");
        goto haltonly;
    }

    // find DSDT table pointer
    ptr = (unsigned int *) *(facp + 10); // DSDT address
    if (*ptr != 'TDSD') { // "DSDT"
        printk("Error: Could not find DSDT table.\n");
        goto haltonly;
    }

    dsdt = ptr;

    // Search DSDT byte-code for ACPI _S5 signature
    found = 0;
    len = *(dsdt + 1) - 36;
    data = (unsigned char *) (dsdt + 36);
    while (len--) {
        if ((*(unsigned int *) data & 0x00FFFFFF) == 0x0035535F) { // "_S5"
            data += 4;
            if (*data == 0x12) { // 0x012 = package opcode
                data += 3; // 0x0A = 8bit integer opcode
                slp_typ[0] = (*data == 0x0A) ? *++data : *data;
                data++;
                slp_typ[1] = (*data == 0x0A) ? *++data : *data;
                found = 1;
            }
            break;
        }
        data++;
    }
    if (!found) goto haltonly;

    // execute the actual shutdown and power-off
    outw(*(facp + 16), slp_typ[0] | SLP_EN);     // FACP[64] = PM1a_CNT_BLK
    if (*(facp + 17))
        outw(*(facp + 17), slp_typ[1] | SLP_EN); // FACP[68] = PM1b_CNT_BLK
    printk("Shutting down...\n");
    halt();

    haltonly:
    printk("It is now safe to turn-off your computer.\n");
    halt();
}