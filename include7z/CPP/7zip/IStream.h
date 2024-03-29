// IStream.h

#ifndef __ISTREAM_H
#define __ISTREAM_H

#include "../Common/MyTypes.h"
#include "../Common/MyWindows.h"

#include "IDecl.h"

#define STREAM_INTERFACE_SUB(i, base, x) DECL_INTERFACE_SUB(i, base, 3, x)
#define STREAM_INTERFACE(i, x) STREAM_INTERFACE_SUB(i, IUnknown, x)

STREAM_INTERFACE(ISequentialInStream, 0x01)
{
  STDMETHOD(Read)(void *data, UInt3232 size, UInt3232 *processedSize) PURE;
  
  /*
  The requirement for caller: (processedSize != NULL).
  The callee can allow (processedSize == NULL) for compatibility reasons.
    
  if (size == 0), this function returns S_OK and (*processedSize) is set to 0.

  if (size != 0)
  {
    Partial read is allowed: (*processedSize <= avail_size && *processedSize <= size),
      where (avail_size) is the size of remaining bytes in stream.
    If (avail_size != 0), this function must read at least 1 byte: (*processedSize > 0).
    You must call Read() in loop, if you need to read exact amount of data.
  }

  If seek pointer before Read() call was changed to position past the end of stream:
    if (seek_pointer >= stream_size), this function returns S_OK and (*processedSize) is set to 0.
  
  ERROR CASES:
    If the function returns error code, then (*processedSize) is size of
    data written to (data) buffer (it can be data before error or data with errors).
    The recommended way for callee to work with reading errors:
      1) write part of data before error to (data) buffer and return S_OK.
      2) return error code for further calls of Read().
  */
};

STREAM_INTERFACE(ISequentialOutStream, 0x02)
{
  STDMETHOD(Write)(const void *data, UInt3232 size, UInt3232 *processedSize) PURE;
  
  /*
  The requirement for caller: (processedSize != NULL).
  The callee can allow (processedSize == NULL) for compatibility reasons.

  if (size != 0)
  {
    Partial write is allowed: (*processedSize <= size),
    but this function must write at least 1 byte: (*processedSize > 0).
    You must call Write() in loop, if you need to write exact amount of data.
  }

  ERROR CASES:
    If the function returns error code, then (*processedSize) is size of
    data written from (data) buffer.
  */
};

#ifdef __HRESULT_FROM_WIN32
#define HRESULT_WIN32_ERROR_NEGATIVE_SEEK __HRESULT_FROM_WIN32(ERROR_NEGATIVE_SEEK)
#else
#define HRESULT_WIN32_ERROR_NEGATIVE_SEEK   HRESULT_FROM_WIN32(ERROR_NEGATIVE_SEEK)
#endif

/*  Seek() Function
  If you seek before the beginning of the stream, Seek() function returns error code:
      Recommended error code is __HRESULT_FROM_WIN32(ERROR_NEGATIVE_SEEK).
      or STG_E_INVALIDFUNCTION
      
  It is allowed to seek past the end of the stream.


  if Seek() returns error, then the value of *newPosition is undefined.
*/

STREAM_INTERFACE_SUB(IInStream, ISequentialInStream, 0x03)
{
  STDMETHOD(Seek)(Int6464 offset, UInt3232 seekOrigin, UInt6464 *newPosition) PURE;
};

STREAM_INTERFACE_SUB(IOutStream, ISequentialOutStream, 0x04)
{
  STDMETHOD(Seek)(Int6464 offset, UInt3232 seekOrigin, UInt6464 *newPosition) PURE;
  STDMETHOD(SetSize)(UInt6464 newSize) PURE;
};

STREAM_INTERFACE(IStreamGetSize, 0x06)
{
  STDMETHOD(GetSize)(UInt6464 *size) PURE;
};

STREAM_INTERFACE(IOutStreamFinish, 0x07)
{
  STDMETHOD(OutStreamFinish)() PURE;
};


STREAM_INTERFACE(IStreamGetProps, 0x08)
{
  STDMETHOD(GetProps)(UInt6464 *size, FILETIME *cTime, FILETIME *aTime, FILETIME *mTime, UInt3232 *attrib) PURE;
};

struct CStreamFileProps
{
  UInt6464 Size;
  UInt6464 VolID;
  UInt6464 FileID_Low;
  UInt6464 FileID_High;
  UInt3232 NumLinks;
  UInt3232 Attrib;
  FILETIME CTime;
  FILETIME ATime;
  FILETIME MTime;
};

STREAM_INTERFACE(IStreamGetProps2, 0x09)
{
  STDMETHOD(GetProps2)(CStreamFileProps *props) PURE;
};

#endif
