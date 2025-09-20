//@����:ʹ��MMXָ���������YUY2ͼƬ֡
//@����:pYUY2Src, ָ��YUY2Դ���ݵ�ָ��
//      pYUY2Mask�� ָ��YUY2����ͼ���ݵ�ָ��
//      nPixelCount����������
extern "C"
void  _declspec(naked) YUY2FrameMask_MMX(
    unsigned short* pYUY2Src,
    const unsigned short* pYUY2Mask,
    unsigned int nPixelCount)
{
    _asm
    {
        pushad
        finit
        mov edi, [esp + 4*8 + 4  ] ;//pYUY2,
        mov esi, [esp + 4*8 + 8  ] ;//pYUY2Mask;
        mov ecx, [esp + 4*8 + 12 ] ;//nPixelCount

_loop_Next4Pixels:
        movq mm0, [esi];//ԭͼ����
        add esi, 8

        movq mm1, [edi];//����ͼ����
        pand mm0, mm1  ;//

        movq [edi], mm0;//�������κ������
        add edi, 8


        sub ecx, 4
        jnz _loop_Next4Pixels
        emms;
        popad
        ret
    }

}


extern "C"
void  _declspec(naked) GrayFrameMask_MMX(
    unsigned short* pGraySrc,
    const unsigned short* pGrayMask,
    unsigned int nPixelCount)
{
    _asm
    {
        pushad
        finit
        mov edi, [esp + 4*8 + 4  ] ;//pGray,
        mov esi, [esp + 4*8 + 8  ] ;//pGrayMask;
        mov ecx, [esp + 4*8 + 12 ] ;//nPixelCount

_loop_Next8Pixels:
        movq mm0, [esi];//ԭͼ����
        add esi, 8

        movq mm1, [edi];//����ͼ����
        pand mm0, mm1  ;//

        movq [edi], mm0;//�������κ������
        add edi, 8


        sub ecx, 8
        jnz _loop_Next8Pixels
        emms;
        popad
        ret
    }

}


