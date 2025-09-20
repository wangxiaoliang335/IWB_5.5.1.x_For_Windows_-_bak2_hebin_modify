#pragma once
/* Type declarations */
#ifdef _MSC_VER
#include <basetsd.h>
typedef UINT8  uint8_t;
typedef UINT16 uint16_t;
typedef INT16  int16_t;
typedef UINT32 uint32_t;
#pragma warning( disable: 4799 )
#else
#include <stdint.h>
#endif

//ʮ�ּ���CPPʵ��
//3*3���ڼ��
extern "C" void cross_detect3(size_t w, size_t h, const uint8_t* image, uint8_t* response );

//5*5���ڼ��
extern "C" void cross_detect5(size_t w, size_t h, const uint8_t* image, uint8_t* response );

//5*5���ڼ��,������У��ͼ���߳�С�ڵ���11������ʱ��У���ɹ���
extern "C" void cross_detect5_improve(size_t w, size_t h, const uint8_t* image, uint8_t* response);

//ʮ�ּ���SIMDʵ��
//3*3���ڼ��
extern "C" void cross_detect3_sse2(size_t w, size_t h, const uint8_t* image, uint8_t* response);

//5*5���ڼ��
extern "C" void cross_detect5_sse2(size_t w, size_t h, const uint8_t* image, uint8_t* response);
