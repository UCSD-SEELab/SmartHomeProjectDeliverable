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

using System;
using System.Text;
using uPLibrary.Networking.M2Mqtt;
using uPLibrary.Networking.M2Mqtt.Messages;

namespace MQTTLogger
{
    public class MessageReceiver
    {
        private readonly MqttClient _client;
        private Action<string, string> _func;

        public MessageReceiver()
        {
            try
            {
                Console.Out.WriteLine("Creating MQTT Client");
                _client = new MqttClient(SamsungProjectConfig.MQTTServer, SamsungProjectConfig.MQTTPort, false,
                    MqttSslProtocols.None, null, null);
                _client.MqttMsgPublishReceived += Client_MQTTMsgPublishReceived;
                _client.Connect(Guid.NewGuid().ToString(), SamsungProjectConfig.MQTTUser,
                    SamsungProjectConfig.MQTTPassword);
                string[] topic = {"#"};
                byte[] qosLevels = {MqttMsgBase.QOS_LEVEL_EXACTLY_ONCE};
                _client.Subscribe(topic, qosLevels);
            }
            catch (Exception e)
            {
                Console.Error.WriteLine(e.Message);
                Console.Error.WriteLine(e.StackTrace);
                if (_client != null)
                {
                    _client.Disconnect();
                    _client = null;
                }
            }
        }

        public void Publish(string topic, string message)
        {
            _client.Publish(topic, Encoding.ASCII.GetBytes(message), MqttMsgBase.QOS_LEVEL_EXACTLY_ONCE, true);
        }

        public void Init(Action<string, string> func)
        {
            _func = func;
        }

        public void Disconnect()
        {
            _client.Disconnect();
        }

        private void Client_MQTTMsgPublishReceived(object sender, MqttMsgPublishEventArgs e)
        {
            var message = Encoding.UTF8.GetString(e.Message);
            var logMessage = message.Length < 80 ? message : message.Substring(0, 77) + "...";
            Console.Out.WriteLine($"topic: {e.Topic} - message: {logMessage}");
            _func?.Invoke(e.Topic, message);
        }
    }
}