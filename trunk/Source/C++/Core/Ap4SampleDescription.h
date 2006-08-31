/*****************************************************************
|
|    AP4 - Sample Descriptions
|
|    Copyright 2002-2005 Gilles Boccon-Gibod & Julien Boeuf
|
|
|    This file is part of Bento4/AP4 (MP4 Atom Processing Library).
|
|    Unless you have obtained Bento4 under a difference license,
|    this version of Bento4 is Bento4|GPL.
|    Bento4|GPL is free software; you can redistribute it and/or modify
|    it under the terms of the GNU General Public License as published by
|    the Free Software Foundation; either version 2, or (at your option)
|    any later version.
|
|    Bento4|GPL is distributed in the hope that it will be useful,
|    but WITHOUT ANY WARRANTY; without even the implied warranty of
|    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
|    GNU General Public License for more details.
|
|    You should have received a copy of the GNU General Public License
|    along with Bento4|GPL; see the file COPYING.  If not, write to the
|    Free Software Foundation, 59 Temple Place - Suite 330, Boston, MA
|    02111-1307, USA.
|
 ****************************************************************/

#ifndef _AP4_SAMPLE_DESCRIPTION_H_
#define _AP4_SAMPLE_DESCRIPTION_H_

/*----------------------------------------------------------------------
|   includes
+---------------------------------------------------------------------*/
#include "Ap4Types.h"
#include "Ap4Atom.h"
#include "Ap4EsDescriptor.h"
#include "Ap4EsdsAtom.h"

/*----------------------------------------------------------------------
|   class references
+---------------------------------------------------------------------*/
class AP4_SampleEntry;
class AP4_DataBuffer;

/*----------------------------------------------------------------------
|   AP4_SampleDescription
+---------------------------------------------------------------------*/
class AP4_SampleDescription
{
 public:
    // type constants of the sample description
    enum Type {
        TYPE_UNKNOWN  = 0x00,
        TYPE_MPEG     = 0x01,
        TYPE_ISMACRYP = 0x02
    };

    // constructors & destructor
    AP4_SampleDescription(Type type, AP4_UI32 format) : 
        m_Type(type), m_Format(format) {}
    virtual ~AP4_SampleDescription() {}
                 
    // accessors
    Type GetType()       const { return m_Type;   }
    AP4_UI32 GetFormat() const { return m_Format; }

    // factories
    virtual AP4_Atom* ToAtom() const = 0;

 protected:
    Type     m_Type;
    AP4_UI32 m_Format;
};

/*----------------------------------------------------------------------
|   AP4_AudioSampleDescription  // MIXIN class
+---------------------------------------------------------------------*/
class AP4_AudioSampleDescription
{
public:
    // constructor
    AP4_AudioSampleDescription(unsigned int sample_rate,
                               unsigned int sample_size,
                               unsigned int channel_count) :
    m_SampleRate(sample_rate),
    m_SampleSize(sample_size),
    m_ChannelCount(channel_count) {}

    // accessors
    AP4_UI32 GetSampleRate()   { return m_SampleRate;   }
    AP4_UI16 GetSampleSize()   { return m_SampleSize;   }
    AP4_UI16 GetChannelCount() { return m_ChannelCount; }

protected:
    // members
    AP4_UI32 m_SampleRate;
    AP4_UI16 m_SampleSize;
    AP4_UI16 m_ChannelCount;
};

/*----------------------------------------------------------------------
|   AP4_VideoSampleDescription  // MIXIN class
+---------------------------------------------------------------------*/
class AP4_VideoSampleDescription
{
public:
    // constructor
    AP4_VideoSampleDescription(AP4_UI16    width,
                               AP4_UI16    height,
                               AP4_UI16    depth,
                               const char* compressor_name) :
    m_Width(width),
    m_Height(height),
    m_Depth(depth),
    m_CompressorName(compressor_name) {}

    // accessors
    AP4_UI32    GetWidth()          { return m_Width;   }
    AP4_UI16    GetHeight()         { return m_Height;   }
    AP4_UI16    GetDepth()          { return m_Depth; }
    const char* GetCompressorName() { return m_CompressorName.GetChars(); }

protected:
    // members
    AP4_UI16   m_Width;
    AP4_UI16   m_Height;
    AP4_UI16   m_Depth;
    AP4_String m_CompressorName;
};

/*----------------------------------------------------------------------
|   AP4_UnknownSampleDescription
+---------------------------------------------------------------------*/
class AP4_UnknownSampleDescription : public AP4_SampleDescription
{
public:
    // methods
    AP4_UnknownSampleDescription(AP4_UI32 format);
    ~AP4_UnknownSampleDescription();
    AP4_Atom* ToAtom() const;
};

