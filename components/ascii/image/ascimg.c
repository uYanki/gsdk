#include "ascimg.h"

//---------------------------------------------------------------------------
// Definitions
//---------------------------------------------------------------------------

#define LOG_LOCAL_TAG   "ascii-image"
#define LOG_LOCAL_LEVEL LOG_LEVEL_INFO

//---------------------------------------------------------------------------
// Prototypes
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Variables
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

/**
 * @brief display ascii images
 *
 * @param[in] cpu8Image please refer to the image generation method @ref tools\imgae\ascimg
 * @param[in] u16Width  image width
 * @param[in] u16Height image height
 *
 * @return none
 */
void DisplayAsciiImgae(const uint8_t* cpu8Image, uint16_t u16Width, uint16_t u16Height)
{
    const char* szTable = "@B%%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^`'. ";

    uint8_t u8Index;

    while (u16Height--)
    {
        for (uint16_t j = 0; j < u16Width; ++j)
        {
            u8Index = ((sizeof(szTable) - 1) / 256.0f) * (*cpu8Image++);
            putchar(szTable[u8Index]);
        }

        // putchar('\r');
        putchar('\n');
    }
}
