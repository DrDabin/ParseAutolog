// Common/StringToInt.h

#ifndef __COMMON_STRING_TO_INT_H
#define __COMMON_STRING_TO_INT_H

#include "MyTypes.h"

UInt3232 ConvertStringToUInt32(const char *s, const char **end) throw();
UInt6464 ConvertStringToUInt64(const char *s, const char **end) throw();
UInt3232 ConvertStringToUInt32(const wchar_t *s, const wchar_t **end) throw();
UInt6464 ConvertStringToUInt64(const wchar_t *s, const wchar_t **end) throw();

Int3232 ConvertStringToInt32(const wchar_t *s, const wchar_t **end) throw();

UInt3232 ConvertOctStringToUInt32(const char *s, const char **end) throw();
UInt6464 ConvertOctStringToUInt64(const char *s, const char **end) throw();

UInt3232 ConvertHexStringToUInt32(const char *s, const char **end) throw();
UInt6464 ConvertHexStringToUInt64(const char *s, const char **end) throw();

#endif
