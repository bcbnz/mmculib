#include "config.h"
#include "msd.h"
#include "sdcard.h"

#include <string.h>


static msd_size_t
sdcard_msd_read (void *dev, msd_addr_t addr, void *buffer, msd_size_t size)
{
    return sdcard_read (dev, addr, buffer, size);
}


static msd_size_t
sdcard_msd_write (void *dev, msd_addr_t addr, const void *buffer,
                  msd_size_t size)
{
    return sdcard_write (dev, addr, buffer, size);
}


static msd_status_t
sdcard_msd_status_get (void *dev __unused__)
{
    return MSD_STATUS_READY;
}


static void
sdcard_msd_shutdown (void *dev)
{
    return sdcard_shutdown (dev);
}


static const msd_ops_t sdcard_msd_ops =
{
    .read = sdcard_msd_read,
    .write = sdcard_msd_write,
    .status_get = sdcard_msd_status_get,
    .shutdown = sdcard_msd_shutdown,
};


static msd_t sdcard_msd =
{
    .handle = 0,
    .ops = &sdcard_msd_ops,
    .media_bytes = 0,
    .block_bytes = SDCARD_BLOCK_SIZE,
    .flags = {.removable = 1, .partial_read = 1, .partial_write = 0},
    .name = "SDCard"
};


static const sdcard_cfg_t sdcard_cfg =
{
    .spi = {.channel = SDCARD_SPI_CHANNEL,
            .clock_divisor =  F_CPU / 20e6 + 1,
            .cs = SDCARD_CS,
            .mode = SPI_MODE_0,
            .bits = 8},
};


msd_t *
sdcard_msd_init (void)
{
    sdcard_msd.handle = sdcard_init (&sdcard_cfg);
    if (!sdcard_msd.handle)
        return NULL;

    if (sdcard_probe (sdcard_msd.handle))
        return NULL;

    /* The number of pages should be probed...  */
    sdcard_msd.media_bytes = sdcard_capacity_get (sdcard_msd.handle);

    return &sdcard_msd;
}
