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

#include "./APDS9660_lib.h"
#include <cmath>

static uint8_t rgb_gain;

int APDS9660_Master::conf_rgbc(int gain){
	
	uint8_t data[2] = {0x80,0x00};
	uint8_t read_data;
	I2C_Master::read_msg(0x39,0x80,&read_data,1);
	if((read_data & 0x03) != 3){
		printf("Setting enable register to ALS EN and Power ON\n");
		data[1] = read_data | 0x03;
		I2C_Master::write_msg(0x39,data,2);
	}


	I2C_Master::read_msg(0x39,0x8f,&read_data,1);
	if((read_data & 0x3) != gain){
		printf("Setting gain to %d\n", gain);
		data[0] = 0x8f;
		data[1] = read_data | gain;
		I2C_Master::write_msg(0x39,data,2);
		rgb_gain = gain;
	} else{
		rgb_gain = read_data & 0x3;
	}


	return 0;
}



int APDS9660_Master::conf_proximity(int gain, int ledBoost){
	
	uint8_t data[2] = {0x80,0x00};
	uint8_t read_data;
	
	I2C_Master::read_msg(0x39,0x80,&read_data,1);
	
	if((read_data & 0x05) != 5){
		printf("Setting enable register to Prox EN and Power ON\n");
		data[1] = read_data | 0x05;
		I2C_Master::write_msg(0x39,data,2);
	}

	I2C_Master::read_msg(0x39,0x8f,&read_data,1);
	printf("Proximity gain: %x\n", read_data);
	if((read_data & 0x0C) != gain){
		printf("Setting LED gain to %d\n", ledBoost);
		data[0] = 0x8f;
		data[1] = read_data | (gain << 2);
		I2C_Master::write_msg(0x39,data,2);
	}
	
	I2C_Master::read_msg(0x39,0x90,&read_data,1);
	printf("Proximity %x\n", read_data);
	if((read_data & 0x30) != ledBoost){
		printf("Setting LED Boost to %d\n", ledBoost);
		data[0] = 0x90;
		data[1] = read_data | ((ledBoost & 0x03) << 4);
		I2C_Master::write_msg(0x39,data,2);
	}
	
	return 0;

}

int APDS9660_Master::conf_gesture(int ledBoost, uint8_t proximity_enter, uint8_t proximity_exit){

	uint8_t data[2] = {0x80,0x00};
	uint8_t read_data;

	I2C_Master::read_msg(0x39,0xA3,&read_data,1);
	printf("Gesture gain: %x\n", read_data);
	if(read_data != 0x65){
		printf("Setting LED Gain to x8\n");
		data[0] = 0xA3;
		data[1] = read_data | 0x65;
		I2C_Master::write_msg(0x39,data,2);
	}

	I2C_Master::read_msg(0x39,0x80,&read_data,1);
	if((read_data & 0x41) != 0x41){
		printf("Setting enable register to Ges EN and Power ON\n");
		data[0] = 0x80;
		data[1] = read_data | 0x41;
		I2C_Master::write_msg(0x39,data,2);
	}

	I2C_Master::read_msg(0x39,0x8f,&read_data,1);
	printf("Proximity gain: %x\n", read_data);
	if((read_data & 0x0C >> 2) != 3){
		printf("Setting LED gain to %d\n", ledBoost);
		data[0] = 0x8f;
		data[1] = read_data | 0x0C;
		I2C_Master::write_msg(0x39,data,2);
	}

	I2C_Master::read_msg(0x39,0x8f,&read_data,1);
	printf("Proximity gain 2n measure: %x\n", read_data);


	I2C_Master::read_msg(0x39,0x90,&read_data,1);
	printf("Gesture Led Boost %x\n", read_data);
	if(((read_data & 0x30) >> 4) != ledBoost){
		printf("Setting LED Boost to %d\n", ledBoost);
		data[0] = 0x90;
		data[1] = read_data | ((ledBoost & 0x03) << 4);
		I2C_Master::write_msg(0x39,data,2);
	}

	//Clear FIFO data

	data[0] = 0xAB;
	data[1] = 0x04;

	I2C_Master::write_msg(0x39,data,2);

	//Proximity enter

	data[0] = 0xA0;
	data[1] = proximity_enter;

	I2C_Master::write_msg(0x39,data,2);

	//Proximity exit

	data[0] = 0xA1;
	data[1] = proximity_exit;

	I2C_Master::write_msg(0x39,data,2);

	//Proximity threshold

	data[0] = 0xA2;
	data[1] = 0x40;

	I2C_Master::write_msg(0x39,data,2);

	return 0;

}

