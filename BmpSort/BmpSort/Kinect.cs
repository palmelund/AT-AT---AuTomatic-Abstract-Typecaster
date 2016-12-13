using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.Drawing.Imaging;
using System.Globalization;
using System.IO;
using System.IO.Ports;
using System.Linq;
using System.Runtime.InteropServices;
using System.Security.RightsManagement;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Threading;
using Accord.Imaging.Filters;
using Accord.Statistics.Kernels;
using Microsoft.Kinect;
using PixelFormat = System.Drawing.Imaging.PixelFormat;

namespace BmpSort
{
    class Kinect
    {
        public Kinect()
        {
            // Look through all sensors and start the first connected one.
            // This requires that a Kinect is connected at the time of app startup.
            // To make your app robust against plug/unplug, 
            // it is recommended to use KinectSensorChooser provided in Microsoft.Kinect.Toolkit (See components in Toolkit Browser).
            foreach (var potentialSensor in KinectSensor.KinectSensors)
            {
                if (potentialSensor.Status == KinectStatus.Connected)
                {
                    _sensor = potentialSensor;
                    break;
                }
            }

            if (null != _sensor)
            {
                // Turn on the color stream to receive color frames
                _sensor.ColorStream.Enable(ColorImageFormat.RgbResolution640x480Fps30);
                try
                {
                    _sensor.Start();
                }
                catch (IOException)
                {
                    _sensor = null;
                }
            }
        }
       
        private readonly KinectSensor _sensor;

        public Bitmap TakePicture(Rectangle cropped)
        {
            using (var frame = _sensor.ColorStream.OpenNextFrame(10000))
            {
                if (frame == null)
                    throw new NotImplementedException("I don't want this to happen");

                var raw = frame.GetRawPixelData();
                var tempBitmap = new Bitmap(frame.Width, frame.Height, PixelFormat.Format32bppRgb);
                
                var data = tempBitmap.LockBits(
                    new Rectangle(0, 0, frame.Width, frame.Height),
                    ImageLockMode.WriteOnly,
                    tempBitmap.PixelFormat);

                // This seems to be the best way to copy an array into the pointer for 
                // the bitmap.
                Marshal.Copy(raw, 0, data.Scan0, frame.PixelDataLength);
                tempBitmap.UnlockBits(data);

                var result = tempBitmap.Clone(cropped, PixelFormat.Format32bppRgb);
                return result;
            }
        }
    }
}