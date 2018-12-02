// BSD 3-Clause License
//
// Copyright (c) 2018, The Regents of the University of California.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// * Neither the name of the copyright holder nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef LOGGER_h
#define LOGGER_h

int freeRam();
#define M_RAM_LEFT(str) //Serial.print(str); Serial.println(freeRam()); Serial.flush();
#define M_LOG(str) //Serial.println(str); Serial.flush()
#define M_LOG_(str) //Serial.print(str); Serial.flush()
//#define RAM_LEFT(str) ;
//#define LOG(str)
//#define LOG_(str)
//#define M_LOG_OUT_MSG() Serial.print("-->"); msg.SerializeTo(Serial);
//#define M_LOG_OUT_MSG()


#define M_LOGGER_TO_SERIAL(...) //Serial1.printf(__VA_ARGS__);

#define INO_TRACE(...) //M_LOGGER_TO_SERIAL(__VA_ARGS__)
#define PM_TRACE(...) //M_LOGGER_TO_SERIAL(__VA_ARGS__)
#define SC_TRACE(...) //M_LOGGER_TO_SERIAL(__VA_ARGS__)
#define MSG_TRACE(...) //M_LOGGER_TO_SERIAL(__VA_ARGS__)
#define M_TRACE(...) //M_LOGGER_TO_SERIAL(__VA_ARGS__)
#define M_DEBUG(...) // M_LOGGER_TO_SERIAL(__VA_ARGS__)
#define M_INFO(...) // M_LOGGER_TO_SERIAL(__VA_ARGS__)
#define M_ERROR(...) // M_LOGGER_TO_SERIAL(__VA_ARGS__)
#define M_MQTT_TRACE(...) //Serial.printf(__VA_ARGS__);


#define M_OUT_MSG  //Serial.print("-->"); msg.SerializeTo(Serial);
#define M_IN_MSG_A  //Serial.print("<--");
#define M_IN_MSG_N  //Serial.println();
#define M_IN_MSG(...)  //Serial.printf(__VA_ARGS__);

#define LOG_TRACE(...) //Serial.printlnf(__VA_ARGS__);

#endif
