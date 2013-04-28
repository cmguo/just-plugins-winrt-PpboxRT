#pragma once

#include <wrl/client.h>
#include <implements.h>

#include <Windows.Phone.Media.Capture.Native.h>
using namespace Windows::Phone::Media::Capture;

namespace PpboxRT
{

    class CameraCaptureSampleSink :
        public Microsoft::WRL::RuntimeClass<
        Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::RuntimeClassType::ClassicCom>,
        ICameraCaptureSampleSink>
    {
    public:
        CameraCaptureSampleSink()
            : capture_(NULL)
            , itrack_(UINT32(-1))
        {
        }

        void init(
            PPBOX_HANDLE capture, 
            UINT32 itrack);

        IFACEMETHODIMP_(void) OnSampleAvailable(
            ULONGLONG hnsPresentationTime,
            ULONGLONG hnsSampleDuration,
            DWORD cbSample,
            BYTE* pSample);

    private:
        PPBOX_HANDLE capture_;
        UINT32 itrack_;
    };

}
