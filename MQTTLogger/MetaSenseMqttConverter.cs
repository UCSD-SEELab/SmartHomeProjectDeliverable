// BSD 3-Clause License
// 
// Copyright (c) 2018, The Regents of the University of California.
// All rights reserved.
// 
// Created by Massimiliano Menarini
// University of California, San Diego
// Department of Computer Science and Engineering
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

using Newtonsoft.Json;

namespace MQTTLogger
{
    public class MetaSenseMqttConverter
    {
        private const string Topic = "MetaSense-E7E1/raw";
        private readonly MessageReceiver _receiver;

        public MetaSenseMqttConverter(MessageReceiver receiver)
        {
            _receiver = receiver;
        }

        public void Publish(string messageStr)
        {
            var msg = MetaSenseMessage.FromJsonString(messageStr);
            var conv = new MetaSenseConverters(msg);
            var gas = conv.Gas();
            var hupr = conv.HuPr();
            if (msg.Ts == null) return;
            var mqttMessage = new MetaSenseSmartHome
            {
                humidity = hupr.HumPercent * 100,
                pressure = hupr.PresMilliBar,
                temperature = hupr.PresCelsius,
                S1A = gas.No2A * 1000,
                S1W = gas.No2W * 1000,
                S2A = gas.OxA * 1000,
                S2W = gas.OxW * 1000,
                S3A = gas.CoA * 1000,
                S3W = gas.CoW * 1000,
                timestamp = msg.Ts.Value
            };
            if (msg.Co2 != null)
                mqttMessage.CO2 = msg.Co2Raw.Val;

            _receiver.Publish(Topic, JsonConvert.SerializeObject(mqttMessage));
        }
    }
}