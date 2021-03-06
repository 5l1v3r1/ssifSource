/*
 * (C) 2003-2006 Gabest
 * (C) 2006-2013 see Authors.txt
 *
 * This file is part of MPC-HC.
 *
 * MPC-HC is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * MPC-HC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#pragma once

#include "../BaseSplitter/BaseSplitter.h"
#include "MpegSplitterFile.h"
#ifndef NOSETTINGS
#	include "MpegSplitterSettingsWnd.h"
#endif
#include "ITrackInfo.h"

#define MpegSplitterName L"MPC MPEG Splitter"
#define MpegSourceName   L"MPC MPEG Source"

class __declspec(uuid("DC257063-045F-4BE2-BD5B-E12279C464F0"))
    CMpegSplitterFilter
    : public CBaseSplitterFilter
    , public IAMStreamSelect
#ifndef NOSETTINGS
    , public ISpecifyPropertyPages2
    , public IMpegSplitterFilter
#endif
{
    REFERENCE_TIME  m_rtStartOffset;
    bool            m_pPipoBimbo;
    CHdmvClipInfo   m_ClipInfo;

protected:
    CAutoPtr<CMpegSplitterFile> m_pFile;
    CComQIPtr<ITrackInfo> pTI;

    HRESULT CreateOutputs(IAsyncReader* pAsyncReader);
    void    ReadClipInfo(LPCOLESTR pszFileName);

    bool DemuxInit();
    void DemuxSeek(REFERENCE_TIME rt);
    bool DemuxLoop();
    bool BuildPlaylist(LPCTSTR pszFileName, CAtlList<CHdmvClipInfo::PlaylistItem>& files);
    bool BuildChapters(LPCTSTR pszFileName, CAtlList<CHdmvClipInfo::PlaylistItem>& PlaylistItems, CAtlList<CHdmvClipInfo::PlaylistChapter>& Items);

    HRESULT DemuxNextPacket(REFERENCE_TIME rtStartOffset);

    REFERENCE_TIME m_rtPlaylistDuration;

private:
    CString m_csAudioLanguageOrder, m_csSubtitlesLanguageOrder;
    bool m_useFastStreamChange, m_ForcedSub, m_AlternativeDuration;
    int m_nVC1_GuidFlag, m_AC3CoreOnly;
    CCritSec m_csProps;

public:
    CMpegSplitterFilter(LPUNKNOWN pUnk, HRESULT* phr, const CLSID& clsid = __uuidof(CMpegSplitterFilter));
    void SetPipo(bool bPipo) { m_pPipoBimbo = bPipo; };

    bool StreamIsTrueHD(const WORD pid);

    DECLARE_IUNKNOWN
    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void** ppv);
    STDMETHODIMP GetClassID(CLSID* pClsID);
    STDMETHODIMP Load(LPCOLESTR pszFileName, const AM_MEDIA_TYPE* pmt);

    // CBaseFilter

    STDMETHODIMP QueryFilterInfo(FILTER_INFO* pInfo);

    // IAMStreamSelect

    STDMETHODIMP Count(DWORD* pcStreams);
    STDMETHODIMP Enable(long lIndex, DWORD dwFlags);
    STDMETHODIMP Info(long lIndex, AM_MEDIA_TYPE** ppmt, DWORD* pdwFlags, LCID* plcid,
                      DWORD* pdwGroup, WCHAR** ppszName, IUnknown** ppObject, IUnknown** ppUnk);

    // ISpecifyPropertyPages2

    STDMETHODIMP GetPages(CAUUID* pPages);
    STDMETHODIMP CreatePage(const GUID& guid, IPropertyPage** ppPage);

    // IMpegSplitterFilter
    STDMETHODIMP Apply();

    STDMETHODIMP SetFastStreamChange(BOOL nValue);
    STDMETHODIMP_(BOOL) GetFastStreamChange();

    STDMETHODIMP SetForcedSub(BOOL nValue);
    STDMETHODIMP_(BOOL) GetForcedSub();

    STDMETHODIMP SetAudioLanguageOrder(WCHAR* nValue);
    STDMETHODIMP_(WCHAR*) GetAudioLanguageOrder();

    STDMETHODIMP SetSubtitlesLanguageOrder(WCHAR* nValue);
    STDMETHODIMP_(WCHAR*) GetSubtitlesLanguageOrder();

    STDMETHODIMP SetVC1_GuidFlag(int nValue);
    STDMETHODIMP_(int) GetVC1_GuidFlag();

    STDMETHODIMP SetTrueHD(int nValue);
    STDMETHODIMP_(int) GetTrueHD();

    STDMETHODIMP SetAlternativeDuration(BOOL nValue);
    STDMETHODIMP_(BOOL) GetAlternativeDuration();

    STDMETHODIMP_(int) GetMPEGType();
};

class __declspec(uuid("1365BE7A-C86A-473C-9A41-C0A6E82C9FA3"))
    CMpegSourceFilter : public CMpegSplitterFilter
{
public:
    CMpegSourceFilter(LPUNKNOWN pUnk, HRESULT* phr, const CLSID& clsid = __uuidof(CMpegSourceFilter));
};

class CMpegSplitterOutputPin : public CBaseSplitterOutputPin, protected CCritSec
{
    CAutoPtr<Packet> m_p;
    CAutoPtrList<Packet> m_pl;
    REFERENCE_TIME m_rtPrev, m_rtOffset, m_rtMaxShift;
    bool m_fHasAccessUnitDelimiters;
    bool m_bFilterDTSMA;
    bool DD_reset;
    bool m_bFlushed;
    int  m_type;

protected:
    HRESULT DeliverNewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate);
    HRESULT DeliverPacket(CAutoPtr<Packet> p);
    HRESULT DeliverEndFlush();

public:
    CMpegSplitterOutputPin(CAtlArray<CMediaType>& mts, LPCWSTR pName, CBaseFilter* pFilter, CCritSec* pLock, HRESULT* phr, int type);
    virtual ~CMpegSplitterOutputPin();
    STDMETHODIMP Connect(IPin* pReceivePin, const AM_MEDIA_TYPE* pmt);
    void SetMaxShift(REFERENCE_TIME rtMaxShift) { m_rtMaxShift = rtMaxShift; };
};
