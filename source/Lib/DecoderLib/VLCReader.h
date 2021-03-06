/* -----------------------------------------------------------------------------
Software Copyright License for the Fraunhofer Software Library VVdec

(c) Copyright (2018-2020) Fraunhofer-Gesellschaft zur Förderung der angewandten Forschung e.V. 

1.    INTRODUCTION

The Fraunhofer Software Library VVdec (“Fraunhofer Versatile Video Decoding Library”) is software that implements (parts of) the Versatile Video Coding Standard - ITU-T H.266 | MPEG-I - Part 3 (ISO/IEC 23090-3) and related technology. 
The standard contains Fraunhofer patents as well as third-party patents. Patent licenses from third party standard patent right holders may be required for using the Fraunhofer Versatile Video Decoding Library. It is in your responsibility to obtain those if necessary. 

The Fraunhofer Versatile Video Decoding Library which mean any source code provided by Fraunhofer are made available under this software copyright license. 
It is based on the official ITU/ISO/IEC VVC Test Model (VTM) reference software whose copyright holders are indicated in the copyright notices of its source files. The VVC Test Model (VTM) reference software is licensed under the 3-Clause BSD License and therefore not subject of this software copyright license.

2.    COPYRIGHT LICENSE

Internal use of the Fraunhofer Versatile Video Decoding Library, in source and binary forms, with or without modification, is permitted without payment of copyright license fees for non-commercial purposes of evaluation, testing and academic research. 

No right or license, express or implied, is granted to any part of the Fraunhofer Versatile Video Decoding Library except and solely to the extent as expressly set forth herein. Any commercial use or exploitation of the Fraunhofer Versatile Video Decoding Library and/or any modifications thereto under this license are prohibited.

For any other use of the Fraunhofer Versatile Video Decoding Library than permitted by this software copyright license You need another license from Fraunhofer. In such case please contact Fraunhofer under the CONTACT INFORMATION below.

3.    LIMITED PATENT LICENSE

As mentioned under 1. Fraunhofer patents are implemented by the Fraunhofer Versatile Video Decoding Library. If You use the Fraunhofer Versatile Video Decoding Library in Germany, the use of those Fraunhofer patents for purposes of testing, evaluating and research and development is permitted within the statutory limitations of German patent law. However, if You use the Fraunhofer Versatile Video Decoding Library in a country where the use for research and development purposes is not permitted without a license, you must obtain an appropriate license from Fraunhofer. It is Your responsibility to check the legal requirements for any use of applicable patents.    

Fraunhofer provides no warranty of patent non-infringement with respect to the Fraunhofer Versatile Video Decoding Library.


4.    DISCLAIMER

The Fraunhofer Versatile Video Decoding Library is provided by Fraunhofer "AS IS" and WITHOUT ANY EXPRESS OR IMPLIED WARRANTIES, including but not limited to the implied warranties fitness for a particular purpose. IN NO EVENT SHALL FRAUNHOFER BE LIABLE for any direct, indirect, incidental, special, exemplary, or consequential damages, including but not limited to procurement of substitute goods or services; loss of use, data, or profits, or business interruption, however caused and on any theory of liability, whether in contract, strict liability, or tort (including negligence), arising in any way out of the use of the Fraunhofer Versatile Video Decoding Library, even if advised of the possibility of such damage.

5.    CONTACT INFORMATION

Fraunhofer Heinrich Hertz Institute
Attention: Video Coding & Analytics Department
Einsteinufer 37
10587 Berlin, Germany
www.hhi.fraunhofer.de/vvc
vvc@hhi.fraunhofer.de
------------------------------------------------------------------------------------------- */

/** \file     VLCWReader.h
 *  \brief    Reader for high level syntax
 */

#ifndef __VLCREADER__
#define __VLCREADER__

#include "CommonLib/Rom.h"
#include "CommonLib/BitStream.h"
#include "CommonLib/Slice.h"
#include "CommonLib/SampleAdaptiveOffset.h"
#include "CABACReader.h"

#if ENABLE_TRACING

#define READ_SCODE(length, code, name)    xReadSCode  ( length, code, name )
#define READ_CODE(length, code, name)     xReadCodeTr ( length, code, name )
#define READ_UVLC(        code, name)     xReadUvlcTr (         code, name )
#define READ_SVLC(        code, name)     xReadSvlcTr (         code, name )
#define READ_FLAG(        code, name)     xReadFlagTr (         code, name )

#else


#define READ_SCODE(length, code, name)    xReadSCode ( length, code )
#define READ_CODE(length, code, name)     xReadCode ( length, code )
#define READ_UVLC(        code, name)     xReadUvlc (         code )
#define READ_SVLC(        code, name)     xReadSvlc (         code )
#define READ_FLAG(        code, name)     xReadFlag (         code )


#endif

//! \ingroup DecoderLib
//! \{

// ====================================================================================================================
// Class definition
// ====================================================================================================================

class VLCReader
{
protected:
  InputBitstream*   m_pcBitstream;

  VLCReader() : m_pcBitstream (NULL) {};
  virtual ~VLCReader() {};