color_data APDS9660_Master::read_rgbc(){

	uint8_t rgbc[8];

	color_data data;

	I2C_Master::read_msg(0x39,0x94,rgbc,8);

	data.clear = rgbc[1] << 8 | rgbc[0];
	data.red = rgbc[3] << 8 | rgbc[2];
	data.green = rgbc[5] << 8 | rgbc[4];
	data.blue = rgbc[7] << 8 | rgbc[6];

	return data;
}

uint8_t APDS9660_Master::read_proximity(){

	uint8_t prox;

	I2C_Master::read_msg(0x39,0x9C,&prox,1);

	return prox;
}

uint8_t APDS9660_Master::check_gesture(){

	uint8_t data_valid;

	uint8_t fifo_level;

	I2C_Master::read_msg(0x39,0xAF,&data_valid,1);

	I2C_Master::read_msg(0x39,0xAE,&fifo_level,1);

	printf("Level: %d\n",fifo_level);

	return data_valid & 0x01;
}

uint8_t APDS9660_Master::read_ges_fifo_ud(){

	uint8_t data[4];

	uint8_t up = 0,down = 0;

	uint8_t fifo_level;

	uint8_t motion = 0;

	I2C_Master::read_msg(0x39,0xAE,&fifo_level,1);

	I2C_Master::read_msg(0x39,0xAF,data,4);

	while(fifo_level>0 && motion == 0){
		I2C_Master::read_msg(0x39,0xFC,data,4);

		printf("UP: %d", data[0]);

		if(data[0]-data[1] < 2){
			if(down == 0){
				down++;
			}

			if(up > 0){
				motion = DOWN;
			}

		}else if(data[0] - data[1] > 5){

			if(up == 0){
				up++;
			}

			if(down > 0){
				motion = UP;
			}
		}

		I2C_Master::read_msg(0x39,0xAE,&fifo_level,1);
	}

	data[0] = 0xAB;
	data[1] = 0x04;

	I2C_Master::write_msg(0x39,data,2);

	return motion;
}

uint8_t APDS9660_Master::read_ges_fifo_lr(){

	uint8_t data[4];

	uint8_t left = 0,right = 0;

	uint8_t fifo_level;

	uint8_t motion = 0;

	I2C_Master::read_msg(0x39,0xAE,&fifo_level,1);

	I2C_Master::read_msg(0x39,0xAF,data,4);

	while(fifo_level>0 && motion == 0){
		I2C_Master::read_msg(0x39,0xFC,data,4);

		if(data[2]-data[3] < 2){
			if(right == 0){
				right++;
			}

			if(left > 0){
				motion = RIGHT;
			}

		}else if(data[2] - data[3] > 5){

			if(left == 0){
				left++;
			}

			if(right > 0){
				motion = LEFT;
			}
		}

		I2C_Master::read_msg(0x39,0xAE,&fifo_level,1);
	}

	data[0] = 0xAB;
	data[1] = 0x04;

	I2C_Master::write_msg(0x39,data,2);

	return motion;
}

float APDS9660_Master::calc_illuminance(color_data data){

	float illuminance = (data.clear/pow(4,rgb_gain))/0.216;

	return illuminance;

}