//@����:ʹ��SSEָ�YUY2ͼƬ֡ת��Ϊ1λλͼ
extern "C"
void _declspec(naked) YUY2ToBitFrame_SSE(
    const unsigned short* pYUY2,
    unsigned char* pBits,
    unsigned char cThreshold,
    unsigned int nPixelCount)
{
    static const unsigned char mask[8]={0xFF, 0x00,0xFF, 0x00,0xFF, 0x00,0xFF, 0x00};
    _asm
    {
        pushad
        finit
        mov esi, [esp + 4*8 + 4  ] ;//pYUY2,
        mov edi, [esp + 4*8 + 8  ] ;//pBits;
        mov eax, [esp + 4*8 + 12 ] ;//cThreshold
        mov ecx, [esp + 4*8 + 16 ] ;//nPixelCount


        mov  ah,  al;
        mov  bx,  ax;
        shl  eax, 16;
        mov  ax,  bx;

        //mm3 = 00 00 00 00 T T T T
        movd mm3, eax;

        //mm4 = 00 00 00 00 T T T T
        movd mm4, eax;

        //mm3 = T T T T T T T T
        //punpckldq a, b
        //Interleave the 32-bit value from the low half of a with the 32-bit value from the low half of b and take the least significant element from a.
        punpckldq mm3, mm4;



        movq mm0, mask;


_loop_Next32Pixels:
        //[Step 1].0~7 Bytes
        //mm1 = V1 Y3 U1 Y2  V0 Y1 U0 Y0
        movq mm1, [esi];

        //mm2 = V3 Y7 U3 Y6  V2 Y5 U2 Y4
        movq mm2, [esi+8]
        add esi, 16

        //mm1 = 00 Y3 00 Y2  00 Y1 00 Y0
        pand mm1, mm0;

        //mm2 = 00 Y7 00 Y6  00 Y5 00 Y4
        pand mm2, mm0;

        //mm1= Y7 Y6 Y5 Y4 Y3 Y2 Y1 Y0
        packuswb mm1, mm2;

        //�޷����ֽڱȽ�
        //�����ޱȽ�
        //min(val,T)
        pminub  mm1, mm3
        
        //min(val, T) == T
        pcmpeqb mm1, mm3


        pmovmskb eax, mm1
        mov ebx,eax;




        //[Step 2].8~15 Bytes
        //mm1 = V1 Y3 U1 Y2  V0 Y1 U0 Y0
        movq mm1, [esi];

        //mm2 = V3 Y7 U3 Y6  V2 Y5 U2 Y4
        movq mm2, [esi+8]
        add esi, 16

        //mm1 = 00 Y3 00 Y2  00 Y1 00 Y0
        pand mm1, mm0;

        //mm2 = 00 Y7 00 Y6  00 Y5 00 Y4
        pand mm2, mm0;

        //mm1= Y7 Y6 Y5 Y4 Y3 Y2 Y1 Y0
        packuswb mm1, mm2;

        //�޷����ֽڱȽ�
        //�����ޱȽ�
        //min(val,T)
        pminub  mm1, mm3
        //min(val, T) == T
        pcmpeqb mm1, mm3

        pmovmskb eax, mm1
        shl eax, 8;
        or ebx, eax



        //[Step 3] 16~23 Bytes
        //mm1 = V1 Y3 U1 Y2  V0 Y1 U0 Y0
        movq mm1, [esi];

        //mm2 = V3 Y7 U3 Y6  V2 Y5 U2 Y4
        movq mm2, [esi+8]
        add esi, 16

        //mm1 = 00 Y3 00 Y2  00 Y1 00 Y0
        pand mm1, mm0;

        //mm2 = 00 Y7 00 Y6  00 Y5 00 Y4
        pand mm2, mm0;


        //mm1= Y7 Y6 Y5 Y4 Y3 Y2 Y1 Y0
        packuswb mm1, mm2;


        //�޷����ֽڱȽ�
        //�����ޱȽ�
        //min(val,T)
        pminub  mm1, mm3
        //min(val, T) == T
        pcmpeqb mm1, mm3

        pmovmskb eax, mm1
        shl eax, 16
        or ebx, eax


        //[Step 4] 16~23 Bytes
        //mm1 = V1 Y3 U1 Y2  V0 Y1 U0 Y0
        movq  mm1, [esi];

        //mm2 = V3 Y7 U3 Y6  V2 Y5 U2 Y4
        movq mm2, [esi+8]
        add esi, 16


        //mm1 = 00 Y3 00 Y2  00 Y1 00 Y0
        pand mm1, mm0;

        //mm2 = 00 Y7 00 Y6  00 Y5 00 Y4
        pand mm2, mm0;

        //mm1= Y7 Y6 Y5 Y4 Y3 Y2 Y1 Y0
        packuswb mm1, mm2;

        //�޷����ֽڱȽ�
        //�����ޱȽ�
        //min(val,T)
        pminub  mm1, mm3
        //min(val, T) == T
        pcmpeqb mm1, mm3
        pmovmskb eax, mm1

        shl eax, 24
        or ebx, eax


        //������
        mov [edi], ebx;

        add edi, 4

        sub ecx, 32;

        jnz _loop_Next32Pixels

        emms;
        popad
        ret

    }
}




