#include <stdint.h>
#include <vector>
using std::vector;
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "Constants.h"
#include "stb_image.h"
#include "stb_image_write.h"

struct adjacent_pixels
{
	int center = 0, left = 0, right = 0, bottom = 0, top = 0;
	int  left_top = 0, left_bottom = 0, right_top = 0, right_bottom = 0;
	constexpr adjacent_pixels(int x, int y, int width) :
		center(x * 3 + y * 3 * width),
		left((x - 1) * 3 + y * 3 * width),
		right((x + 1) * 3 + y * 3 * width),
		top(x * 3 + (y - 1) * 3 * width),
		bottom(x * 3 + (y + 1) * 3 * width),
		left_top((x - 1) * 3 + (y - 1) * 3 * width),
		left_bottom(((x - 1) * 3 + (y + 1) * 3 * width)),
		right_top((x + 1) * 3 + (y - 1) * 3 * width),
		right_bottom((x + 1) * 3 + (y + 1) * 3 * width)
	{};
};
enum class Laplacian
{
	center = 8,
	left = -1,
	right = -1,
	top = -1,
	bottom = -1,
	left_top = -1,
	left_bottom = -1,
	right_top = -1,
	right_bottom = -1
};
void filter(uint8_t* image,const int x,const int y,const int width, uint8_t* final_image)
{
	int16_t pixel_value=0;
	adjacent_pixels adj_pixels(x, y, width);

	for (int canal = 0; canal < Canals; canal++)
	{
		pixel_value = image[adj_pixels.center + canal] * static_cast<int16_t>(Laplacian::center);
		pixel_value += image[adj_pixels.bottom + canal] * static_cast<int16_t>(Laplacian::bottom);
		pixel_value += image[adj_pixels.top + canal] * static_cast<int16_t>(Laplacian::top);
		pixel_value += image[adj_pixels.left + canal] * static_cast<int16_t>(Laplacian::left);
		pixel_value += image[adj_pixels.right + canal] * static_cast<int16_t>(Laplacian::right);
		pixel_value += image[adj_pixels.left_top + canal] * static_cast<int16_t>(Laplacian::left_top);
		pixel_value += image[adj_pixels.left_bottom + canal] * static_cast<int16_t>(Laplacian::left_bottom);
		pixel_value += image[adj_pixels.right_top + canal] * static_cast<int16_t>(Laplacian::right_top);
		pixel_value += image[adj_pixels.right_bottom + canal] * static_cast<int16_t>(Laplacian::right_bottom);
		if (pixel_value < 0) pixel_value = 0;
		else if (pixel_value > 255) pixel_value = 255;
		final_image[x * 3 + y * 3 * width + canal] = pixel_value;
	}
	

}

int main() {
  int width, height, bpp,size;
 

  uint8_t* rgb_image = stbi_load("train.png", &width, &height, &bpp, 3);
  size = width * height * 3;
  uint8_t* final_image = new uint8_t[3 * width * height]{ 0 };
  for (int x = 1; x < width-1; x++)
  {
	  for (int y = 1; y < height-1; y++)
	  {
		  filter(rgb_image, x, y, width, final_image);
	  }
  }
  stbi_write_png("image.png", width, height, 3, final_image, width * 3);
  stbi_image_free(rgb_image);
  return 0;
}