/*----------------------------------------------------------------------
|   AP4_GenericAudioSampleDescription
+---------------------------------------------------------------------*/
class AP4_GenericAudioSampleDescription : public AP4_UnknownSampleDescription,
                                          public AP4_AudioSampleDescription
{
public:
    // constructor
    AP4_GenericAudioSampleDescription(AP4_UI32     format,
                                      unsigned int sample_rate,
                                      unsigned int sample_size,
                                      unsigned int channel_count) :
    AP4_UnknownSampleDescription(format),
    AP4_AudioSampleDescription(sample_rate, sample_size, channel_count) {}
};

/*----------------------------------------------------------------------
|   AP4_GenericVideoSampleDescription
+---------------------------------------------------------------------*/
class AP4_GenericVideoSampleDescription : public AP4_UnknownSampleDescription,
                                          public AP4_VideoSampleDescription
{
public:
    // constructor
    AP4_GenericVideoSampleDescription(AP4_UI32    format,
                                      AP4_UI16    width,
                                      AP4_UI16    height,
                                      AP4_UI16    depth,
                                      const char* compressor_name) :
    AP4_UnknownSampleDescription(format),
    AP4_VideoSampleDescription(width, height, depth, compressor_name) {}
};

/*----------------------------------------------------------------------
|   AP4_MpegSampleDescription
+---------------------------------------------------------------------*/
class AP4_MpegSampleDescription : public AP4_SampleDescription
{
 public:
    // types
    typedef AP4_UI08 StreamType;
    typedef AP4_UI08 OTI;
    
    // class methods
    const char* GetStreamTypeString(StreamType type);
    const char* GetObjectTypeString(OTI oti);

    // constructors & destructor
    AP4_MpegSampleDescription(AP4_UI32      format,
                              AP4_EsdsAtom* esds);
    AP4_MpegSampleDescription(AP4_UI32              format,
                              StreamType            stream_type,
                              OTI                   oti,
                              const AP4_DataBuffer* decoder_info,
                              AP4_UI32              buffer_size,
                              AP4_UI32              max_bitrate,
                              AP4_UI32              avg_bitrate);
    virtual ~AP4_MpegSampleDescription();
    
    // accessors
    AP4_Byte GetStreamType() const { return m_StreamType; }
    AP4_Byte GetObjectTypeId() const { return m_ObjectTypeId; }
    const AP4_DataBuffer* GetDecoderInfo() const { return m_DecoderInfo; }
    AP4_UI32 GetBufferSize() const { return m_BufferSize; }
    AP4_UI32 GetMaxBitrate() const { return m_MaxBitrate; }
    AP4_UI32 GetAvgBitrate() const { return m_AvgBitrate; }

    // methods
    AP4_EsDescriptor* CreateEsDescriptor() const;

 protected:
    // members
    AP4_UI32        m_Format;
    StreamType      m_StreamType;
    OTI             m_ObjectTypeId;
    AP4_DataBuffer* m_DecoderInfo;
    AP4_UI32        m_BufferSize;
    AP4_UI32        m_MaxBitrate;
    AP4_UI32        m_AvgBitrate;
    
};

/*----------------------------------------------------------------------
|   AP4_MpegSystemSampleDescription
+---------------------------------------------------------------------*/
class AP4_MpegSystemSampleDescription : public AP4_MpegSampleDescription
{
public:
    // constructor
    AP4_MpegSystemSampleDescription(AP4_EsdsAtom* esds);
    AP4_MpegSystemSampleDescription(StreamType            stream_type,
                                    OTI                   oti,
                                    const AP4_DataBuffer* decoder_info,
                                    AP4_UI32              buffer_size,
                                    AP4_UI32              max_bitrate,
                                    AP4_UI32              avg_bitrate);

    // methods
    AP4_Atom* ToAtom() const;
};

/*----------------------------------------------------------------------
|   AP4_MpegAudioSampleDescription
+---------------------------------------------------------------------*/
class AP4_MpegAudioSampleDescription : public AP4_MpegSampleDescription,
                                       public AP4_AudioSampleDescription
{
public:
    // constructor
    AP4_MpegAudioSampleDescription(AP4_EsdsAtom* esds,
                                   unsigned int  sample_rate,
                                   unsigned int  sample_size,
                                   unsigned int  channel_count);
    AP4_MpegAudioSampleDescription(OTI                   oti,
                                   unsigned int          sample_rate,
                                   unsigned int          sample_size,
                                   unsigned int          channel_count,
                                   const AP4_DataBuffer* decoder_info,
                                   AP4_UI32              buffer_size,
                                   AP4_UI32              max_bitrate,
                                   AP4_UI32              avg_bitrate);

    // methods
    AP4_Atom* ToAtom() const;
};

