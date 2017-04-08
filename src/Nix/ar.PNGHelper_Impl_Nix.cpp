﻿#include "ar.PNGHelper_Impl_Nix.h"

#define Z_SOLO
#include <png.h>
#include <pngstruct.h>
#include <pnginfo.h>


namespace ar
{
	static void PngReadData(png_structp png_ptr, png_bytep data, png_size_t length)
	{
		auto d = (uint8_t**)png_get_io_ptr(png_ptr);

		memcpy(data, *d, length);
		(*d) += length;
	}

	PNGHelper_Impl_Nix::PNGHelper_Impl_Nix()
	{
		tempBuffer1.resize(2048 * 2048 * 4);
	}

	PNGHelper_Impl_Nix::~PNGHelper_Impl_Nix()
	{

	}

	bool PNGHelper_Impl_Nix::Read(PNGLoadFunc readFunc, void* userData, const void* src, int32_t src_size)
	{
		uint8_t* data_ = (uint8_t*)src;

		/* pngアクセス構造体を作成 */
		png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

		/* リードコールバック関数指定 */
		png_set_read_fn(png, &data_, &PngReadData);

		/* png画像情報構造体を作成 */
		png_infop png_info = png_create_info_struct(png);

		/* エラーハンドリング */
		if (setjmp(png_jmpbuf(png)))
		{
			png_destroy_read_struct(&png, &png_info, NULL);
			return false;
		}

		/* IHDRチャンク情報を取得 */
		png_read_info(png, png_info);

		/* インターレース */
		auto number_of_passes = png_set_interlace_handling(png);
		if (number_of_passes == 0)
		{
			number_of_passes = 1;
		}

		/* RGBA8888フォーマットに変換する */
		if (png_info->bit_depth < 8)
		{
			png_set_packing(png);
		}
		else if (png_info->bit_depth == 16)
		{
			png_set_strip_16(png);
		}

		uint32_t pixelBytes = 4;
		switch (png_info->color_type)
		{
		case PNG_COLOR_TYPE_PALETTE:
		{
			png_set_palette_to_rgb(png);

			png_bytep trans_alpha = NULL;
			int num_trans = 0;
			png_color_16p trans_color = NULL;

			png_get_tRNS(png, png_info, &trans_alpha, &num_trans, &trans_color);
			if (trans_alpha != NULL)
			{
				pixelBytes = 4;
			}
			else
			{
				pixelBytes = 3;
			}
		}
		break;
		case PNG_COLOR_TYPE_GRAY:
			png_set_expand_gray_1_2_4_to_8(png);
			png_set_gray_to_rgb(png);
			pixelBytes = 3;
			break;
		case PNG_COLOR_TYPE_GRAY_ALPHA:
			png_set_gray_to_rgb(png);
			pixelBytes = 4;
			break;
		case PNG_COLOR_TYPE_RGB:
			pixelBytes = 3;
			break;
		case PNG_COLOR_TYPE_RGBA:
			break;
		}

		uint8_t* image = new uint8_t[png_info->width * png_info->height * pixelBytes];
		uint32_t pitch = png_info->width * pixelBytes;

		// 読み込み
		if (rev)
		{
			png_bytepp rp;
			for (auto pass = 0; pass < number_of_passes; pass++)
			{
				for (uint32_t i = 0; i < png_info->height; i++)
				{
					png_read_row(png, &image[(png_info->height - 1 - i) * pitch], NULL);
				}
			}
		}
		else
		{
			for (auto pass = 0; pass < number_of_passes; pass++)
			{
				for (uint32_t i = 0; i < png_info->height; i++)
				{
					png_read_row(png, &image[i * pitch], NULL);
				}
			}
		}

		auto imagewidth = png_info->width;
		auto imageheight = png_info->height;
		tempBuffer1.resize(imagewidth * imageheight * 4);
		auto imagedst_ = tempBuffer1.data();

		if (pixelBytes == 4)
		{
			memcpy(imagedst_, image, imagewidth * imageheight * 4);
		}
		else
		{
			for (int32_t y = 0; y < imageheight; y++)
			{
				for (int32_t x = 0; x < imagewidth; x++)
				{
					auto src = (x + y * imagewidth) * 3;
					auto dst = (x + y * imagewidth) * 4;
					imagedst_[dst + 0] = image[src + 0];
					imagedst_[dst + 1] = image[src + 1];
					imagedst_[dst + 2] = image[src + 2];
					imagedst_[dst + 3] = 255;
				}
			}
		}

		readFunc(tempBuffer1.data(), imagewidth, imageheight, userData);

		SafeDeleteArray(image);
		png_destroy_read_struct(&png, &png_info, NULL);

		return true;
	}
}