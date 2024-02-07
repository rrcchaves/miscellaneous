#include <cstdint>
#include <vector>
#include <fstream>
#include <ostream>
#include <cstring>

#pragma pack(1)
struct BMPFileHeader
{
    uint16_t signature;
    uint32_t file_size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t offset_data;
};

struct BMPInfoHeader
{
    uint32_t size;
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t bit_count;
    uint32_t compression;
    uint32_t image_size;
    int32_t x_pixels_per_meter;
    int32_t y_pixels_per_meter;
    uint32_t colors_used;
    uint32_t colors_important;
};

struct RGBQUAD
{
    uint8_t rgb_blue;
    uint8_t rgb_green;
    uint8_t rgb_red;
    uint8_t rgb_reserved;
};
#pragma pack()

std::vector<uint8_t> generate_grayscale_image(int width, int height)
{
    std::vector<uint8_t> image(width * height, 0);
    for (int h = 0; h < height; h++)
    {
        for (int w = 0; w < width; w++)
        {
            image[h * width + w] = w % 256;
        }
    }
    return image;
}

std::vector<uint8_t> encode_grayscale_bmp(const std::vector<uint8_t> &image, int image_width, int image_height)
{
    const int NUMBER_OF_COLORS = 256;
    const int COLOR_PALETTE_SIZE = NUMBER_OF_COLORS * sizeof(RGBQUAD);
    const int HEADER_OFFSET = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader) + COLOR_PALETTE_SIZE;
    const int TOTAL_FILE_SIZE = HEADER_OFFSET + image.size();

    BMPFileHeader fileHeader;
    memset(&fileHeader, 0, sizeof(fileHeader));
    fileHeader.signature = 0x4D42;
    fileHeader.file_size = TOTAL_FILE_SIZE;
    fileHeader.offset_data = HEADER_OFFSET;

    BMPInfoHeader infoHeader;
    memset(&infoHeader, 0, sizeof(infoHeader));
    infoHeader.size = sizeof(BMPInfoHeader);
    infoHeader.width = image_width;
    infoHeader.height = image_height;
    infoHeader.planes = 1;
    infoHeader.bit_count = 8;
    infoHeader.compression = 0;
    infoHeader.image_size = image_width * image_height;
    infoHeader.x_pixels_per_meter = 0;
    infoHeader.y_pixels_per_meter = 0;
    infoHeader.colors_used = NUMBER_OF_COLORS;
    infoHeader.colors_important = 0;

    RGBQUAD colorPalette[NUMBER_OF_COLORS];
    for (int i = 0; i < NUMBER_OF_COLORS; i++)
    {
        colorPalette[i].rgb_red = i;
        colorPalette[i].rgb_green = i;
        colorPalette[i].rgb_blue = i;
        colorPalette[i].rgb_reserved = 0;
    }

    std::vector<uint8_t> bmp(TOTAL_FILE_SIZE);
    memcpy(bmp.data(), &fileHeader, sizeof(fileHeader));
    memcpy(bmp.data() + sizeof(fileHeader), &infoHeader, sizeof(infoHeader));
    memcpy(bmp.data() + sizeof(fileHeader) + sizeof(infoHeader), colorPalette, COLOR_PALETTE_SIZE);
    memcpy(bmp.data() + HEADER_OFFSET, image.data(), image.size());

    return bmp;
}

void save_image_file(const std::string &filename, const std::vector<uint8_t> &image)
{
    std::ofstream file(filename, std::ios::binary);
    file.write(reinterpret_cast<const char *>(image.data()), image.size());
    file.close();
}

int main(int argc, char *argv[])
{
    int image_width = 320;
    int image_height = 480;
    std::vector<uint8_t> image = generate_grayscale_image(image_width, image_height);
    std::vector<uint8_t> bmp = encode_grayscale_bmp(image, image_width, image_height);
    save_image_file("output.bmp", bmp);
    return 0;
}