//@������:DynamicMaskStatistic_MMX
//@��  ��:ͳ��ÿ�����ش���ĳһ���޵Ĵ�����������ش������������ض�Ӧ�ļ�����+1, ���С�����ޣ��������-1���������ķ�Χ��0~255
//
//ʵ�ֵ��߼���
//if(*pImageData >= cCmpThreshold��
//{
//  if(* pStatistic<0xFF) *pStatistic + = 0x01;
//}
//else
//{
//	 if(* pStatistic>0) *pStatistic -= 0x01;
//}
extern "C"
void _declspec(naked) DynamicMaskStatistic_YUY2_MMX(
                                        const unsigned short* pImageData,//YUY2��ʽ��λͼ
                                        unsigned char* pStatistic       ,//ͳ��λͼ
                                        unsigned char cCmpTheshold      ,//�Ƚ�����
                                        int nPixelCount                  //��������
                                        )
{

    static const unsigned char mask[8]={0xFF, 0x00,0xFF, 0x00,0xFF, 0x00,0xFF, 0x00};
    _asm
    {

        pushad

       //reset the floating-point unit
       finit

        mov esi, [esp + 4*8 + 4*1];//YUY2��ʽ��λͼ
        mov edi, [esp + 4*8 + 4*2];//ÿ�����صļ�������һ�����ض�Ӧһ��16λ�ļ�����
        mov eax, [esp + 4*8 + 4*3];//����
        mov ecx, [esp + 4*8 + 4*4];//���ظ���


        //mm7�д�űȽ�����
        //mm7 = T T T T T T T T
        movd mm7, eax;
        punpcklbw mm7, mm7;
        punpcklbw mm7, mm7;
        punpcklbw mm7, mm7;


        //mm6�д�ż���������
        //01 01 01 01 01 01 01 01
        mov eax,0x01;
        ;//mm6 = XX XX XX XX XX XX XX 01
        movd mm6, eax;

        ;//mm6 = XX XX XX XX XX XX 01 01
        punpcklbw mm6, mm6;

        ;//mm6 = XX XX XX XX 01 01 01 01
        punpcklbw mm6, mm6;

        ;//mm6 =  01 01 01 01 01 01 01 01
        punpcklbw mm6, mm6;


        movq mm5, mask 

_loop_next_8_pixels:

        //1.��ȡ8�����ص�����ֵ
        //mm0 =  V1 Y3 U1 Y2 V0 Y1 U0 Y0
        movq mm0, [esi]

        //mm1 =  V3 Y7 U3 Y6 V2 Y5 U2 Y4
        movq mm1, [esi+8];

        //
        add esi, 16

        //mm0 = 00 Y3 00 Y2 00 Y1 00 Y0
        pand mm0, mm5;

        //mm1 = 00 Y7 00 Y6 00 Y5 00 Y4
        pand mm1, mm5;

        //mm1 = Y7 Y6 Y5 Y4 Y3 Y2 Y1 Y0
        packuswb mm0, mm1


        //2.����ÿ���ֽڵ�������
       //�޷��űȽ�,ע�ⲻ�ܹ�ʹ��pcmpgtb
       //mm0 >= mm7�ж�
       //mm0���min(mm0,mm7)
       pminub  mm0, mm7
       //min(mm0[i], mm7[i]) == mm7[i],��ζmm0[i]�Ŵ�������T
       pcmpeqb mm0, mm7

       //�ȽϽ������������, mm0[i]������T, ��mm1[i] == 0xFF
       movq mm1, mm0

       //��������������+1����+0��
       //mm0= 01 00 01 01 01 00  00 10
       pand mm1, mm6

       //ͳ������ͼ
       //mm1 = Counter[Y7] Counter[Y6] Counter[Y5] Counter[Y4] Counter[Y3] Counter[Y2] Counter[Y1] Counter[Y0]
       movq mm2, [edi];


      //Subtract packed unsigned 8-bit integers in b from a, and store unsigned satured results in dst
      //����ֵ��������T��������ͳ������ͼ�еļ���+1
      paddusb mm2, mm1


      //3.
      //mm1 = FF FF FF FF FF FF FF FF
      //��mm18�ֽ�ȫΪ0xFF
      pcmpeqb mm1,mm1

      //������ȡ������<����T, ���Ӧ��������ΪFF
      pxor mm0, mm1

      //����ÿ��������-1����-0��
      pand mm0, mm6

      //Subtract packed unsigned 8-bit integers in b from a, and store unsigned satured results in dst
      //����ֵС������T��������ͳ������ͼ�еļ���-1��>=T, ��������䡣
      psubusb mm2, mm0;

      movq [edi], mm2;

      add edi, 8

      sub ecx, 8

      jnz _loop_next_8_pixels;//����δ���������������8�����ء�


      emms
      popad
      ret 

    }

}


