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

namespace MQTTLogger
{
    public class MetaSenseVoltGasReadings
    {
        public double CoA;
        public double CoW;
        public double Nc;
        public double No2A;
        public double No2W;
        public double OxA;
        public double OxW;
        public double Temp;
    }

    public class MetaSenseHuPrReadings
    {
        public double HumCelsius;
        public double HumPercent;
        public double PresCelsius;
        public double PresMilliBar;
    }


    public class MetaSenseConverters
    {
        private readonly MetaSenseMessage _msg;

        public MetaSenseConverters(MetaSenseMessage msg)
        {
            _msg = msg;
        }

        public MetaSenseHuPrReadings HuPr()
        {
            return ConvertHuPr(_msg.HuPr);
        }

        public static MetaSenseHuPrReadings ConvertHuPr(MetaSenseRawHuPrReadings huPr)
        {
            var ret = new MetaSenseHuPrReadings
            {
                HumCelsius = huPr.HumiditySensorTemperatureCelsius,
                HumPercent = huPr.HumiditySensorHumidityPercent / 100.0,
                PresMilliBar = huPr.BarometricSensorPressureMillibar,
                PresCelsius = huPr.BarometricSensorTemperatureCelsius
            };
            return ret;
        }

        private static double ConvertRawGasToVoltage(int rng, int rawValue)
        {
            double gain = rng;
            if (rng == 0)
                gain = 2.0 / 3.0;
            var voltCalc = 4.096 / (gain * 0x7FFF);
            return rawValue * voltCalc;
        }

        public MetaSenseVoltGasReadings Gas()
        {
            return ConvertGas(_msg.Raw);
        }

        public static MetaSenseVoltGasReadings ConvertGas(MetaSenseRawGasReadings raw)
        {
            var ret = new MetaSenseVoltGasReadings
            {
                CoA = ConvertRawGasToVoltage(raw.Rng, raw.S3A),
                CoW = ConvertRawGasToVoltage(raw.Rng, raw.S3W),
                OxA = ConvertRawGasToVoltage(raw.Rng, raw.S2A),
                OxW = ConvertRawGasToVoltage(raw.Rng, raw.S2W),
                No2A = ConvertRawGasToVoltage(raw.Rng, raw.S1A),
                No2W = ConvertRawGasToVoltage(raw.Rng, raw.S1W),
                Nc = ConvertRawGasToVoltage(raw.Rng, raw.Voc),
                Temp = ConvertRawGasToVoltage(raw.Rng, raw.Temperature)
            };
            return ret;
        }
    }
}