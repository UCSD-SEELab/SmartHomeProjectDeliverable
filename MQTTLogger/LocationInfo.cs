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

namespace MQTTLogger
{
    public class LocationInfo
    {
        public LocationInfo(double latitude, double longitude, double? radius, double? altitude, double? speed,
            double? direction, DateTime timestamp)
        {
            Latitude = latitude;
            Longitude = longitude;
            Radius = radius;
            Altitude = altitude;
            Speed = speed;
            Direction = direction;
            TimeStamp = timestamp;
        }

        public double Latitude { get; protected set; }
        public double Longitude { get; protected set; }
        public double? Radius { get; protected set; } // meters
        public double? Altitude { get; protected set; } // Feet
        public double? Speed { get; protected set; } // Miles/hour
        public double? Direction { get; protected set; } // Degrees
        public DateTime TimeStamp { get; protected set; } // Degrees

        public double DistanceInMetersFrom(LocationInfo location)
        {
            return DistanceInMeters(Latitude, Longitude, location.Latitude, location.Longitude);
        }

        public double DistanceInMetersFrom(double latitude, double longitude)
        {
            return DistanceInMeters(Latitude, Longitude, latitude, longitude);
        }

        public static double DistanceInMeters(double latitude1, double longitude1, double latitude2, double longitude2)
        {
            var theta = longitude1 - longitude2;
            var dist = Math.Sin(Deg2Rad(latitude1)) * Math.Sin(Deg2Rad(latitude2)) + Math.Cos(Deg2Rad(latitude1)) *
                       Math.Cos(Deg2Rad(latitude2)) * Math.Cos(Deg2Rad(theta));
            dist = Math.Acos(dist);
            dist = Rad2Deg(dist);
            dist = dist * 60 * 1853.159616;
            return dist;
        }

        private static double Deg2Rad(double deg)
        {
            return deg * Math.PI / 180.0;
        }

        private static double Rad2Deg(double rad)
        {
            return rad / Math.PI * 180.0;
        }
    }
}