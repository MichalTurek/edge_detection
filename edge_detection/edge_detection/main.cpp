#include <stdint.h>
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"
void filter(uint8_t* image, int x,int y,int width, int canal, uint8_t* final_image)
{
	int16_t pixel_value;
	pixel_value = image[x * 3 + y * 3 * width + canal ] * 8;
	pixel_value += image[x * 3 + (y + 1) * 3 * width + canal] * -1;
	pixel_value += image[x * 3 + (y - 1) * 3 * width + canal] * -1;
	pixel_value += image[(x - 1) * 3 + y * 3 * width + canal] * -1;
	pixel_value += image[(x - 1) * 3 + (y + 1) * 3 * width + canal] * -1;
	pixel_value += image[(x - 1) * 3 + (y - 1) * 3 * width + canal] * -1;
	pixel_value += image[(x + 1) * 3 + y * 3 * width + canal] * -1;
	pixel_value += image[(x + 1) * 3 + (y + 1) * 3 * width + canal] * -1;
	pixel_value += image[(x + 1) * 3 + (y - 1) * 3 * width + canal] * -1;
	if (pixel_value < 0) pixel_value = 0;
	else if (pixel_value > 255) pixel_value = 255;
	final_image[x * 3 + y * 3 * width + canal] = pixel_value;

}
void filter_corner(uint8_t* image, int x, int y, int width, int canal, uint8_t* final_image)
{
	int16_t pixel_value;
	pixel_value = image[x * 3 + y * 3 * width + canal] * 8;
	pixel_value += image[x * 3 + (y + 1) * 3 * width + canal] * (-1);
	pixel_value += image[x * 3 + (y - 1) * 3 * width + canal] * (-1);
	pixel_value += image[(x - 1) * 3 + y * 3 * width + canal] * (-1);
	pixel_value += image[(x - 1) * 3 + (y + 1) * 3 * width + canal] * (-1);
	pixel_value += image[(x - 1) * 3 + (y - 1) * 3 * width + canal] * (-1);
	pixel_value += image[(x + 1) * 3 + y * 3 * width + canal] * (-1);
	pixel_value += image[(x + 1) * 3 + (y + 1) * 3 * width + canal] * (-1);
	pixel_value += image[(x + 1) * 3 + (y - 1) * 3 * width + canal] * (-1);
	if (pixel_value < 0) pixel_value = 0;
	else if (pixel_value > 255) pixel_value = 255;
	final_image[x * 3 + y * 3 * width + canal] = pixel_value;
}
int main() {
  int width, height, bpp;
 
  uint8_t* rgb_image = stbi_load("train.png", &width, &height, &bpp, 3);
  uint8_t* final_image = new uint8_t[3*width * height];
  

  for (int x = 1; x < width-1; x++)
  {
	  for (int y = 1; y < height-1; y++)
	  {
		  filter(rgb_image, x, y, width, 0, final_image);
		  filter(rgb_image, x, y, width, 1, final_image);
		  filter(rgb_image, x, y, width, 2, final_image);
	  }
  }
  stbi_write_png("image.png", width, height, 3, final_image, width * 3);
  stbi_image_free(rgb_image);
  return 0;
}