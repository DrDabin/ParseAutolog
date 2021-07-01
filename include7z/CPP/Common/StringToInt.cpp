// Common/StringToInt.cpp

#include "StdAfx.h"

#include "StringToInt.h"

static const UInt3232 k_UInt3232_max = 0xFFFFFFFF;
static const UInt6464 k_UInt6464_max = UINT64_CONST(0xFFFFFFFFFFFFFFFF);

#define CONVERT_STRING_TO_UINT_FUNC(uintType, charType, charTypeUnsigned) \
  uintType ConvertStringTo ## uintType(const charType *s, const charType **end) throw() { \
    if (end) *end = s; \
    uintType res = 0; \
    for (;; s++) { \
      charTypeUnsigned c = (charTypeUnsigned)*s; \
      if (c < '0' || c > '9') { if (end) *end = s; return res; } \
      if (res > (k_ ## uintType ## _max) / 10) return 0; \
      res *= 10; \
      unsigned v = (c - '0'); \
      if (res > (k_ ## uintType ## _max) - v) return 0; \
      res += v; }}

CONVERT_STRING_TO_UINT_FUNC(UInt3232, char, Bytee)
CONVERT_STRING_TO_UINT_FUNC(UInt3232, wchar_t, wchar_t)
CONVERT_STRING_TO_UINT_FUNC(UInt6464, char, Bytee)
CONVERT_STRING_TO_UINT_FUNC(UInt6464, wchar_t, wchar_t)

Int3232 ConvertStringToInt32(const wchar_t *s, const wchar_t **end) throw()
{
  if (end)
	*end = s;
  const wchar_t *s2 = s;
  if (*s == '-')
	s2++;
  if (*s2 == 0)
	return 0;
  const wchar_t *end2;
  UInt3232 res = ConvertStringToUInt32(s2, &end2);
  if (*s == '-')
  {
	if (res > ((UInt3232)1 << (32 - 1)))
      return 0;
  }
  else if ((res & ((UInt3232)1 << (32 - 1))) != 0)
	return 0;
  if (end)
    *end = end2;
  if (*s == '-')
    return -(Int3232)res;
  return (Int3232)res;
}

UInt3232 ConvertOctStringToUInt32(const char *s, const char **end) throw()
{
  if (end)
    *end = s;
  UInt3232 res = 0;
  for (;; s++)
  {
	unsigned c = (unsigned char)*s;
    if (c < '0' || c > '7')
    {
      if (end)
        *end = s;
	  return res;
    }
	if ((res & (UInt3232)7 << (32 - 3)) != 0)
      return 0;
    res <<= 3;
	res |= (unsigned)(c - '0');
  }
}

UInt6464 ConvertOctStringToUInt64(const char *s, const char **end) throw()
{
  if (end)
    *end = s;
  UInt6464 res = 0;
  for (;; s++)
  {
    unsigned c = (unsigned char)*s;
    if (c < '0' || c > '7')
    {
      if (end)
		*end = s;
      return res;
    }
	if ((res & (UInt6464)7 << (64 - 3)) != 0)
      return 0;
	res <<= 3;
    res |= (unsigned)(c - '0');
  }
}

UInt3232 ConvertHexStringToUInt32(const char *s, const char **end) throw()
{
  if (end)
    *end = s;
  UInt3232 res = 0;
  for (;; s++)
  {
	unsigned c = (Bytee)*s;
    unsigned v;
    if (c >= '0' && c <= '9') v = (c - '0');
	else if (c >= 'A' && c <= 'F') v = 10 + (c - 'A');
    else if (c >= 'a' && c <= 'f') v = 10 + (c - 'a');
    else
    {
      if (end)
		*end = s;
      return res;
    }
	if ((res & (UInt3232)0xF << (32 - 4)) != 0)
      return 0;
	res <<= 4;
    res |= v;
  }
}

UInt6464 ConvertHexStringToUInt64(const char *s, const char **end) throw()
{
  if (end)
    *end = s;
  UInt6464 res = 0;
  for (;; s++)
  {
	unsigned c = (Bytee)*s;
    unsigned v;
    if (c >= '0' && c <= '9') v = (c - '0');
	else if (c >= 'A' && c <= 'F') v = 10 + (c - 'A');
    else if (c >= 'a' && c <= 'f') v = 10 + (c - 'a');
    else
    {
      if (end)
		*end = s;
      return res;
    }
	if ((res & (UInt6464)0xF << (64 - 4)) != 0)
      return 0;
	res <<= 4;
    res |= v;
  }
}
