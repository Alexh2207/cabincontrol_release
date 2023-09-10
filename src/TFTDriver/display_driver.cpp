/**
  ******************************************************************************
  * @file   display_driver.h
  * @author Alejandro Hontanilla Belinchón (a.hontanillab@alumnos.upm.es)
  * @brief  TFTDisplay driver. Provides a layer of abstraction over SPI comms.
  *
  * @note   End-of-degree work. Optimization inspired on algorithms developed by
  * 		Pablo Sanmillán Fierro
  ******************************************************************************
*/

#include "display_driver.h"
#include <algorithm>

//Write in memory

uint8_t mem_data_cmd[] = { 0x2C };

//Software reset

uint8_t soft_reset[] = { 0x01 };

//Get out of sleep mode

uint8_t sleep_out[] = { 0x11 };

//Sleep in

uint8_t sleep_in[] = { 0x10 };

//Color mode command

uint8_t color_mode[] = { 0x3A };

//Color mode data:

uint8_t color_mode_data[] = { 0x55 };

//Memory data access command

uint8_t mem_dat_access[] = { 0x36 };

//Memory data access data

uint8_t mem_dat_access_mode[] = { 0x00 };

//Set column address command

uint8_t col_set_addr[] = { 0x2A };

//Set row address command

uint8_t row_set_addr[] = { 0x2B };


uint8_t invert_on[] = { 0x21 };

//Set normal mode of operation

uint8_t normal_mode[] = { 0x13 };

//Power on the display

uint8_t display_on[] = { 0x29 };

uint8_t display_off[] = { 0x28 };

uint8_t idle_on[] = { 0x39 };

//Dummy array for received data

uint8_t default_rx[5] = {0, };

//SPI speed

static uint32_t speed = 62500000;

//Private helper function to set the column and row addresses

void prep_write(uint8_t column_data[], uint8_t row_data[]);


/**
 * @brief Initializes the display with a black background.
 */
void Display_driver::init_display(){

	SPI_Master::spi_start(0, 0, 8, 62500000);

	SPI_Master::send_spi_msg(soft_reset, default_rx, 1, sizeof(soft_reset), 0, speed);

	sleep(0.5);

	SPI_Master::send_spi_msg(sleep_out, default_rx, 1, sizeof(sleep_out), 10000, speed);
	SPI_Master::send_spi_msg(color_mode, default_rx, 1, sizeof(color_mode), 10000, speed);
	SPI_Master::send_spi_msg(color_mode_data, default_rx, 0, sizeof(color_mode_data), 10000, speed);
	SPI_Master::send_spi_msg(mem_dat_access, default_rx, 1, sizeof(mem_dat_access), 10000, speed);
	SPI_Master::send_spi_msg(mem_dat_access_mode, default_rx, 0, sizeof(mem_dat_access_mode), 10000, speed);
	SPI_Master::send_spi_msg(invert_on, default_rx, 1, sizeof(invert_on), 10000, speed);
	SPI_Master::send_spi_msg(normal_mode, default_rx, 1, sizeof(normal_mode), 10000, speed);
	SPI_Master::send_spi_msg(display_on, default_rx, 1, sizeof(display_on), 10000, speed);
	SPI_Master::send_spi_msg(mem_data_cmd, default_rx, 1, sizeof(mem_data_cmd), 10000, speed);

	int reset_size = 57600 * 2;
	int msg_size = 32;

	uint8_t msg[msg_size];

	std::fill_n(msg, msg_size, BACKGROUND);
	long i;

	for(i = 1; i < reset_size; i += msg_size){
		SPI_Master::send_spi_msg(msg, default_rx, 0, sizeof(msg), 0, speed);
	}

	if(i != reset_size){
		SPI_Master::send_spi_msg(msg, default_rx, 0, i % msg_size, 0, speed);
	}

}

/**
 * @brief Writes a text string to the Display
 *
 * @param[in] start_x X coordinate of the top left corner of the bounding box containing the first character
 *
 * @param[in] start_y Y coordinate of the top left corner of the bounding box containing the first character
 *
 * @param[in] string string to write
 *
 * @param[in] color color of the text to write
 *
 * @param[in] font font of the text
 */