  void  xReadCode    ( uint32_t   length, uint32_t& val );
  void  xReadUvlc    ( uint32_t& val );
  void  xReadSvlc    ( int& val );
  void  xReadFlag    ( uint32_t& val );
#if ENABLE_TRACING
  void  xReadCodeTr  ( uint32_t  length, uint32_t& rValue, const char *pSymbolName );
  void  xReadUvlcTr  ( uint32_t& rValue, const char *pSymbolName );
  void  xReadSvlcTr  ( int& rValue, const char *pSymbolName );
  void  xReadFlagTr  ( uint32_t& rValue, const char *pSymbolName );
#endif
#if ENABLE_TRACING
  void  xReadSCode   ( uint32_t length, int& val, const char *pSymbolName );
#else
  void  xReadSCode   ( uint32_t length, int& val );
#endif

public:
  void  setBitstream ( InputBitstream* p )   { m_pcBitstream = p; }
  InputBitstream* getBitstream() { return m_pcBitstream; }

protected:
  void xReadRbspTrailingBits();
  bool isByteAligned() { return ( m_pcBitstream->getNumBitsUntilByteAligned() == 0 ); }
};



class AUDReader: public VLCReader
{
public:
  AUDReader() {};
  virtual ~AUDReader() {};
  void parseAccessUnitDelimiter( InputBitstream* bs, uint32_t &picType );
};



class FDReader: public VLCReader
{
public:
  FDReader() {};
  virtual ~FDReader() {};
  void parseFillerData(InputBitstream* bs, uint32_t &fdSize);
};



class HLSyntaxReader : public VLCReader
{
public:
  HLSyntaxReader();
  virtual ~HLSyntaxReader();

protected:
  void  copyRefPicList      ( SPS* pcSPS, ReferencePictureList* source_rpl, ReferencePictureList* dest_rpl );
  void  parseRefPicList     ( SPS* pcSPS, ReferencePictureList* rpl, int rplIdx );

public:
  void  setBitstream        ( InputBitstream* p )   { m_pcBitstream = p; }
  void  parseVPS            ( VPS* pcVPS );
  void  parseDCI            ( DCI* dci );
  void  parseSPS            ( SPS* pcSPS, ParameterSetManager *parameterSetManager );
  void  parsePPS            ( PPS* pcPPS, ParameterSetManager *parameterSetManager );
  void  parseAPS            ( APS* pcAPS);
  void  parseAlfAps         ( APS* pcAPS );
  void  parseLmcsAps        ( APS* pcAPS );
  void  parseScalingListAps ( APS* pcAPS );
  void  parseVUI            ( VUI* pcVUI, SPS* pcSPS );
  void  parseConstraintInfo ( ConstraintInfo *cinfo );
#if JVET_Q0786_PTL_only
  void  parseProfileTierLevel( ProfileTierLevel *ptl, bool profileTierPresentFlag, int maxNumSubLayersMinus1 );
#else
  void  parseProfileTierLevel( ProfileTierLevel *ptl, int maxNumSubLayersMinus1 );
#endif
  void  parseOlsHrdParameters    ( GeneralHrdParams* generalHrd, OlsHrdParams *olsHrd, uint32_t firstSubLayer, uint32_t tempLevelHigh );
  void  parseGeneralHrdParameters( GeneralHrdParams *generalHrd );
  void  parsePictureHeader  ( PicHeader* picHeader, ParameterSetManager *parameterSetManager, bool readRbspTrailingBits );
  void  parseSliceHeader    ( Slice* pcSlice, PicHeader* parsedPicHeader, ParameterSetManager *parameterSetManager, const int prevTid0POC, Picture* parsePic );
  void  checkAlfNaluTidAndPicTid(Slice* pcSlice, PicHeader* picHeader, ParameterSetManager *parameterSetManager);
  void  getSlicePoc         ( Slice* pcSlice, PicHeader* picHeader, ParameterSetManager *parameterSetManager, const int prevTid0POC );
  void  parseTerminatingBit ( uint32_t& ruiBit );
  void  parseRemainingBytes ( bool noTrailingBytesExpected );

  void  parsePredWeightTable( Slice* pcSlice, const SPS *sps );
  void  parsePredWeightTable( PicHeader *picHeader, const SPS *sps );
  void  parseScalingList    ( ScalingList *scalingList, bool aps_chromaPresentFlag );
  void  decodeScalingList   ( ScalingList *scalingList, uint32_t scalingListId, bool isPredictor );
  void  parseReshaper       ( SliceReshapeInfo& sliceReshaperInfo, const SPS* pcSPS, const bool isIntra );
  void  alfFilter           ( AlfSliceParam& alfSliceParam, const bool isChroma, const int altIdx );
  void  ccAlfFilter         ( Slice *pcSlice );
#if JVET_P0117_PTL_SCALABILITY
  void  dpb_parameters      ( int maxSubLayersMinus1, bool subLayerInfoFlag, SPS *pcSPS );
#endif
  void  parseExtraPHBitsStruct( SPS *sps, int numBytes );
  void  parseExtraSHBitsStruct( SPS *sps, int numBytes );
private:

protected:
  bool  xMoreRbspData();
};


//! \}

#endif
