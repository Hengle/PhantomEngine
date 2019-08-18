#pragma once
#include <iostream>
#include <setjmp.h>
#include "interface/ImageParser.h"
#include "jpeglib.h"

namespace Phantom {

	class JpegParser : implements ImageParser
	{
	public:
		virtual Image Parse(Buffer& buf)
		{
			Image img;
			struct jpeg_decompress_struct cinfo;
			struct jpeg_error_mgr jerr;
			// ����ͳ�ʼ��һ��decompression�ṹ��
			cinfo.err = jpeg_std_error(&jerr);
			jpeg_create_decompress(&cinfo);

			jpeg_mem_src(&cinfo, buf.GetData(),buf.GetDataSize());

			// ��jpeg_read_header���jpg��Ϣ
			jpeg_read_header(&cinfo, TRUE);
			/* Դ��Ϣ */
			/*printf("image_width    = %d\n", cinfo.image_width);
			printf("image_height   = %d\n", cinfo.image_height);*/

			// ���ý�ѹ����,����Ŵ���С
		  // ������ѹ��jpeg_start_decompress
			jpeg_start_decompress(&cinfo);

			/* �����ͼ����Ϣ */
			/*printf("output_width   = %d\n", cinfo.output_width);
			printf("output_height  = %d\n", cinfo.output_height);
			printf("output_components = %d\n", cinfo.output_components);*/

			//����һ�е�����buffer
			int row_stride = cinfo.output_width* cinfo.output_components;
			int buffer_height = 1;
			JSAMPARRAY buffer = (JSAMPARRAY)malloc(sizeof(JSAMPROW)*buffer_height);
			buffer[0] = (JSAMPROW)malloc(sizeof(JSAMPLE) * row_stride);
			img.bitcount = sizeof(JSAMPLE)* cinfo.output_components*8;
			img.pitch = row_stride;
			img.data_size = cinfo.output_width * cinfo.output_height * cinfo.output_components;
			img.data = new uint8_t[img.data_size];
			long counter = 0;
			// ѭ������jpeg_read_scanlinesһ��һ�еĻ�ý�ѹ����
			while (cinfo.output_scanline < cinfo.output_height) {
				jpeg_read_scanlines(&cinfo, buffer, 1);
				memcpy(img.data + counter, buffer[0], row_stride);
				counter += row_stride;
			}
				//setting
			img.Width = cinfo.output_width;
			img.Height = cinfo.output_height;

			//cinfo.data_precision;
		
			jpeg_finish_decompress(&cinfo);

			jpeg_destroy_decompress(&cinfo);

			img.mipmaps[0].Width = img.Width;
			img.mipmaps[0].Height = img.Height;
			img.mipmaps[0].pitch = img.pitch;
			img.mipmaps[0].offset = 0;
			img.mipmaps[0].data_size = img.data_size;


			if(img.bitcount == 24)  //24bit metal not supported.
			{
				int compNum = img.Width * img.Height;
				uint8_t * tempPtr  = img.data;
				img.data = new uint8_t[compNum * 4];
				std::memset(img.data, 0x50, compNum * 4);
				for( int i = 0; i < compNum; i++)
				{
					memcpy(img.data + i*4, tempPtr + i*3 , 3);
				}
				

				img.bitcount = 32;
                img.pitch = row_stride/3*4;
                img.data_size = img.data_size/3*4;
                img.mipmaps[0].pitch = img.pitch;
                img.mipmaps[0].offset = 0;
                img.mipmaps[0].data_size = img.data_size;

				delete tempPtr;
			}
			return img;
		}
	};
}