/*----------------------------------------------------------------------
|   AP4_MpegVideoSampleDescription
+---------------------------------------------------------------------*/
class AP4_MpegVideoSampleDescription : public AP4_MpegSampleDescription,
                                       public AP4_VideoSampleDescription
{
public:
    // constructor
    AP4_MpegVideoSampleDescription(AP4_EsdsAtom* esds,
                                   AP4_UI16      width,
                                   AP4_UI16      height,
                                   AP4_UI16      depth,
                                   const char*   compressor_name);
    AP4_MpegVideoSampleDescription(OTI                   oti,
                                   AP4_UI16              width,
                                   AP4_UI16              height,
                                   AP4_UI16              depth,
                                   const char*           compressor_name,
                                   const AP4_DataBuffer* decoder_info,
                                   AP4_UI32              buffer_size,
                                   AP4_UI32              max_bitrate,
                                   AP4_UI32              avg_bitrate);

    // methods
    AP4_Atom* ToAtom() const;
};

/*----------------------------------------------------------------------
|   constants
+---------------------------------------------------------------------*/
const AP4_MpegSampleDescription::StreamType AP4_FORBIDDEN_STREAM_TYPE = 0x00;
const AP4_MpegSampleDescription::StreamType AP4_OD_STREAM_TYPE        = 0x01;
const AP4_MpegSampleDescription::StreamType AP4_CR_STREAM_TYPE        = 0x02;	
const AP4_MpegSampleDescription::StreamType AP4_BIFS_STREAM_TYPE      = 0x03;
const AP4_MpegSampleDescription::StreamType AP4_VISUAL_STREAM_TYPE    = 0x04;
const AP4_MpegSampleDescription::StreamType AP4_AUDIO_STREAM_TYPE     = 0x05;
const AP4_MpegSampleDescription::StreamType AP4_MPEG7_STREAM_TYPE     = 0x06;
const AP4_MpegSampleDescription::StreamType AP4_IPMP_STREAM_TYPE      = 0x07;
const AP4_MpegSampleDescription::StreamType AP4_OCI_STREAM_TYPE       = 0x08;
const AP4_MpegSampleDescription::StreamType AP4_MPEGJ_STREAM_TYPE     = 0x09;

const AP4_MpegSampleDescription::OTI AP4_MPEG4_SYSTEM_OTI         = 0x01;
const AP4_MpegSampleDescription::OTI AP4_MPEG4_SYSTEM_COR_OTI     = 0x02;
const AP4_MpegSampleDescription::OTI AP4_MPEG4_VISUAL_OTI         = 0x20;
const AP4_MpegSampleDescription::OTI AP4_MPEG4_AUDIO_OTI          = 0x40;
const AP4_MpegSampleDescription::OTI AP4_MPEG2_VISUAL_SIMPLE_OTI  = 0x60;
const AP4_MpegSampleDescription::OTI AP4_MPEG2_VISUAL_MAIN_OTI    = 0x61;
const AP4_MpegSampleDescription::OTI AP4_MPEG2_VISUAL_SNR_OTI     = 0x62;
const AP4_MpegSampleDescription::OTI AP4_MPEG2_VISUAL_SPATIAL_OTI = 0x63;
const AP4_MpegSampleDescription::OTI AP4_MPEG2_VISUAL_HIGH_OTI    = 0x64;
const AP4_MpegSampleDescription::OTI AP4_MPEG2_VISUAL_422_OTI     = 0x65;
const AP4_MpegSampleDescription::OTI AP4_MPEG2_AAC_AUDIO_MAIN_OTI = 0x66;
const AP4_MpegSampleDescription::OTI AP4_MPEG2_AAC_AUDIO_LC_OTI   = 0x67;
const AP4_MpegSampleDescription::OTI AP4_MPEG2_AAC_AUDIO_SSRP_OTI = 0x68;
const AP4_MpegSampleDescription::OTI AP4_MPEG2_PART3_AUDIO_OTI    = 0x69;
const AP4_MpegSampleDescription::OTI AP4_MPEG1_VISUAL_OTI         = 0x6A;
const AP4_MpegSampleDescription::OTI AP4_MPEG1_AUDIO_OTI          = 0x6B;
const AP4_MpegSampleDescription::OTI AP4_JPEG_OTI                 = 0x6C;

#endif // _AP4_SAMPLE_DESCRIPTION_H_
