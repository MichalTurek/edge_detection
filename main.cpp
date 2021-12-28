#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>
#include <array>

#define _USE_MATH_DEFINES
#include <cmath>
#include <stdint.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define CHANNEL_NUM 1

const int8_t laplacian[] = { -1, -1, -1,
                             -1,  8, -1,
                             -1, -1, -1 };

const int8_t sobel_x[] = { 1, 0, -1,
                           2, 0, -2,
                           1, 0, -1 };

const int8_t sobel_y[] = { 1, 2, 1,
                           0, 0, 0,
                         -1, -2, -1 };


const float gaussian[] = { 0.0030,0.0133,0.0219,0.0133,0.0030,
    0.0133, 0.0596,    0.0983,    0.0596,    0.0133,
    0.0219,    0.0983,    0.1621,    0.0983 ,   0.0219,
    0.0133,    0.0596,    0.0983,    0.0596,    0.0133,
    0.0030,    0.0133,    0.0219,    0.0133,    0.0030 };



uint8_t pixelInPosition(int x, int y, std::vector<uint8_t>& vector, int width, int height) {
  if (x > width - 1)  x -= 2;
  else if (x < 0) x += 2;

  if (y > height - 1) y -= 2;
  else if (y < 0) y += 2;

  int index = y * (long long)width + x;
  if (index < 0 || index >= vector.size()) {
      
    return -1;
  }
  return vector[y * (long long)width + x];
}

float angleInPosition(int x, int y, std::vector<float>& vector, int width, int height) {
    if (x > width - 1)  x -= 2;
    else if (x < 0) x += 2;

    if (y > height - 1) y -= 2;
    else if (y < 0) y += 2;

    int index = y * (long long)width + x;
    if (index < 0 || index >= vector.size()) {

        return -1;
    }
    return vector[y * (long long)width + x];
}

void filter(const int x, const int y, std::vector<uint8_t>& inputImage, std::vector<uint8_t>& outputImage, int width, int height) {
  int16_t newPixelValue = 0;
  int matrix_size = sqrt(sizeof(gaussian) / sizeof(gaussian[0]));
  int iterator = 0;

  for (int i = -matrix_size / 2; i <= matrix_size / 2; i++)
  {
      for (int j = -matrix_size / 2; j <= matrix_size / 2; j++)
      {
          newPixelValue += (int16_t)((float)pixelInPosition(x + i, y + j, inputImage, width, height) * gaussian[iterator]);
          iterator++;
      }
  }

  newPixelValue = newPixelValue > 255 ? 255 : newPixelValue;
  newPixelValue = newPixelValue < 0 ? 0 : newPixelValue;
  outputImage.push_back(newPixelValue);
}

void filter_sobel(const int x, const int y, std::vector<uint8_t>& inputImage, 
    std::vector<uint8_t>& outputImage, std::vector<float>& outputAngle, int width, int height) {
    int16_t newPixelValue = 0;
    int16_t newPixelValue_x = 0;
    int16_t newPixelValue_y = 0;
    int matrix_size = sqrt(sizeof(sobel_x) / sizeof(sobel_x[0]));
    int iterator = 0;

    for (int i = -matrix_size / 2; i <= matrix_size / 2; i++)
    {
        for (int j = -matrix_size / 2; j <= matrix_size / 2; j++)
        {
            int pixel = pixelInPosition(x + i, y + j, inputImage, width, height);
            newPixelValue_x += (int16_t)((float) pixel * sobel_x[iterator]);
            newPixelValue_y += (int16_t)((float) pixel * sobel_y[iterator]);
            iterator++;
        }
    }
    newPixelValue = sqrt(pow(newPixelValue_x,2) + pow(newPixelValue_y,2));
    newPixelValue = newPixelValue > 255 ? 255 : newPixelValue;
    newPixelValue = newPixelValue < 0 ? 0 : newPixelValue;
    outputImage.push_back(newPixelValue);
    outputAngle.push_back(atan2(newPixelValue_y, newPixelValue_x));
}

void NonMaximumSuppression(const int x, const int y, std::vector<uint8_t>& inputImage, 
    std::vector<float>& inputAngle, std::vector<uint8_t>& outputImage, int width, int height) {
    int16_t newPixelValue = 0;
    uint8_t first_neighbour, second_neighbour;
    double angle = 180 * (double)angleInPosition(x, y, inputAngle, width, height) / M_PI;
    if ((0 <= angle < 22.5) or (157.5 <= angle <= 180)) {
        first_neighbour = pixelInPosition(x, y + 1, inputImage, width, height);
        second_neighbour = pixelInPosition(x, y - 1, inputImage, width, height);
    }
    else if (22.5 <= angle < 67.5) {
        first_neighbour = pixelInPosition(x+1, y -1, inputImage, width, height);
        second_neighbour = pixelInPosition(x-1, y + 1, inputImage, width, height);
    }
    else if (67.5 <= angle < 112.5) {
        first_neighbour = pixelInPosition(x + 1, y, inputImage, width, height);
        second_neighbour = pixelInPosition(x - 1, y, inputImage, width, height);
    }
    else if (112.5 <= angle < 157.5) {
        first_neighbour = pixelInPosition(x - 1, y - 1, inputImage, width, height);
        second_neighbour = pixelInPosition(x + 1, y + 1, inputImage, width, height);
    }
    int pixel = pixelInPosition(x, y, inputImage, width, height);
    if (pixel >= first_neighbour and pixel >= second_neighbour){
        outputImage.push_back(pixel);
    }
    else {
        outputImage.push_back(0);
    }
        


    outputImage.push_back(newPixelValue);
}

int main() {
  int width, height, bpp;

  uint8_t* grayScaleImage = stbi_load("x.jpg", &width, &height, &bpp, 1);

  std::vector<uint8_t> inputImage(grayScaleImage, grayScaleImage + (width * (long long)height));
  std::vector<uint8_t> gaussianImage;
  gaussianImage.reserve(inputImage.size());
  std::vector<uint8_t> sobelImage;
  sobelImage.reserve(inputImage.size());
  std::vector<float> sobelAngle;
  sobelAngle.reserve(inputImage.size());
  std::vector<uint8_t> outputSuppression;
  outputSuppression.reserve(inputImage.size());

  int x = 0;
  int y = 0;
  for (auto pixel : inputImage) {
    filter(x, y, inputImage, gaussianImage, width, height);
    x++;
    if (x == width) {
      x = x % width;
      y++;
    }
  }

  x = 0;
  y = 0;
  for (auto pixel : gaussianImage) {
      filter_sobel(x, y, gaussianImage, sobelImage, sobelAngle, width, height);
      x++;
      if (x == width) {
          x = x % width;
          y++;
      }
  }

  x = 0;
  y = 0;
  for (auto pixel : gaussianImage) {
      NonMaximumSuppression(x, y, sobelImage, sobelAngle, outputSuppression,  width, height);
      x++;
      if (x == width) {
          x = x % width;
          y++;
      }
  }

  stbi_write_png("output1.png", width, height, CHANNEL_NUM, outputSuppression.data(), width * CHANNEL_NUM);
  stbi_image_free(grayScaleImage);

  return 0;
}