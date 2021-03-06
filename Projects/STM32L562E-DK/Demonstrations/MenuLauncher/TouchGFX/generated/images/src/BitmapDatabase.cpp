// 4.12.2 dither_algorithm=2 alpha_dither=yes layout_rotation=0 opaque_image_format=RGB565 non_opaque_image_format=ARGB8888 section=ExtFlashSection extra_section=ExtFlashSection generate_png=no 0xed14995e
// Generated by imageconverter. Please, do not edit!

#include <touchgfx/Bitmap.hpp>
#include <BitmapDatabase.hpp>

extern const unsigned char _background[]; // BITMAP_BACKGROUND_ID = 0, Size: 240x240 pixels
extern const unsigned char _background_overlay[]; // BITMAP_BACKGROUND_OVERLAY_ID = 1, Size: 240x240 pixels
extern const unsigned char _icon_ai[]; // BITMAP_ICON_AI_ID = 2, Size: 136x136 pixels
extern const unsigned char _icon_ble[]; // BITMAP_ICON_BLE_ID = 3, Size: 136x136 pixels
extern const unsigned char _icon_info[]; // BITMAP_ICON_INFO_ID = 4, Size: 136x136 pixels
extern const unsigned char _icon_measurements[]; // BITMAP_ICON_MEASUREMENTS_ID = 5, Size: 136x136 pixels
extern const unsigned char _icon_music[]; // BITMAP_ICON_MUSIC_ID = 6, Size: 136x136 pixels
extern const unsigned char _st_logo1[]; // BITMAP_ST_LOGO1_ID = 7, Size: 171x94 pixels
extern const unsigned char _st_logo2[]; // BITMAP_ST_LOGO2_ID = 8, Size: 171x32 pixels
extern const unsigned char _sw1_pic[]; // BITMAP_SW1_PIC_ID = 9, Size: 240x151 pixels

const touchgfx::Bitmap::BitmapData bitmap_database[] =
{
    { _background, 0, 240, 240, 0, 0, 240, (uint8_t)(touchgfx::Bitmap::RGB565) >> 3, 240, (uint8_t)(touchgfx::Bitmap::RGB565) & 0x7 },
    { _background_overlay, 0, 240, 240, 0, 0, 240, (uint8_t)(touchgfx::Bitmap::ARGB8888) >> 3, 52, (uint8_t)(touchgfx::Bitmap::ARGB8888) & 0x7 },
    { _icon_ai, 0, 136, 136, 21, 20, 94, (uint8_t)(touchgfx::Bitmap::ARGB8888) >> 3, 96, (uint8_t)(touchgfx::Bitmap::ARGB8888) & 0x7 },
    { _icon_ble, 0, 136, 136, 21, 20, 94, (uint8_t)(touchgfx::Bitmap::ARGB8888) >> 3, 96, (uint8_t)(touchgfx::Bitmap::ARGB8888) & 0x7 },
    { _icon_info, 0, 136, 136, 21, 20, 94, (uint8_t)(touchgfx::Bitmap::ARGB8888) >> 3, 96, (uint8_t)(touchgfx::Bitmap::ARGB8888) & 0x7 },
    { _icon_measurements, 0, 136, 136, 21, 20, 94, (uint8_t)(touchgfx::Bitmap::ARGB8888) >> 3, 96, (uint8_t)(touchgfx::Bitmap::ARGB8888) & 0x7 },
    { _icon_music, 0, 136, 136, 21, 20, 94, (uint8_t)(touchgfx::Bitmap::ARGB8888) >> 3, 96, (uint8_t)(touchgfx::Bitmap::ARGB8888) & 0x7 },
    { _st_logo1, 0, 171, 94, 44, 20, 83, (uint8_t)(touchgfx::Bitmap::ARGB8888) >> 3, 63, (uint8_t)(touchgfx::Bitmap::ARGB8888) & 0x7 },
    { _st_logo2, 0, 171, 32, 46, 16, 1, (uint8_t)(touchgfx::Bitmap::ARGB8888) >> 3, 1, (uint8_t)(touchgfx::Bitmap::ARGB8888) & 0x7 },
    { _sw1_pic, 0, 240, 151, 9, 0, 224, (uint8_t)(touchgfx::Bitmap::ARGB8888) >> 3, 151, (uint8_t)(touchgfx::Bitmap::ARGB8888) & 0x7 }
};

namespace BitmapDatabase
{
const touchgfx::Bitmap::BitmapData* getInstance()
{
    return bitmap_database;
}
uint16_t getInstanceSize()
{
    return (uint16_t)(sizeof(bitmap_database) / sizeof(touchgfx::Bitmap::BitmapData));
}
}