void Display_driver::write_fast_string(int start_x, int start_y, char str[], uint8_t color[], GFXfont font){

	int max_height = 0, min_height = 0, x_length = 0;

	GFXglyph glyph;

	//Calculate maximum height of letters over the cursor, maximum height under the cursor and total string length

	for(size_t i = 0; i < strlen(str); i++){
		glyph = font.glyph[str[i] - font.first];

		if(abs(glyph.yOffset) > max_height)
			max_height = abs(glyph.yOffset);

		if(min_height < glyph.height + glyph.yOffset)
			min_height = glyph.height + glyph.yOffset;

		x_length += glyph.xAdvance;
	}

	uint16_t char_array[x_length*(max_height+min_height)];

	int current_index = 0;

	//Populate the array with background color

	std::fill_n(char_array, x_length*(max_height+min_height), BACKGROUND << 8 | BACKGROUND);

	//Process row by row each letter

	for(int row = 0; row < max_height + min_height; row++){

		for(size_t i = 0; i < strlen(str); i++){

			char character = str[i];

			glyph = font.glyph[character - font.first];

			// Calculate relative row position with respect to to the start of the glyph
			// If the relative position is higher than or equal 0 the row contains relevant data from the glyph

			int relative_row_top = row - (max_height + glyph.yOffset);

			//Calculate relative position of row to lowest height of glyph (under the cursor)

			int relative_row_bottom = (row - max_height) - (glyph.height + glyph.yOffset);

			if(relative_row_top >= 0 && relative_row_bottom <= 0){

				uint8_t char_bytes[40];

				memset(char_bytes, 0, sizeof(char_bytes));

				int k;

				for(k=0; k<= (glyph.height*glyph.width)/8; k++){
					char_bytes[k] = font.bitmap[glyph.bitmapOffset+k];
				}

				k = 0;

				//Get all pixels encoded in 16 bits with RGB565

				uint16_t all_bytes[glyph.height*glyph.width];
				int bytes_element = 0;

				for(long i = 0; i <= (glyph.height*glyph.width)/8; i++){

					for(long j = 0; (j < 8) && k < glyph.height*glyph.width; j++){

						if(((char_bytes[i] >> (7-j)) & 1) == 1){
							all_bytes[bytes_element] = color[1] << 8 | color[0];
							bytes_element++;
						}else{
							all_bytes[bytes_element] = BACKGROUND << 8 | BACKGROUND;
							bytes_element++;
						}
						k++;

					}

				}

				//Add the bytes of the row

				for(int byte = 0; byte < glyph.width; byte ++){

					char_array[current_index + glyph.xOffset + byte] = all_bytes[relative_row_top * glyph.width + byte];

				}

			}

			//Next character

			current_index = current_index + glyph.xAdvance;

		}
	}

	draw_icon(start_x, start_y, x_length, char_array, sizeof(char_array)/2);
}

/**
 * @brief Draws a given icon to the display overriding the color of the bitmap
 *
 * @param[in] start_x X coordinate of the top left corner of the bounding box containing the icon
 *
 * @param[in] start_y Y coordinate of the top left corner of the bounding box containing the icon
 *
 * @param[in] byte_map bytes of the icon with format 565 of the icon to write
 *
 * @param[in] size size of the bitmap
 *
 * @param[in] color color of the icon
 */
void Display_driver::draw_icon_col(int start_x, int start_y, int width, uint16_t bit_map[],int size, uint8_t color[]){

	uint8_t height = size/(width);

	uint8_t col_data[4] = {0x00,};
	uint8_t row_data[4] = {0x00,};

	col_data[1] = start_x;
	row_data[1] = start_y;

	col_data[3] = col_data[1] + width - 1;
	row_data[3] = row_data[1] + height - 1;

	prep_write(col_data,row_data);

	int j = 0;
	uint8_t msg[32];

	std::fill_n(msg, sizeof(msg), BACKGROUND);

	for(long i = 0; i < size; i++){

		if(!(bit_map[i] == ((BACKGROUND << 8) | BACKGROUND))){
			msg[j++] = color[0];
			msg[j++] = color[1];
		}else{
			j+=2;
		}

	    if(j == sizeof(msg)){
	      j = 0;
	      SPI_Master::send_spi_msg(msg, default_rx, 0, sizeof(msg), 0, speed);
	      std::fill_n(msg, sizeof(msg), BACKGROUND);
	    }
	}

	if(j != 0){
		SPI_Master::send_spi_msg(msg, default_rx, 0, j, 0, speed);
	}

}

/**
 * @brief Draws a given icon to the display without overriding the color of the bitmap
 *
 * @param[in] start_x X coordinate of the top left corner of the bounding box containing the icon
 *
 * @param[in] start_y Y coordinate of the top left corner of the bounding box containing the icon
 *
 * @param[in] byte_map bytes of the icon with format 565 of the icon to write
 *
 * @param[in] size size of the bitmap
 */
void Display_driver::draw_icon(int start_x, int start_y, int width, uint16_t bit_map[],int size){

	uint8_t height = size/(width);

	uint8_t col_data[4] = {0x00,};
	uint8_t row_data[4] = {0x00,};

	col_data[1] = start_x;
	row_data[1] = start_y;

	col_data[3] = col_data[1] + width - 1;
	row_data[3] = row_data[1] + height - 1;

	prep_write(col_data,row_data);

	int j = 0;
	uint8_t msg[32];

	for(long i = 0; i < size; i++){
		msg[j++] = bit_map[i] & 255;
		msg[j++] = bit_map[i] >> 8;

		if(j == sizeof(msg)){
			j = 0;
			SPI_Master::send_spi_msg(msg, default_rx, 0, sizeof(msg), 0, speed);
		}
	}

	if(j != 0){
		SPI_Master::send_spi_msg(msg, default_rx, 0, j, 0, speed);
	}

}

/**
 * @brief Frees up the resources used by the display
 */
void Display_driver::uninit(){

	SPI_Master::send_spi_msg(sleep_in, default_rx, 1, sizeof(sleep_in), 10000, speed);

	SPI_Master::spi_end();

}

void prep_write(uint8_t column_data[], uint8_t row_data[]){

	SPI_Master::send_spi_msg(col_set_addr, default_rx, 1, sizeof(col_set_addr), 10000, speed);
	SPI_Master::send_spi_msg(column_data, default_rx, 0, 4, 0, speed);
	SPI_Master::send_spi_msg(row_set_addr, default_rx, 1, sizeof(row_set_addr), 10000, speed);
	SPI_Master::send_spi_msg(row_data, default_rx, 0, 4, 0, speed);
	SPI_Master::send_spi_msg(mem_data_cmd, default_rx, 1, sizeof(mem_data_cmd), 10000, speed);

}