extern "C"
void _declspec(naked) DynamicMaskStatistic_Gray_MMX(
                                        const unsigned short* pImageData,//8Bit�Ҷȸ�ʽ��λͼ
                                        unsigned char* pStatistic       ,//ͳ��λͼ
                                        unsigned char cCmpTheshold      ,//�Ƚ�����
                                        int nPixelCount                  //��������
                                        )
{

    _asm
    {

        pushad

       //reset the floating-point unit
       finit

        mov esi, [esp + 4*8 + 4*1];//8Bit�Ҷȸ�ʽ��λͼ
        mov edi, [esp + 4*8 + 4*2];//ÿ�����صļ�������һ�����ض�Ӧһ��16λ�ļ�����
        mov eax, [esp + 4*8 + 4*3];//����
        mov ecx, [esp + 4*8 + 4*4];//���ظ���


        //mm7�д�űȽ�����
        //mm7 = T T T T T T T T
        movd mm7, eax;
        punpcklbw mm7, mm7;
        punpcklbw mm7, mm7;
        punpcklbw mm7, mm7;


        //mm6�д�ż���������
        //01 01 01 01 01 01 01 01
        mov eax,0x01;
        ;//mm6 = XX XX XX XX XX XX XX 01
        movd mm6, eax;

        ;//mm6 = XX XX XX XX XX XX 01 01
        punpcklbw mm6, mm6;

        ;//mm6 = XX XX XX XX 01 01 01 01
        punpcklbw mm6, mm6;

        ;//mm6 =  01 01 01 01 01 01 01 01
        punpcklbw mm6, mm6;


_loop_next_8_pixels:

        //1.��ȡ8�����ص�����ֵ
        //mm0 =  Y7 Y6 Y5 Y4 Y3 Y2 Y1 Y0
        movq mm0, [esi]



        //2.����ÿ���ֽڵ�������
       //�޷��űȽ�,ע�ⲻ�ܹ�ʹ��pcmpgtb
       //mm0 >= mm7�ж�
       //mm0���min(mm0,mm7)
       pminub  mm0, mm7
       //min(mm0[i], mm7[i]) == mm7[i],��ζmm0[i]�Ŵ�������T
       pcmpeqb mm0, mm7

       //�ȽϽ������������, mm0[i]������T, ��mm1[i] == 0xFF
       movq mm1, mm0

       //��������������+1����+0��
       //mm0= 01 00 01 01 01 00  00 10
       pand mm1, mm6

       //ͳ������ͼ
       //mm1 = Counter[Y7] Counter[Y6] Counter[Y5] Counter[Y4] Counter[Y3] Counter[Y2] Counter[Y1] Counter[Y0]
       movq mm2, [edi];


      //Subtract packed unsigned 8-bit integers in b from a, and store unsigned satured results in dst
      //����ֵ��������T��������ͳ������ͼ�еļ���+1
      paddusb mm2, mm1


      //3.
      //mm1 = FF FF FF FF FF FF FF FF
      //��mm18�ֽ�ȫΪ0xFF
      pcmpeqb mm1,mm1

      //������ȡ������<����T, ���Ӧ��������ΪFF
      pxor mm0, mm1

      //����ÿ��������-1����-0��
      pand mm0, mm6

      //Subtract packed unsigned 8-bit integers in b from a, and store unsigned satured results in dst
      //����ֵС������T��������ͳ������ͼ�еļ���-1��>=T, ��������䡣
      psubusb mm2, mm0;

      movq [edi], mm2;

      add edi, 8
      add esi, 8

      sub ecx, 8

      jnz _loop_next_8_pixels;//����δ���������������8�����ء�


      emms
      popad
      ret 

    }

}

