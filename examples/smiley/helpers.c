#include "helpers.h"

void colorize(int height, int width, RGBTRIPLE image[height][width])
{
    // #FFFFFF
    RGBTRIPLE black;
    black.rgbtBlue = 0x00;
    black.rgbtGreen = 0x00;
    black.rgbtRed = 0x00;

    // #FF69B4
    RGBTRIPLE pink;
    pink.rgbtRed = 0xFF;
    pink.rgbtGreen = 0x69;
    pink.rgbtBlue = 0xB4;

    // Change all black pixels to a color of your choosing
    for (int h = 0; h < height; h++)
    {
        for (int w = 0; w < width; w++)
        {
            if (image[h][w].rgbtRed == black.rgbtRed && image[h][w].rgbtBlue == black.rgbtBlue &&
                image[h][w].rgbtGreen == black.rgbtGreen)
            {
                image[h][w] = pink;
            }
        }
    }
}