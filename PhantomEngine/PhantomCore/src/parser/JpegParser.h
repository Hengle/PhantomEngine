#pragma once
#include <iostream>
#include <setjmp.h>
#include "ImageParser.h"
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
			printf("image_width    = %d\n", cinfo.image_width);
			printf("image_height   = %d\n", cinfo.image_height);
			printf("num_components = %d\n", cinfo.num_components);


			// ���ý�ѹ����,����Ŵ���С

		  // ������ѹ��jpeg_start_decompress
			jpeg_start_decompress(&cinfo);

			/* �����ͼ����Ϣ */
			printf("output_width   = %d\n", cinfo.output_width);
			printf("output_height  = %d\n", cinfo.output_height);
			printf("output_components = %d\n", cinfo.output_components);

			// ѭ������jpeg_read_scanlinesһ��һ�еĻ�ý�ѹ����

			jpeg_finish_decompress(&cinfo);

			jpeg_destroy_decompress(&cinfo);

			img.mipmaps[0].Width = img.Width;
			img.mipmaps[0].Height = img.Height;
			img.mipmaps[0].pitch = img.pitch;
			img.mipmaps[0].offset = 0;
			img.mipmaps[0].data_size = img.data_size;
			return img;
		}
	};
}
