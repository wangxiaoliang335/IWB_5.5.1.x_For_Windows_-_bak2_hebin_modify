#pragma once
//Ϊ�˼�������������������,����������2^ALOG_SCALE_SHIFT_SIZE������
//����������a,b
//�� c = (a<<ALOG_SCALE_SHIFT_SIZE) + (b<<ALOG_SCALE_SHIFT_SIZE)
//�� c = (a<<ALOG_SCALE_SHIFT_SIZE) - (b<<ALOG_SCALE_SHIFT_SIZE)
//�� c = (a<<ALOG_SCALE_SHIFT_SIZE) *(b<<ALOG_SCALE_SHIFT_SIZE),
//       ע������2^ALOG_SCALE_SHIFT_SIZE*2^ALOG_SCALE_SHIFT_SIZE  ���ܵ����������
//�� c = (a<<ALOG_SCALE_SHIFT_SIZE)/b


//�����Ƶ����㾫��
#define INT_SCALE_SHIFT_SIZE  4



//0x33AA = 13226
#define IRCUT_OFF 0x33AA

//0x44AA = 17578
#define IRCUT_ON  0x44AA


