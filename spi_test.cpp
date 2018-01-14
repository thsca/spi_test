#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <cstring>
#include <cstdio>
#include <time.h>


int transfer(const char *spidev,
                    unsigned char *tx,
                    unsigned char *rx,
                    unsigned int length,
                    unsigned int speed_hz = 10000000,
                    unsigned char bits_per_word = 8,
                    unsigned short delay_usecs = 0)
{
    spi_ioc_transfer spi_transfer;
    
    memset(&spi_transfer, 0, sizeof(spi_ioc_transfer));

    spi_transfer.tx_buf = (unsigned long)tx;
    spi_transfer.rx_buf = (unsigned long)rx;
    spi_transfer.len = length;
    spi_transfer.speed_hz = speed_hz;
    spi_transfer.bits_per_word = bits_per_word;
    spi_transfer.delay_usecs = delay_usecs;

    int spi_fd = ::open(spidev, O_RDWR);
    if (spi_fd < 0 ) {
        printf("Error: Can not open SPI device\n");
        
        return -1;
    }

    int status = ioctl(spi_fd, SPI_IOC_MESSAGE(1), &spi_transfer);
    int speed;
    int ret = ioctl(spi_fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);

    ::close(spi_fd);

    return status;
}

int main()
{
    struct timespec t_start, t_end;
    int dt_us = 0;
    int tmax = 0;
    float tavg = 0;
    float duration = 0;

    while (dt_us <= 1000) {
        clock_gettime(CLOCK_MONOTONIC_RAW, &t_start);

        // spi transfer
        unsigned char tx[21] = { 0 };
        unsigned char rx[21] = { 0 };
        tx[0] = 0x3B | 0x80;
        transfer("/dev/spidev0.1", tx, rx, 21);

        clock_gettime(CLOCK_MONOTONIC_RAW, &t_end);
        dt_us = t_end.tv_sec * 1000000 + t_end.tv_nsec / 1000 - t_start.tv_sec * 1000000 - t_start.tv_nsec / 1000;
        if (dt_us > tmax) {
            tmax = dt_us;
        }
        tavg = (0.01 * (float)dt_us) + (0.99) * tavg;
        usleep(50);
    }

    printf("Max (us): %d\n",tmax);
    printf("Avg (us): %d\n",(int)tavg);

}