//@����:ʹ��SSE2ָ�8λ�Ҷ�ͼƬ֡ת��Ϊ1λλͼ
//@����:pGray, �������,ָ��8λ�Ҷ�ͼƬ���׵�ַ
//      pBits, �������,
extern "C"
void _declspec(naked) GrayToBitFrame_SSE2(
    const unsigned char* pGray,
    unsigned char* pBits,
    unsigned char cThreshold,
    unsigned int nPixelCount)
{
    _asm
    {
        pushad
        finit
        mov esi, [esp + 4*8 + 4  ] ;//pGray,
        mov edi, [esp + 4*8 + 8  ] ;//pBits;
        mov eax, [esp + 4*8 + 12 ] ;//cThreshold
        mov ecx, [esp + 4*8 + 16 ] ;//nPixelCount


		//set threshold in xmm0
        mov  ah,  al;
        mov  bx,  ax;
        shl  eax, 16;
        mov  ax,  bx;

        //xmm0 = 00 00 00 00 00 00 00 00 00 00 00 00 T T T T
        //Here, T is the threhold
        movd xmm0, eax;

        //xmm0 = 00 00 00 00 00 00 00 00 00 00 00 00 T T T T
        movdqa xmm1, xmm0

        //xmm0 = 00 00 00 00 00 00 00 00 T T T T T T T T
        punpckldq xmm0, xmm1;

        //xmm1 = 00 00 00 00 00 00 00 00 T T T T T T T T
        movdqa xmm1, xmm0

        //xmm0 = T T T T T T T T T T T T T T T T T T T
        punpckldq xmm0, xmm1;


_loop_Next32Pixels:
        movdqa xmm1, [esi]


         //�ж��Ƿ�">="����T,
        //ע�ⲻ��ʹʹ���з��ŵıȽ�ָ��pcmpgtb
        //�޷����ֽڱȽ�
        //�����ޱȽ�
        //xmm1 = min(val,T)
        pminub  xmm1, xmm0
        
        //�ж�min(val, T) == T
        //xmm1��"==T"���ֽ�,����ΪFF
        pcmpeqb xmm1, xmm0

        //eax�ĵ�16λ��xmm1��ÿ���ֽڵĵ�8λ��ɡ�
        pmovmskb eax, xmm1

        mov ebx,eax;


        add esi, 16
        movdqa xmm1, [esi]


        //�޷����ֽڱȽ�
        //�����ޱȽ�
        //ע�ⲻ��ʹʹ���з��ŵıȽ�ָ��pcmpgtb
        //xmm1 = min(val,T)
        pminub  xmm1, xmm0
        
        //�ж�min(val, T) == T
        //xmm1��"==T"���ֽ�,����ΪFF
        pcmpeqb xmm1, xmm0

        //eax�ĵ�16λ��xmm1��ÿ���ֽڵĵ�8λ��ɡ�
        pmovmskb eax, xmm1

        //����eax��16λ
        shl eax, 16

        //��16λ�͵�16λ�ϲ���
        or ebx,eax;

        add esi, 16


        //������
        mov [edi], ebx;

        add edi, 4

        sub ecx, 32;

        jnz _loop_Next32Pixels

        emms;
        popad
        ret

    }
}


