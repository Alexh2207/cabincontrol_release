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


/* Includes ------------------------------------------------------------------*/
#include "custom_PWM.h" // Module header
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <stdlib.h>

/* Private defines -----------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Functions -----------------------------------------------------------------*/


/**
 * @brief Class constructor. Sets the pin of the GPIO. If there is already GPIO with this pin, the
 *        behavior is undefined.
 */
CustomPWM::PWMDriver::PWMDriver(int channel){
	this->channel = channel;
	period = 1000000;
	duty_cycle = period*0.5;
	initialized = false;
}

/**
 * @brief Initialize PWM channel with a period and a duty cycle.
 *
 * @param[in] period The period of the square signal to be produced in ms.
 *
 * @param[in] duty_cycle The duty cycle of the signal produced in ms.
 *
 * @return 0 if success, -1 if error.
 */
int CustomPWM::PWMDriver::setProperties(int period, int duty_cycle){

	//Import the gpio file to user space
	int exp_fd;
	exp_fd = open("/sys/class/pwm/pwmchip0/export", O_WRONLY);
	if (exp_fd == -1)
		return -1;
	std::string channel_string = std::to_string(channel);
	write(exp_fd, channel_string.c_str(), channel_string.size());
	close(exp_fd);


	//Set pwm period: in or out
	int per_fd;
	std::string dir_path = "/sys/class/pwm/pwmchip0/pwm" + std::to_string(channel) + "/period";
	per_fd = open(dir_path.c_str(), O_WRONLY);

	std::string per_string = std::to_string(period);

	if (per_fd == -1)
		return -1;

	if(write(per_fd, per_string.c_str(), per_string.size()) == -1)
		return -1;

	close(per_fd);

	//set pwm duty cycle
	int duty_fd;
	std::string duty_cycle_path = "/sys/class/pwm/pwmchip0/pwm" + std::to_string(channel) + "/duty_cycle";
	duty_fd = open(duty_cycle_path.c_str(), O_WRONLY);

	if (duty_fd == -1)
		return -1;

	std::string duty_string = std::to_string(duty_cycle);

	if(write(duty_fd, duty_string.c_str(), duty_string.size()) == -1)
		return -1;

	initialized = true;
	return 0;
}


/**
 * @brief Enables the PWM with the configuration previously defined
 *
 * @return 0 if success, -1 if error.
 */
int CustomPWM::PWMDriver::enable(){

	int enable_fd;

	std::string rdwr_path = "/sys/class/pwm/pwmchip0/pwm" + std::to_string(channel) + "/enable";

	enable_fd = open(rdwr_path.c_str(), O_WRONLY);

	if (enable_fd == -1)
		return -1;

	if(write(enable_fd, "1", 1) == -1)
		return -1;

	close(enable_fd);

	return 0;
}

/**
 * @brief Disables the PWM
 *
 * @return 0 if success, -1 if error.
 */
int CustomPWM::PWMDriver::disable(){

	int disable_fd;

	std::string rdwr_path = "/sys/class/pwm/pwmchip0/pwm" + std::to_string(channel) + "/enable";

	disable_fd = open(rdwr_path.c_str(), O_WRONLY);

	if (disable_fd == -1)
		return -1;

	if(write(disable_fd, "0", 1) == -1)
		return -1;

	close(disable_fd);

	return 0;
}

/**
 * @brief Class destructor. Free all the related resources.
 */
CustomPWM::PWMDriver::~PWMDriver(){
	int unex_fd;
	unex_fd = open("/sys/class/pwm/pwmchip0/unexport", O_WRONLY);
	if (unex_fd != -1){
		std::string channel_string = std::to_string(channel);

		write(unex_fd, channel_string.c_str(), channel_string.size());
		close(unex_fd);
	}
}

