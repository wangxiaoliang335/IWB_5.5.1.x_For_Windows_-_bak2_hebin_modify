#pragma once

//�¼���
class INewFrameEventSink
{
public:
     virtual void OnNewFrame(CImageFrame* pFrame) = 0;

};