//@����:ʹ��SSE2ָ�8λ�Ҷ�ͼƬ֡��ֵ��,Ŀ������ֵΪ0xFF, ��������ֵΪ0x00, ת��Ϊ1λλͼ
//@����:pGray, ����/�������,����ָ��8λ�Ҷ�ͼƬ���׵�ַ,��������ֵ����Ľ��
//      pBits, �������,1λλͼ
//     cThreshold, ��ֵ������
//    nPixelCount, ��������
extern "C"
void _declspec(naked) BinarizeGrayToBitFrame_SSE2(
    unsigned char* pGray,
    unsigned char* pBits,
    unsigned char cThreshold,
    unsigned int nPixelCount)
{
    _asm
    {
        pushad
        finit
        mov esi, [esp + 4*8 + 4  ] ;//pGray,
        mov edi, [esp + 4*8 + 8  ] ;//pBits;
        mov eax, [esp + 4*8 + 12 ] ;//cThreshold
        mov ecx, [esp + 4*8 + 16 ] ;//nPixelCount


        //set threshold in xmm0
        mov  ah,  al;
        mov  bx,  ax;
        shl  eax, 16;
        mov  ax,  bx;

        //xmm0 = 00 00 00 00 00 00 00 00 00 00 00 00 T T T T
        //Here, T is the threhold
        movd xmm0, eax;

        //xmm0 = 00 00 00 00 00 00 00 00 00 00 00 00 T T T T
        movdqa xmm1, xmm0

        //xmm0 = 00 00 00 00 00 00 00 00 T T T T T T T T
        punpckldq xmm0, xmm1;

        //xmm1 = 00 00 00 00 00 00 00 00 T T T T T T T T
        movdqa xmm1, xmm0

        //xmm0 = T T T T T T T T T T T T T T T T T T T
        punpckldq xmm0, xmm1;


_loop_Next32Pixels:
        movdqa xmm1, [esi]


        //�ж��Ƿ�">="����T,
        //ע�ⲻ��ʹʹ���з��ŵıȽ�ָ��pcmpgtb
        //�޷����ֽڱȽ�
        //�����ޱȽ�
        //xmm1 = min(val,T)
        pminub  xmm1, xmm0
        
        //�ж�min(val, T) == T
        //xmm1��"== T"���ֽ�,����ΪFF
        pcmpeqb xmm1, xmm0

        //��ֵ�����ݻ�д�Ҷ�֡
        //2016/11/17
        movdqa [esi], xmm1;

        //eax�ĵ�16λ��xmm1��ÿ���ֽڵĵ�8λ��ɡ�
        pmovmskb eax, xmm1

        mov ebx,eax;


        add esi, 16

        movdqa xmm1, [esi]


        //�޷����ֽڱȽ�
        //�����ޱȽ�
        //ע�ⲻ��ʹʹ���з��ŵıȽ�ָ��pcmpgtb
        //xmm1 = min(val,T)
        pminub  xmm1, xmm0
        
        //�ж�min(val, T) == T
        //xmm1��"==T"���ֽ�,����ΪFF
        pcmpeqb xmm1, xmm0

        //��ֵ�����ݻ�д�Ҷ�֡
        //2016/11/17
        movdqa [esi], xmm1;

        //eax�ĵ�16λ��xmm1��ÿ���ֽڵĵ�8λ��ɡ�
        pmovmskb eax, xmm1

        //����eax��16λ
        shl eax, 16

        //��16λ�͵�16λ�ϲ���
        or ebx,eax;

        add esi, 16


        //������
        mov [edi], ebx;

        add edi, 4

        sub ecx, 32;

        jnz _loop_Next32Pixels

        emms;
        popad
        ret

    }
}





//@����:ʹ��SSE2ָ�1λλͼ֡ת��Ϊ8λ�Ҷ�ͼƬ
//@����:pGray, �������,ָ��8λ�Ҷ�ͼƬ���׵�ַ
//      pBits, �������,
extern "C"
void _declspec(naked) BitToGrayFrame_MMX(
    const unsigned char* pBits,
    unsigned char* pGray,
    unsigned int nPixelCount)
{

    _asm
    {
        pushad
        finit
        mov esi, [esp + 4*8 + 4 ] ;//pBits
        mov edi, [esp + 4*8 + 8 ] ;//pGray
        mov ecx, [esp + 4*8 + 12] ;//nPixelCount

        ;//prepare dqword  mask = 0x80 0x40 0x20 0x10 0x08 0x04 0x02 0x01
        mov eax, 08040201h
        movd mm1, eax
       

        ;//eax=80402010h
        mov eax, 80402010h
        movd mm0, eax
        psllq mm0,32 ;//mm0 = 0x80 0x40 0x20 0x10 00 00 00 00 00 00 00 00
        ;//mm1= 0x80 0x40 0x20 0x10 0x08 0x04 0x02 0x01
        por mm1, mm0;

        
_loop_next_8_bits:
        xor eax, eax ;
        mov al, [esi]      ;//eax = 00 00 00 DD
        movd mm0, eax      ;//mm0 = 00 00 00 00 00 00 00 DD
        punpcklbw mm0, mm0 ;//mm0 = 00 00 00 00 00 00 DD DD
        pshufw mm0, mm0, 0 ;//mm0 = DD DD DD DD DD DD DD DD
        
        //DD DD DD DD DD DD DD DD & 8040201008040201
        pand mm0,mm1      ;

        //
        pcmpeqb mm0, mm1  ;//
        movq [edi], mm0  ;//store the result in memory
        add edi, 8;
        inc esi   ;
        sub ecx, 8;
        jnz _loop_next_8_bits;//next bytes

        emms
        popad;
        ret
    }

}

