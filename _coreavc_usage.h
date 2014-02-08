#pragma once
#include "avisynth.h"
#include "directshow_grabber.h"

const UINT WM_GRAPH_EVENT = WM_APP + 1;

enum ShowParameters2 {
    SP2_AVCVIEW         = 0x01,
    SP2_MVCVIEW         = 0x02,
	SP2_HORIZONTALSTACK = 0x04,
	SP2_SWAPVIEWS       = 0x08,
    SP2_MASK            = 0x0F
};

class SSIFSource2: public IClip {
    CSampleGrabber *main_grabber, *sub_grabber, *skip_grabber;
    VideoInfo vi, frame_vi;
    int params;
    int current_frame_number;
    IGraphBuilder *pGraph;
    CComQIPtr<IMediaEventEx> pEvent;
    CComQIPtr<IMediaControl> pControl;
	CComQIPtr<IMediaSeeking> pSeeking;
    HWND hWindow;
    REFERENCE_TIME tmDuration;

    void Clear();
    void Throw(const char* str) {
        Clear();
        throw str;
    }
    void DataToFrame(CSampleGrabber *grabber, PVideoFrame& vf, IScriptEnvironment* env, bool bSignal = true);
    void DropFrame(CSampleGrabber *grabber, IScriptEnvironment* env, bool bSignal = true);
    void ParseEvents();
	void SeekToFrame(int framenumber, bool& bMainSignal, IScriptEnvironment* env);
	void SeekToFrame(int framenumber, IScriptEnvironment* env) {
		bool bMainSignal = true;
		SeekToFrame(framenumber, bMainSignal, env);
	}
	bool SwapViewsDetect(const string& filename);
	int FrameCountDetect(const string& filename, IScriptEnvironment* env);
public:
	SSIFSource2(AVSValue& args, IScriptEnvironment* env);
    virtual ~SSIFSource2();

	static AVSValue __cdecl Create(AVSValue args, void* user_data, IScriptEnvironment* env);
    PVideoFrame WINAPI GetFrame(int n, IScriptEnvironment* env);

    bool WINAPI GetParity(int n) { return false; }
    void WINAPI GetAudio(void* buf, __int64 start, __int64 count, IScriptEnvironment* env) { /* empty */ }
    const VideoInfo& WINAPI GetVideoInfo() { return vi; }
    void WINAPI SetCacheHints(int cachehints, int frame_range) { /* empty */ }
};

// class for holding one frame
class FrameHolder: public IClip {
    PVideoFrame vf;
    const VideoInfo vi;
public:
    FrameHolder(const VideoInfo& vi, PVideoFrame& vf): vi(vi), vf(vf) {}
    PVideoFrame WINAPI GetFrame(int n, IScriptEnvironment* env) { return vf; }
    bool WINAPI GetParity(int n) { return false; }
    void WINAPI GetAudio(void* buf, __int64 start, __int64 count, IScriptEnvironment* env) { /* empty */ }
    const VideoInfo& WINAPI GetVideoInfo() { return vi; }
    void WINAPI SetCacheHints(int cachehints, int frame_range) { /* empty */ }
};

PClip ClipStack(IScriptEnvironment* env, PClip a, PClip b, bool horizontal = false);