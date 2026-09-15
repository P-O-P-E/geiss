#pragma once
// WASAPI capture stays on its own COM thread; rendering reads a bounded snapshot.
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <wrl/client.h>
#include <thread>
#include <mutex>
#include <atomic>
#include <array>
class StandaloneAudio {
    std::thread worker;
    std::atomic<bool> stopping{false};
    std::mutex mutex;
    std::array<short,16384> samples{};
    size_t cursor=0;
    ULONGLONG lastPacket=0;
public:
    std::atomic<HRESULT> status{S_OK};
    ~StandaloneAudio() { stop(); }
    void stop() {
        stopping=true;
        if(worker.joinable()) worker.join();
        std::lock_guard<std::mutex> lock(mutex);
        samples.fill(0); cursor=0; lastPacket=0;
    }
    void start(bool microphone) {
        stop(); stopping=false; status=S_OK;
        worker=std::thread([this,microphone] {
            HRESULT hr=CoInitializeEx(nullptr,COINIT_MULTITHREADED);
            if(FAILED(hr)) { status=hr; return; }
            {
                using Microsoft::WRL::ComPtr;
                ComPtr<IMMDeviceEnumerator> enumerator;
                ComPtr<IMMDevice> device;
                ComPtr<IAudioClient> client;
                ComPtr<IAudioCaptureClient> capture;
                hr=CoCreateInstance(__uuidof(MMDeviceEnumerator),nullptr,CLSCTX_ALL,IID_PPV_ARGS(&enumerator));
                if(SUCCEEDED(hr)) hr=enumerator->GetDefaultAudioEndpoint(microphone?eCapture:eRender,eConsole,&device);
                if(SUCCEEDED(hr)) hr=device->Activate(__uuidof(IAudioClient),CLSCTX_ALL,nullptr,&client);
                WAVEFORMATEX format={WAVE_FORMAT_PCM,2,44100,176400,4,16,0};
                DWORD flags=AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM;
                if(!microphone) flags|=AUDCLNT_STREAMFLAGS_LOOPBACK;
                if(SUCCEEDED(hr)) hr=client->Initialize(AUDCLNT_SHAREMODE_SHARED,flags,1000000,0,&format,nullptr);
                if(SUCCEEDED(hr)) hr=client->GetService(IID_PPV_ARGS(&capture));
                if(SUCCEEDED(hr)) hr=client->Start();
                bool started=SUCCEEDED(hr);
                while(SUCCEEDED(hr) && !stopping) {
                    UINT32 frames=0;
                    hr=capture->GetNextPacketSize(&frames);
                    while(SUCCEEDED(hr) && frames && !stopping) {
                        BYTE* data=nullptr; DWORD packetFlags=0;
                        hr=capture->GetBuffer(&data,&frames,&packetFlags,nullptr,nullptr);
                        if(FAILED(hr)) break;
                        {
                            std::lock_guard<std::mutex> lock(mutex);
                            if(packetFlags&AUDCLNT_BUFFERFLAGS_DATA_DISCONTINUITY) samples.fill(0);
                            const short* pcm=reinterpret_cast<const short*>(data);
                            for(size_t i=0;i<size_t(frames)*2;++i) {
                                samples[cursor]=(packetFlags&AUDCLNT_BUFFERFLAGS_SILENT)?0:pcm[i];
                                cursor=(cursor+1)%samples.size();
                            }
                            lastPacket=GetTickCount64();
                        }
                        hr=capture->ReleaseBuffer(frames);
                        if(SUCCEEDED(hr)) hr=capture->GetNextPacketSize(&frames);
                    }
                    if(SUCCEEDED(hr)) Sleep(5);
                }
                if(started) client->Stop();
                status=hr;
            }
            CoUninitialize();
        });
    }
    void read(short* out,size_t count) {
        std::lock_guard<std::mutex> lock(mutex);
        if(count>samples.size()) count=samples.size();
        bool fresh=lastPacket && GetTickCount64()-lastPacket<150;
        for(size_t i=0;i<count;++i) out[i]=fresh?samples[(cursor+samples.size()-count+i)%samples.size()]:0;
    }
};
