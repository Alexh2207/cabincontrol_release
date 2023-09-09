/**
  ******************************************************************************
  * @file   APDS9660_lib.cpp
  * @author Alejandro Hontanilla Belinchón (a.hontanillab@alumnos.upm.es)
  * @brief  APDS9660 Handler Module
  *
  * @note   End-of-degree work.
  *         This module manages the APDS9660 sensor
  ******************************************************************************
*/

#ifndef __APDS9660__
#define __APDS9660__

  /* Includes ------------------------------------------------------------------*/
    #include <stdint.h>
    #include <stdio.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <sys/ioctl.h>
    #include <linux/i2c-dev.h>
    #include <linux/i2c.h>
    #include "../i2c_master/i2c_master.h"

#ifdef __cplusplus
extern "C" {
#endif
  /* Exported variables --------------------------------------------------------*/
  /* Exported types ------------------------------------------------------------*/

typedef struct{

	int red;
	int blue;
	int green;
	int clear;

}color_data;

  /* Exported constants --------------------------------------------------------*/
  /* Exported macro ------------------------------------------------------------*/

#define UP 0X01
#define DOWN 0X02
#define LEFT 0x03
#define RIGHT 0x04
#define OUTDOOR_ILLUMINANCE 50

  /* Exported Functions --------------------------------------------------------*/

#ifdef __cplusplus
    namespace APDS9660_Master{
#endif
      /**
       * @brief configures the APDS9660 and wakes it up.
       * 
       * @param[in] gain Gain of the LEDs. Value from 0 to 3.
       *
       * @return 0 if success, -1 if error.
       */
      int conf_rgbc (int gain);
      
      /**
       * @brief configures the proximity detection
       *        
       * @param[in] gain Gain of the LEDs. Value from 0 to 3.
       *
       * @param[in] ledBoost Boost of the current of the LEDs, allowing further detection.
       *
       * @return non negative value if success, -1 if error.
       */
      int conf_proximity(int gain, int ledBoost);

      /**
       * @brief configures the gesture detection
       *
       * @param[in] ledBoost Boost of the current of the LEDs, allowing further detection.
       *
       * @param[in] proximity_enter Threshold to enter the gesture detection state machine
       *
       * @param[in] proximity_exit Threshold to exit the gesture detection state machine
       *
       * @return non negative value if success, -1 if error.
       */
      int conf_gesture(int ledBoost, uint8_t proximity_enter, uint8_t proximity_exit);

      /**
       * @brief configures the proximity detection
       *
       * @return 0 if not enough data in the FIFO to read, 1 if enough data
       */

      uint8_t check_gesture();

      /**
       * @brief reads the gesture FIFO and detects up and down motion
       *
       * @return 0 if gesture not detected, value if gesture detected
       */

      uint8_t read_ges_fifo_ud();

      /**
       * @brief reads the gesture FIFO and detects left and right motion
       *
       * @return 0 if gesture not detected, value if gesture detected
       */

      uint8_t read_ges_fifo_lr();

      /**
       * @brief Reads the RGBC value given by the sensor
       *  
       * @param[out] rgbc Array of 8 chars to store the RGBC values.
       *
       * @return non negative value if success, -1 if error.
       */

      color_data read_rgbc();

      /**
       * @brief Reads the proximity value given by the sensor
       *
       * @return Proximity byte read from the sensor
       */
      
      uint8_t read_proximity();
      
      /**
       * @brief Calculates the illuminance read by the sensor
       *
       * @return Illuminance in Lux
       *
       */

      float calc_illuminance(color_data data);

#ifdef __cplusplus 
    }
#endif
    
#ifdef __cplusplus
}
#endif

#endif
