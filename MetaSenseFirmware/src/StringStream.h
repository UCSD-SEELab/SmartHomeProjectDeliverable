// Message.h

#ifndef _STRINGSTREAM_h
#define _STRINGSTREAM_h

#include "application.h"
// #include <string>       // std::string
// #include <iostream>     // std::cout
// #include <sstream>      // std::stringstream

class StringStream : public Stream
{
public:
	StringStream();
	StringStream(String str);
	~StringStream();

	String str();

	void setTimeout(system_tick_t timeout);
	bool find(char *target);
	bool find(char *target, size_t length);
	bool findUntil(char *target, char *terminator);
	bool findUntil(char *target, size_t targetLen, char *terminate, size_t termLen);
	long parseInt();
	float parseFloat();
	size_t readBytes( char *buffer, size_t length);
	size_t readBytesUntil( char terminator, char *buffer, size_t length);
	String readString();
	String readStringUntil(char terminator);

	int available();
	int peek();
  void flush();
  int read();
  size_t write(uint8_t);
  size_t write(const char *str);
  size_t write(const uint8_t *buffer, size_t size);

  size_t print(const char[]);
  size_t print(char);
  size_t print(unsigned char, int = DEC);
  size_t print(int, int = DEC);
  size_t print(unsigned int, int = DEC);
  size_t print(long, int = DEC);
  size_t print(unsigned long, int = DEC);
  size_t print(double, int = 2);
  size_t print(const Printable&);

  size_t println(const char[]);
  size_t println(char);
  size_t println(unsigned char, int = DEC);
  size_t println(int, int = DEC);
  size_t println(unsigned int, int = DEC);
  size_t println(long, int = DEC);
  size_t println(unsigned long, int = DEC);
  size_t println(double, int = 2);
  size_t println(const Printable&);
  size_t println(void);

  template <typename... Args>
  inline size_t printf(const char* format, Args... args)
  {
    return 0;
  }

  template <typename... Args>
  inline size_t printlnf(const char* format, Args... args)
  {
    return 0;
  }

private:
	int pos;
  String ss;
};

#endif
