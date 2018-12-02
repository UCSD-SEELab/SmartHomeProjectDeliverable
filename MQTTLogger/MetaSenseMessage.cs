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
using Newtonsoft.Json;

namespace MQTTLogger
{
    public class MetaSenseRawGasReadings
    {
        [JsonProperty("Rng")] public int Rng;

        [JsonProperty("S1A")] public int S1A;

        [JsonProperty("S1W")] public int S1W;

        [JsonProperty("S2A")] public int S2A;

        [JsonProperty("S2W")] public int S2W;

        [JsonProperty("S3A")] public int S3A;

        [JsonProperty("S3W")] public int S3W;

        [JsonProperty("PT")] public int Temperature;

        [JsonProperty("NC")] public int Voc;
    }

    public class MetaSenseLocation
    {
        public float? Accuracy;
        public double? Altitude;
        public float? Bearing;
        public double? Latitude;
        public double? Longitude;
        public float? Speed;
    }

    public class MetaSenseRawHuPrReadings
    {
        [JsonProperty("bP")] public double BarometricSensorPressureMillibar;

        [JsonProperty("bT")] public double BarometricSensorTemperatureCelsius;

        [JsonProperty("hH")] public double HumiditySensorHumidityPercent;

        [JsonProperty("hT")] public double HumiditySensorTemperatureCelsius;
    }
    //public class MetaSenseAfeConfiguration
    //{
    //    public string Ser;
    //    public string Name;
    //    //public int PTV;
    //    //public SensorCalibrationParameters[] cal;
    //}
    //public class SensorCalibrationParameters
    //{
    //    public string Typ;
    //    public int We0;
    //    public int Ws0;
    //    public int Ae0;
    //    public int As0;
    //    public int Sns;
    //    public int No2Sns;
    //}

    public class MetaSenseCo2Readings
    {
        [JsonProperty("CO2")] public double Co2;
    }

    public class MetaSenseCo2RawReadings
    {
        [JsonProperty("val")] public int Val;
    }

    public class MetaSenseVocReadings
    {
        [JsonProperty("vIP")] public double VIp;

        [JsonProperty("vPP")] public double VPp;
    }

    public enum Flags
    {
        //Boolean Flags
        [JsonProperty("s_sd")] StreamSD,
        [JsonProperty("s_wifi")] StreamWifi,
        [JsonProperty("s_ble")] StreamBLE,
        [JsonProperty("wifi_en")] WifiEn,
        [JsonProperty("sleep_en")] SleepEn,
        [JsonProperty("usb_en")] UsbEn,
        [JsonProperty("usb_pass")] UsbPass,
        [JsonProperty("co2_en")] Co2En,
        [JsonProperty("voc_en")] VocEn,

        //Long Flags
        [JsonProperty("power")] Power,
        [JsonProperty("s_inter")] SInter,

        //Int (and enum) flags
        [JsonProperty("f_sd")] FlagSD,
        [JsonProperty("f_wifi")] FlagWifi,
        [JsonProperty("f_ble")] FlagBLE,

        //String Flags
        [JsonProperty("ssid")] Ssid,
        [JsonProperty("pass")] Pass,
        [JsonProperty("node_id")] NodeId,
        [JsonProperty("afe_ser")] AfeSer,
        [JsonProperty("mac_addr")] MacAddr,

        //Execute Command
        [JsonProperty("reset")] Reset,
        [JsonProperty("cl_wifi")] ClWifi,
        [JsonProperty("st_rom")] StRom
    }

    public class MetaSenseMessage
    {
        [JsonProperty("afe_ser")] public string AfeSer;

        //[JsonProperty("conf")]
        //public MetaSenseAfeConfiguration Conf;
        [JsonProperty("co2")] public MetaSenseCo2Readings Co2;

        [JsonProperty("co2_en")] public bool? Co2En;

        [JsonProperty("co2R")] public MetaSenseCo2RawReadings Co2Raw;

        [JsonProperty("f_ble")] public int? FlagBLE;

        //Int (and enum) flags
        [JsonProperty("f_sd")] public int? FlagSD;
        [JsonProperty("f_wifi")] public int? FlagWifi;

        [JsonProperty("hu_pr")] public MetaSenseRawHuPrReadings HuPr;

        [JsonProperty("loc")] public LocationInfo Loc;

        [JsonProperty("mac_addr")] public string MacAddr;
        [JsonProperty("node_id")] public string NodeId;

        [JsonProperty("pass")] public string Pass;

        //Long Flags
        [JsonProperty("power")] public long? Power;

        [JsonProperty("raw")] public MetaSenseRawGasReadings Raw;

        [JsonProperty("req")] public string Req;

        [JsonProperty("s_inter")] public long? SInter;
        [JsonProperty("sleep_en")] public bool? SleepEn;


        //Boolean Flags
        [JsonProperty("s_sd")] public bool? SSd;

        //String Flags
        [JsonProperty("ssid")] public string Ssid;
        [JsonProperty("s_ble")] public bool? StreamBLE;
        [JsonProperty("s_wifi")] public bool? SWifi;

        [JsonProperty("ts")] public long? Ts;

        [JsonProperty("usb_en")] public bool? UsbEn;
        [JsonProperty("usb_pass")] public bool? UsbPass;

        [JsonProperty("voc")] public MetaSenseVocReadings Voc;

        [JsonProperty("voc_en")] public bool? VocEn;
        [JsonProperty("wifi_en")] public bool? WifiEn;

        public MetaSenseMessage()
        {
        }

        public MetaSenseMessage(Flags reqFlag)
        {
            Req = reqFlag.ToString();
        }

        public string ToJsonString()
        {
            var settings = new JsonSerializerSettings
            {
                MissingMemberHandling = MissingMemberHandling.Ignore,
                NullValueHandling = NullValueHandling.Ignore
            };
            JsonConvert.DefaultSettings = () => settings;

            return JsonConvert.SerializeObject(this);
        }

        public static MetaSenseMessage FromJsonString(string str)
        {
            var settings = new JsonSerializerSettings
            {
                MissingMemberHandling = MissingMemberHandling.Ignore,
                NullValueHandling = NullValueHandling.Ignore
            };
            JsonConvert.DefaultSettings = () => settings;
            try
            {
                return JsonConvert.DeserializeObject<MetaSenseMessage>(str);
            }
            catch (Exception)
            {
                //Ignore decoding errors (they are due to empty strings and debug strings
                return null;
            }
        }
    }
}