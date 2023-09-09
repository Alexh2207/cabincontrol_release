/**
  ******************************************************************************
  * @file   spi_master.h
  * @author Alejandro Hontanilla Belinchón (a.hontanillab@alumnos.upm.es)
  * @brief  SPI Handler Module header.
  *
  * @note   End-of-degree work.
  *         This module manages the SPI communications
  ******************************************************************************
*/

#ifndef __SPI_ADAPTER_H__
#define __SPI_ADAPTER_H__

  /* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif
  /* Exported variables --------------------------------------------------------*/
  /* Exported types ------------------------------------------------------------*/
  /* Exported constants --------------------------------------------------------*/
  /* Exported macro ------------------------------------------------------------*/
  /* Exported Functions --------------------------------------------------------*/

#ifdef __cplusplus
    namespace SPI_Master{
#endif

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
	int spi_start (int spi_device, int mode, int bits, int speed);

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
	void send_spi_msg (uint8_t* messages, uint8_t rx[], uint8_t cmd, int size, int delay, int speed);

	/**
	* @brief Frees all the resources of the SPI bus
	*
	* @return 0 if success, -1 if error.
	*/
	int spi_end();

#ifdef __cplusplus 
    }
#endif
    
#ifdef __cplusplus
}
#endif

#endif
