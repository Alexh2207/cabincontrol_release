/**
  ******************************************************************************
  * @file   custom_PWM.cpp
  * @author Alejandro Hontanilla Belinchón (a.hontanillab@alumnos.upm.es)
  * @brief  PWM Driver.
  *
  * @note   End-of-degree work.
  *         This module handles a PWM pin using the sysfs interface.
  ******************************************************************************
*/

#ifndef __CUSTOM_PWM_H__
#define __CUSTOM_PWM_H__

/* Includes ------------------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/

namespace CustomPWM{

class PWMDriver{
  int channel;
  int period;
  int duty_cycle;
  bool initialized;

public:

  /**
   * @brief Class constructor. Sets the pin of the GPIO. If there is already GPIO with this pin, the
   *        behavior is undefined.
   */
  PWMDriver(int channel);

  /**
   * @brief Initialize PWM channel with a period and a duty cycle.
   *
   * @param[in] period The period of the square signal to be produced in ms.
   *
   * @param[in] duty_cycle The duty cycle of the signal produced in ms.
   *
   * @return 0 if success, -1 if error.
   */
  int setProperties(int period, int duty_cycle);

  /**
   * @brief Enables the PWM with the configuration previously defined
   *
   * @return 0 if success, -1 if error.
   */
  int enable();

  /**
   * @brief Disables the PWM
   *
   * @return 0 if success, -1 if error.
   */
  int disable();

  /**
   * @brief Class destructor. Free all the related resources.
   */
  ~PWMDriver();
};


/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported Functions --------------------------------------------------------*/
}
#endif /* __CUSTOM_GPIO_H__ */
