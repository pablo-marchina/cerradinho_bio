#pragma once
#include <stddef.h>
#include "Arduino.h"
#ifdef GUARA_HOST_INTEGRATION
#include <map>
#include <vector>
#include <string>
#include <cstring>
namespace FakeNVS {
inline std::map<std::string, std::vector<uint8_t>> bytes;
inline bool failNextWrite = false;
inline size_t partialNextWrite = 0;
inline bool corruptNextWrite = false;
inline void reset(){ bytes.clear(); failNextWrite = false; partialNextWrite = 0; corruptNextWrite = false; }
}
#endif
class Preferences {
 public:
#ifdef GUARA_HOST_INTEGRATION
 bool begin(const char*,bool){return true;}
 bool isKey(const char* key) const {return FakeNVS::bytes.count(key)!=0;}
 size_t putBytes(const char* key,const void* data,size_t size){
  const auto* ptr=static_cast<const uint8_t*>(data);
  if(FakeNVS::failNextWrite){
   FakeNVS::failNextWrite=false;
   const size_t written=std::min(FakeNVS::partialNextWrite,size);
   FakeNVS::partialNextWrite=0;
   if(written>0)FakeNVS::bytes[key]=std::vector<uint8_t>(ptr,ptr+written);
   return written;
  }
  FakeNVS::bytes[key]=std::vector<uint8_t>(ptr,ptr+size);
  if(FakeNVS::corruptNextWrite && size>0){
   FakeNVS::corruptNextWrite=false;
   FakeNVS::bytes[key][size/2]^=0x5a;
  }
  return size;
 }
 size_t getBytes(const char* key,void* dest,size_t size) const {
  auto it=FakeNVS::bytes.find(key);
  if(it==FakeNVS::bytes.end()||it->second.size()<size)return 0;
  std::memcpy(dest,it->second.data(),size);return size;
 }
 size_t getBytesLength(const char* key)const{
  auto it=FakeNVS::bytes.find(key);
  return it==FakeNVS::bytes.end()?0:it->second.size();
 }
 String getString(const char* key,const char* def)const{
  auto it=FakeNVS::bytes.find(key);
  if(it==FakeNVS::bytes.end())return String(def);
  return String(std::string(it->second.begin(),it->second.end()));
 }
 size_t putString(const char* key,const String &s){return putBytes(key,s.c_str(),s.length());}
 bool remove(const char* key){return FakeNVS::bytes.erase(key)!=0;}
#else
 bool begin(const char*,bool){return true;}
 bool isKey(const char*) const{return false;}
 size_t putBytes(const char*,const void*,size_t size){return size;}
 size_t getBytes(const char*,void*,size_t) const{return 0;}
 size_t getBytesLength(const char*)const{return 0;}
 String getString(const char*,const char* def)const{return String(def);}
 size_t putString(const char*,const String &s){return s.length();}
 bool remove(const char*){return true;}
#endif
};
