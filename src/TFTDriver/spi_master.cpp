/**
  ******************************************************************************
  * @file   spi_master.cpp
  * @author Alejandro Hontanilla Belinchón (a.hontanillab@alumnos.upm.es)
  * @brief  SPI Handler Module header.
  *
  * @note   End-of-degree work.
  *         This module manages the SPI communications
  ******************************************************************************
*/

#include "spi_master.h"
#include "../custom_gpio/custom_gpio.h"

static int fd;

static CustomGPIO::GPIO gpio17(17);
static bool previous_cmd = false;

struct spi_ioc_transfer tr;

/**
 * @brief Starts the SPI device to allow SPI comunnications.
 * 
 * @param[in] spi_device Indicates which SPI device of type spidev0.<number> 
 *                       within the /dev/ directory will be used.
 *
 * @param[in] mode SPI mode, sets the CPHA and CPOL settings in the SPI device
 *
 * @param[in] bits Number of bits in an SPI word
 *
 * @param[in] speed default speed of SPI device
 *
 * @return 0 if success, -1 if error.
 */
int SPI_Master::spi_start (int spi_device, int mode, int bits, int speed) {

	int ret = 0;

	if(fd == 0){

		//Open file descriptor
		char spiFile[15];
		sprintf(spiFile, "/dev/spidev0.%d", spi_device);

		fd = open(spiFile, O_RDWR);

		if(fd == -1){
		  return -1;
		}
	}

	ret = ioctl(fd, SPI_IOC_WR_MODE32, &mode);

	if (ret == -1)
	  fprintf(stderr,"Can't set SPI write mode");

	ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);

	if (ret == -1)
	  fprintf(stderr,"Can't set SPI read mode");

	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);

	if (ret == -1)
	  fprintf(stderr,"Can't set bits per word");

	ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);

	if (ret == -1)
	  fprintf(stderr,"Can't get bits per word");

	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);

	if (ret == -1)
	  fprintf(stderr,"can't set max speed hz");

	ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);

	if (ret == -1)
	  fprintf(stderr,"can't get max speed hz");

	gpio17.setOutput();
	gpio17.write(true);

	return fd;
}

/**
 * @brief Sends an SPI message
 *
 * @param[in] messages Pointer to the messages to be sent over SPI
 *
 * @param[in] rx Array that will receive the bytes sent over the MISO line
 *
 * @param[in] cmd Flag that indicates if message is a command or data
 *
 * @param[in] size Length in bytes of the message
 *
 * @param[in] delay Delay to be applied after the transaction is completed
 *
 * @param[in] speed Clock frequency
 *
 * @return 0 if success, -1 if error.
 */

void SPI_Master::send_spi_msg(uint8_t* messages, uint8_t rx[], uint8_t cmd, int size, int delay, int speed){

	tr.tx_buf = (unsigned long)messages;
	tr.rx_buf = (unsigned long)rx;
	tr.len = size;
	tr.delay_usecs = delay;
	tr.speed_hz = speed;
	tr.bits_per_word = 0;

	if(previous_cmd != cmd){
	  previous_cmd = cmd;
	  gpio17.write(!cmd);
	}


	// Send SPI data

	int ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);

	if(ret == -1){
		fprintf(stderr,"ERROR: Error in SPI transmission. Couldn't perform. Error description: %s\n", strerror(errno));
	}
}

/**
 * @brief Frees all the resources of the SPI bus
 *
 * @return 0 if success, -1 if error.
 */

int SPI_Master::spi_end(){
	int status = close(fd);
	if(status != -1){
	fd = 0;
	}
	gpio17.~GPIO();

	return status;
}
