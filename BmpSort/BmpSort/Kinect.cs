using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Imaging;
using System.Globalization;
using System.IO;
using System.IO.Ports;
using System.Linq;
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

                // Allocate space to put the pixels we'll receive
                _colorPixels = new byte[_sensor.ColorStream.FramePixelDataLength];

                // This is the bitmap we'll display on-screen
                ColorBitmap = new WriteableBitmap(_sensor.ColorStream.FrameWidth,
                    _sensor.ColorStream.FrameHeight, 96.0, 96.0, PixelFormats.Bgr32, null);

                // Start the sensor!
                try
                {
                    _sensor.Start();
                    _sensor.ColorFrameReady += SensorColorPictureFrameReady;
                }
                catch (IOException)
                {
                    _sensor = null;
                }
            }
        }


        public WriteableBitmap ColorBitmap { get; set; }
        
        private const string PortName = "COM3";
        private const int BaudRate = 9600;
        private SerialPort _sp = new SerialPort(PortName, BaudRate, Parity.None, 8, StopBits.One);
        private readonly KinectSensor _sensor;
        private readonly byte[] _colorPixels;

        public BitmapSource TakePicture(Int32Rect cropped)
        {
            WriteableBitmap result = new WriteableBitmap(cropped.Width, cropped.Height, ColorBitmap.DpiX,
                ColorBitmap.DpiY, ColorBitmap.Format, ColorBitmap.Palette);

            result.WritePixels(cropped,_colorPixels, ColorBitmap.BackBufferStride, 0, 0);

            return result;
        }

        private void SensorColorPictureFrameReady(object sender, ColorImageFrameReadyEventArgs e)
        {
            using (ColorImageFrame colorFrame = e.OpenColorImageFrame())
            {
                if (colorFrame != null)
                {
                    // Copy the pixel data from the image to a temporary array
                    colorFrame.CopyPixelDataTo(_colorPixels);

                    // Write the pixel data into our bitmap
                    ColorBitmap.WritePixels(
                        new Int32Rect(0, 0, ColorBitmap.PixelWidth, ColorBitmap.PixelHeight),
                        _colorPixels,
                        ColorBitmap.PixelWidth * sizeof(int),
                        0);
                }
            }
        }
    }
}