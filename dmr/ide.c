/* Important bits in the status register of an ATA controller.
   See ATA/ATAPI-4 spec, section 7.15.6 */
#define IDE_BSY 0x80
#define IDE_DRDY 0x40
#define IDE_DF 0x20
#define IDE_ERR 0x01

/* ATA protocol commands. */
#define IDE_CMD_READ 0x20
#define IDE_CMD_WRITE 0x30

#ifndef outportb
void outport(unsigned port, unsigned val);
void outportb(unsigned port, unsigned char val);
unsigned inport(unsigned port);
unsigned char inportb(unsigned port);
#endif

void rkintr(void);

static int io_sector;
static int io_count;
static int io_cmd;
static int far *io_buf;

/* Wait for IDE disk to become ready. */
int idewait(int checkerr)
{
    int r;

    while (((r = inportb(0x1f7)) & (IDE_BSY | IDE_DRDY)) != IDE_DRDY)
        ;
    if (checkerr && (r & (IDE_DF | IDE_ERR)) != 0)
        return -1;
    return 0;
}

/* Start the request for IDE disk */
void idestart(int sector, int far *data)
{
    int i;

    idewait(0);
    outportb(0x3f6, 0); /* generate interrupt */
    outportb(0x1f2, 1); /* number of sectors */
    outportb(0x1f3, sector & 0xff);
    outportb(0x1f4, (sector >> 8) & 0xff);
    outportb(0x1f5, 0);
    outportb(0x1f6, 0xe0);
    if (data != 0)
    {
        outportb(0x1f7, IDE_CMD_WRITE);
        for (i = 0; i < 256; i++)
            outport(0x1f0, data[i]);
    }
    else
    {
        outportb(0x1f7, IDE_CMD_READ);
    }
}

void ideio(int sector, int count, char far *buf, int cmd)
{
    io_sector = sector;
    io_count = count;
    io_buf = (int far *)buf;
    io_cmd = cmd;

    idestart(io_sector, cmd == 0 ? io_buf : 0);
}

void ideintr(void)
{
    int i;

    io_sector++;
    io_count--;

    if (io_cmd != 0)
    {
        for (i = 0; i < 256; i++)
            io_buf[i] = inport(0x1f0);
    }
    io_buf += 256;

    if (io_count <= 0)
    {
        rkintr();
    }else{
        idestart(io_sector, io_cmd == 0 ? io_buf : 0);
    }
}
