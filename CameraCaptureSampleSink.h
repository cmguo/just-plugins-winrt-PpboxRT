#pragma once

#include <wrl/client.h>
#include <implements.h>

#include <Windows.Phone.Media.Capture.Native.h>
using namespace Windows::Phone::Media::Capture;

namespace PpboxRT
{
    
    ref class Capture;

    class CameraCaptureSampleSink :
        public Microsoft::WRL::RuntimeClass<
        Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::RuntimeClassType::ClassicCom>,
        ICameraCaptureSampleSink>
    {
    public:
        typedef void (*Callback_OnSampleAvailable)(
            Capture ^ capture, 
            UINT itrack,
            ULONGLONG hnsPresentationTime,
            ULONGLONG hnsSampleDuration,
            DWORD cbSample,
            BYTE* pSample);

        CameraCaptureSampleSink()
            : callback_(NULL)
            , itrack_(UINT32(-1))
        {
        }

        void init(
            Callback_OnSampleAvailable callback, 
            Capture ^ capture, 
            UINT itrack);

        void close();

        IFACEMETHODIMP_(void) OnSampleAvailable(
            ULONGLONG hnsPresentationTime,
            ULONGLONG hnsSampleDuration,
            DWORD cbSample,
            BYTE* pSample);

    private:
        Callback_OnSampleAvailable callback_;
        Capture ^ capture_;
        UINT itrack_;
    };

}
