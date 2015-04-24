#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS // disable fopen/fread warnings on windows
#endif

#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include "RgbBitmap.h"
#include "RgbaBitmap.h"
#include "PvrTcEncoder.h"
#include "PvrTcDecoder.h"

/*
 Test program for the PvrTcEncoder, it compresses and then decompresses an
 image, ensuring the end result is something reasonable.
 */

using namespace Javelin;

Bitmap *readTga(const char *filename) {
    FILE *fp = fopen(filename, "rb");

    fseek(fp, 0, SEEK_END);
    int fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    unsigned char header[18];
    fread(header, 18, 1, fp);

    int bpp = header[16];
    int w = header[12] | (header[13] << 8);
    int h = header[14] | (header[15] << 8);

    Bitmap *bitmap = NULL;
    if (bpp == 24) {
        RgbBitmap *rgb = new RgbBitmap(w, h);
        fread((void *) rgb->data, w * h * 3, 1, fp);
        bitmap = rgb;
    }
    else if (bpp == 32) {
        RgbaBitmap *rgba = new RgbaBitmap(w, h);
        fread((void *) rgba->data, w * h * 4, 1, fp);
        bitmap = rgba;
    }

    fclose(fp);

    return bitmap;
}

void writeTga(const char *filename, Bitmap *bitmap) {
    FILE *fp = fopen(filename, "wb");

    bool isRgb = dynamic_cast<RgbBitmap *>(bitmap) != NULL;

    unsigned char header[18];
    memset(header, 0, 18);
    header[2] = 2;
    header[12] = bitmap->width & 0xff;
    header[13] = (bitmap->width >> 8) & 0xff;
    header[14] = bitmap->height & 0xff;
    header[15] = (bitmap->height >> 8) & 0xff;
    header[16] = isRgb ? 24 : 32;

    fwrite(header, 18, 1, fp);

    int bytesPerPixel = isRgb ? 3 : 4;
    fwrite(bitmap->data, bitmap->width * bitmap->height * bytesPerPixel, 1, fp);

    fclose(fp);
}

int main(int argc, char **argv) {
    {
        Bitmap *bitmap = readTga("globe.tga");
        bool isRgb = dynamic_cast<RgbBitmap *>(bitmap) != NULL;

        const int size = bitmap->GetArea() / 2;

        // Write the texture prior to compression
        writeTga("globe_before.tga", bitmap);

        unsigned char *pvrtc = new unsigned char[size];
        memset(pvrtc, 0, size);

        if (isRgb) {
            RgbBitmap *rgb = static_cast<RgbBitmap *>(bitmap);
            ColorRgb<unsigned char> *data = rgb->GetData();
            PvrTcEncoder::EncodeRgb4Bpp(pvrtc, *rgb);
            PvrTcDecoder::DecodeRgb4Bpp(data, bitmap->GetSize(), pvrtc);
        }
        else {
            RgbaBitmap *rgb = static_cast<RgbaBitmap *>(bitmap);
            ColorRgba<unsigned char> *data = rgb->GetData();
            PvrTcEncoder::EncodeRgba4Bpp(pvrtc, *rgb);
            PvrTcDecoder::DecodeRgba4Bpp(data, bitmap->GetSize(), pvrtc);
        }

        // Write the texture post compression
        writeTga("globe_after.tga", bitmap);

        delete bitmap;
    }

    {
        Bitmap *bitmap = readTga("alpha.tga");
        bool isRgb = dynamic_cast<RgbBitmap *>(bitmap) != NULL;

        const int size = bitmap->GetArea() / 2;

        // Write the texture prior to compression
        writeTga("alpha_before.tga", bitmap);

        unsigned char *pvrtc = new unsigned char[size];
        memset(pvrtc, 0, size);

        if (isRgb) {
            RgbBitmap *rgb = static_cast<RgbBitmap *>(bitmap);
            ColorRgb<unsigned char> *data = rgb->GetData();
            PvrTcEncoder::EncodeRgb4Bpp(pvrtc, *rgb);
            PvrTcDecoder::DecodeRgb4Bpp(data, bitmap->GetSize(), pvrtc);
        }
        else {
            RgbaBitmap *rgb = static_cast<RgbaBitmap *>(bitmap);
            ColorRgba<unsigned char> *data = rgb->GetData();
            PvrTcEncoder::EncodeRgba4Bpp(pvrtc, *rgb);
            PvrTcDecoder::DecodeRgba4Bpp(data, bitmap->GetSize(), pvrtc);
        }

        // Write the texture post compression
        writeTga("alpha_after.tga", bitmap);

        delete bitmap;
    }

    return 0;
}
