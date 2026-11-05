#pragma once
#include <cstdint>
#include <cstddef>
#include <cstdlib>
#include <string>
#include <algorithm>
#include <cctype>
#include <type_traits>
#define IRAM_ATTR
#define OUTPUT 1
#define INPUT 0
#define INPUT_PULLUP 2
#define LOW 0
#define HIGH 1
#define CHANGE 3
#define SERIAL_8N1 0
class String {
  std::string value_;
 public:
  String()=default;
  String(const char* p):value_(p ? p : ""){}
  String(const std::string &s):value_(s){}
  template <typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value>::type>
  String(T v):value_(std::to_string(v)){}
  String(double v, int precision): value_(std::to_string(v)) { (void)precision; }
  const char* c_str() const {return value_.c_str();}
  void reserve(size_t n){value_.reserve(n);}
  size_t length() const {return value_.size();}
  String substring(unsigned start) const {return String(value_.substr(start));}
  void trim(){auto a=value_.find_first_not_of(" \n\r\t");if(a==std::string::npos){value_.clear();return;}auto b=value_.find_last_not_of(" \n\r\t");value_=value_.substr(a,b-a+1);}
  void toUpperCase(){std::transform(value_.begin(),value_.end(),value_.begin(),[](unsigned char c){return (char)std::toupper(c);});}
  long toInt() const {return std::strtol(value_.c_str(),nullptr,10);}
  bool startsWith(const char* p) const{return value_.rfind(p,0)==0;}
  bool operator==(const char* p) const {return value_==(p?p:"");}
  bool operator!=(const char* p) const {return !(*this==p);}
  String& operator+=(const char* p){value_+=(p?p:"");return *this;}
 String& operator+=(char c){value_+=c;return *this;}
  String& operator+=(const String& rhs){value_+=rhs.value_;return *this;}
  template <typename T> String& operator+=(T v){return (*this += String(v));}
  friend String operator+(String a, const String &b){a+=b;return a;}
};
struct FakeSerial {
#ifdef GUARA_HOST_INTEGRATION
 std::string input;
 void inject(const std::string &text){input += text;}
#endif
 void begin(unsigned long) {}
 void begin(unsigned long,int,int,int){}
 int available() const{
#ifdef GUARA_HOST_INTEGRATION
   return static_cast<int>(input.size());
#else
   return 0;
#endif
 }
 int read(){
#ifdef GUARA_HOST_INTEGRATION
   if(input.empty())return -1;
   char c=input.front();input.erase(input.begin());return c;
#else
   return -1;
#endif
 }
 template<typename T>void println(const T&)const{}
 template<typename T>void print(const T&)const{}
 template<typename ...Args>void printf(const char*,Args...)const{}
};
extern FakeSerial Serial,Serial2;
unsigned long millis();
void delay(unsigned long);
void pinMode(int,int);
void digitalWrite(int,int);
int digitalRead(int);
int analogRead(int);
void analogReadResolution(int);
void tone(int,unsigned int,unsigned long=0);
void noTone(int);
inline double ledcSetup(uint8_t,double,uint8_t){return 0;}
inline void ledcAttachPin(uint8_t,uint8_t){}
inline void ledcWrite(uint8_t,uint32_t){}
void noInterrupts();
void interrupts();
void attachInterrupt(int,void(*)(),int);
int digitalPinToInterrupt(int);
template<typename T> T constrain(T v,T low,T hi){return v<low?low:(v>hi?hi:v);}