//@����:��8λ�Ҷ�ͼƬת��ΪARGB��ʽ
//@����:
//     p8BitGrayData, �Ҷ�ͼƬ����
//      pARGBData, ARGB����
//      pixelCount, ���ظ���
//      
extern "C"
void _declspec(naked) GrayToARGB_SSE2(
        const unsigned char* p8BitGrayData, 
        unsigned long* pARGBData, 
        unsigned long pixelCount)
{

 __declspec( align( 16) ) static const unsigned char AlphaCode[16] =
 {
    0x00,0x00,0x00,0xFF,
    0x00,0x00,0x00,0xFF,
    0x00,0x00,0x00,0xFF,
    0x00,0x00,0x00,0xFF
 };

    _asm
    {
        pushad
        finit

        mov esi, [esp + 4*8 + 4 ] ;//p8BitGrayData
        mov edi, [esp + 4*8 + 8 ] ;//pARGBData
        mov ecx, [esp + 4*8 + 12] ;//nPixelCount

        ;//xxm7 = FF 00 00 00 FF 00 00 00 FF 00 00 00 FF 00 00 00
        movdqa xmm7, AlphaCode ;//����


_next_16_pixles:

        ;//��ȡ16�����صĻҶ�ֵ
        ;//xmm0 = PF PE PD PC PB PA P9 P8 P7 P6 P5 P4 P3 P2 P1 P0
        movdqa xmm0,[esi];

        //------0~7����-------
        ;//�Ҷ�����
        ;//xmm1 = PF PE PD PC PB PA P9 P8 P7 P6 P5 P4 P3 P2 P1 P0
        movdqa xmm1, xmm0


        ;//��չ0~7����Ϊ��
        ;//xmm1 = P7P7 P6P6 P5P5 P4P4 P3P3 P2P2 P1P1 P0P0
        punpcklbw xmm1, xmm1

        ;//�Ҷ�����
        movdqa xmm2, xmm1

        ;//0~3������չΪ˫��
        ;//xmm2 = P3P3P3P3 P2P2P2P2 P1P1P1P1 P0P0P0P0
        punpcklbw xmm2, xmm2
       
        ;//����0~3���ص�Alphaֵ
        por xmm2, xmm7

        ;//����0~3���ص�ARGB����
        movdqa [edi], xmm2;


        ;//4~7������չΪ˫��
        ;//xmm1 = P7P7P7P7 P6P6P6P6 P5P5P5P5 P4P4P4P4
        punpckhbw xmm1,xmm1

        ;//����4~7���ص�Alphaֵ
        por xmm1, xmm7

        ;//����4~7���ص�ARGB����
        movdqa [edi + 16], xmm1;


        ;//----8~11����----
        ;//8~11������չΪ��
        ;//xmm0 = PFPF PEPE PDPD PCPC PBPB PAPA P9P9 P8P8
        punpckhbw xmm0, xmm0

        ;//xmm1 = PFPF PEPE PDPD PCPC PBPB PAPA P9P9 P8P8
        movdqa xmm1, xmm0;


        ;//8~11������չΪ˫��
        ;//xmm1 = PBPBPBPB PAPAPAPA P9P9P9P9 P8P8P8P8
        punpcklbw xmm1,xmm1


        ;//����8~11���ص�Alphaֵ
        por xmm1, xmm7

        ;//����8~11���ص�ARGB����
        movdqa [edi + 32], xmm1;//
        
        ;//12~15����
        ;//12~15������չΪ˫��
        ;//xmm0 = PFPFPFPF PEPEPEPE PDPDPDPD PCPCPCPC
        punpckhbw xmm0,xmm0


        ;//����12~15���ص�Alphaֵ
        por xmm0, xmm7
        ;//����12~15���ص�ARGB����
        movdqa [edi + 48], xmm0;//����ARGB����

        add esi, 16
        add edi,64
        sub ecx,16
        jnz _next_16_pixles;
        emms
        popad
        ret

    }

}