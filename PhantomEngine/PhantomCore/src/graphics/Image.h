#pragma once
#include <cstring>
#include <iostream>

namespace Phantom {

    struct Image {
        uint32_t Width;//������
        uint32_t Height;//����߶�
        uint8_t* data;

		//---��λͼ��������������ڴ��ϵ�ѹ����ʽ�Լ��ڴ���뷽ʽ�й�
        uint32_t bitcount;//һ������ռ���ڴ��С��bit����
        uint32_t pitch; //ͼ�ε�һ��ռ���ڴ��С��byte����

        size_t  data_size;
        bool    compressed;
        bool    is_float;
        uint32_t compress_format;
        uint32_t mipmap_count;
        struct Mipmap {
            uint32_t Width;
            uint32_t Height;
            uint32_t pitch;
            size_t offset;
            size_t data_size;
        } mipmaps[10];

        Image() : Width(0),
            Height(0),
            data(nullptr),
            bitcount(0),
            pitch(0),
            data_size(0),
            compressed(false),
            is_float(false),
            mipmap_count(1)
        {
            std::memset(mipmaps, 0x00, sizeof(mipmaps));
        };
    };

    std::ostream& operator<<(std::ostream& out, const Image& image);
}
