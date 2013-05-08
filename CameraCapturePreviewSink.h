#pragma once

#include <wrl/client.h>
#include <implements.h>

#include <Windows.Phone.Media.Capture.Native.h>
using namespace Windows::Phone::Media::Capture;

namespace PpboxRT
{

    ref class Capture;

    class CameraCapturePreviewSink :
        public Microsoft::WRL::RuntimeClass<
        Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::RuntimeClassType::ClassicCom>,
        ICameraCapturePreviewSink>
    {
    public:
        typedef void (*Callback_OnFrameAvailable)(
            Capture ^ capture, 
            DXGI_FORMAT format,
            UINT width,
            UINT height,
            BYTE* pixels);

        CameraCapturePreviewSink()
            : callback_(NULL)
            , pixels_(NULL)
        {
        }

        void init(
            Callback_OnFrameAvailable callback, 
            Capture ^ capture);

        void close();

        IFACEMETHODIMP_(void) OnFrameAvailable(
            DXGI_FORMAT format,
            UINT width,
            UINT height,
            BYTE* pixels);

        BYTE* pixels() const
        {
            return pixels_;
        }

    private:
        Callback_OnFrameAvailable callback_;
        Capture ^ capture_;
        BYTE* pixels_;
    };

}
