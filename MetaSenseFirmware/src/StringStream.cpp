#include "StringStream.h"

StringStream::StringStream() {
  pos = 0;
}
StringStream::StringStream(String str) {
  ss.concat(str);
  pos = 0;
}

StringStream::~StringStream() {}


void StringStream::setTimeout(system_tick_t timeout) {

}
bool StringStream::find(char *target) {
  return false;
}
bool StringStream::find(char *target, size_t length) {
  return false;
}
bool StringStream::findUntil(char *target, char *terminator) {
  return false;
}
bool StringStream::findUntil(char *target, size_t targetLen, char *terminate, size_t termLen) {
  return false;
}

void StringStream::flush() {}
int StringStream::available() {
  return pos<ss.length();
}
int StringStream::peek() {
  if (pos<ss.length())
    return ss.charAt(pos);
  return -1;
}


long StringStream::parseInt() {
  //not supported
  return 0;
}
float StringStream::parseFloat() {
  //not supported
  return 0.0;
}
size_t StringStream::readBytes( char *buffer, size_t length) {
  //not supported
  return 0;
}
size_t StringStream::readBytesUntil( char terminator, char *buffer, size_t length) {
  //not supported
  return 0;
}
String StringStream::readString() {
  //not supported
  if (!available())
    return "";
  String ret = ss.substring(pos);
  pos = ss.length();
  return ret;

}
String StringStream::readStringUntil(char terminator) {
  //not supported
  return "";
}
int StringStream::read() {
  if (available())
    return ss.charAt(pos++);
  return -1;
}


size_t StringStream::write(uint8_t val) {
//  Serial.println("write uint8_t val"); Serial.flush();
  ss.concat(String((char)val));
  return 1;
}
size_t StringStream::write(const char *str) {
//  Serial.println("write const char *str"); Serial.flush();
  ss.concat(String(str));
  return strlen(str);
}
size_t StringStream::write(const uint8_t *buffer, size_t size) {
//  Serial.println("write const uint8_t *buffer, size_t size"); Serial.flush();
  ss.concat(String((char*)buffer));
  return size;
}

size_t StringStream::print(const char chrs[]) {
  //Serial.println("print chrs[]"); Serial.flush();
  /*Serial.print(chrs);
  Serial.print("->");
  Serial.println(String(chrs).c_str());*/
  ss.concat(String(chrs));
  return strlen(chrs);
}
size_t StringStream::print(char chr) {
  //Serial.println("print chr"); Serial.flush();
  /*Serial.print(chr);
  Serial.print("->");
  Serial.println(String(chr).c_str());*/
  ss.concat(String(chr));
  return 1;
}
size_t StringStream::print(unsigned char val, int base) {
  //Serial.println("print unsigned char"); Serial.flush();
  if (base == 2)
    ss.concat(String(val,BIN));
  if (base == 8)
    ss.concat(String(val,OCT));
  if (base == 10)
    ss.concat(String(val,DEC));
  if (base == 16)
    ss.concat(String(val,HEX));
  return 1;
}
size_t StringStream::print(int val, int base) {
  //Serial.println("print int"); Serial.flush();
  if (base == 2)
    ss.concat(String(val,BIN));
  if (base == 8)
    ss.concat(String(val,OCT));
  if (base == 10)
    ss.concat(String(val,DEC));
  if (base == 16)
    ss.concat(String(val,HEX));
  return 1;
}
size_t StringStream::print(unsigned int val, int base) {
  //Serial.println("print unsigned int"); Serial.flush();
  if (base == 2)
    ss.concat(String(val,BIN));
  if (base == 8)
    ss.concat(String(val,OCT));
  if (base == 10)
    ss.concat(String(val,DEC));
  if (base == 16)
    ss.concat(String(val,HEX));
  return 1;
}
size_t StringStream::print(long val, int base) {
  //Serial.println("print long"); Serial.flush();
  if (base == 2)
    ss.concat(String(val,BIN));
  if (base == 8)
    ss.concat(String(val,OCT));
  if (base == 10)
    ss.concat(String(val,DEC));
  if (base == 16)
    ss.concat(String(val,HEX));
  return 1;
}
size_t StringStream::print(unsigned long val , int base) {
  //Serial.println("print unsigned long"); Serial.flush();
  if (base == 2)
    ss.concat(String(val,BIN));
  if (base == 8)
    ss.concat(String(val,OCT));
  if (base == 10)
    ss.concat(String(val,DEC));
  if (base == 16)
    ss.concat(String(val,HEX));
  return 1;
}
size_t StringStream::print(double val, int dec) {
  //Serial.println("print double"); Serial.flush();
  ss.concat(String(val,dec));
  return 1;
}
size_t StringStream::print(const Printable& printable) {
  /*Serial.println("print printable"); Serial.flush();
  Serial.print(printable);
  Serial.print("->");
  Serial.println(String(printable).c_str());*/
  ss.concat(String(printable));
  return 1;
}

size_t StringStream::println(const char chrs[]) {
  int ret = print(chrs);
  print('\n');
  return ret + 1;
}
size_t StringStream::println(char chr) {
  int ret = print(chr);
  print('\n');
  return ret + 1;
}
size_t StringStream::println(unsigned char val, int base) {
  int ret = print(val,base);
  print('\n');
  return ret + 1;
}
size_t StringStream::println(int val, int base) {
  int ret = print(val, base);
  print('\n');
  return ret + 1;
}
size_t StringStream::println(unsigned int val, int base) {
  int ret = print(val,base);
  print('\n');
  return ret + 1;
}
size_t StringStream::println(long val, int base) {
  int ret = print(val, base);
  print('\n');
  return ret + 1;
}
size_t StringStream::println(unsigned long val, int base) {
  int ret = print(val, base);
  print('\n');
  return ret + 1;
}
size_t StringStream::println(double val, int dec) {
  int ret = print(val, dec);
  print('\n');
  return ret + 1;
}
size_t StringStream::println(const Printable& printable) {
  int ret = print(printable);
  print('\n');
  return ret + 1;
}
size_t StringStream::println(void) {
  print('\n');
  return 1;
}

String StringStream::str() {
  return ss;
}
