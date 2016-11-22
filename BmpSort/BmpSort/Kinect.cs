using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Threading;
using Accord.Imaging.Filters;
using Microsoft.Kinect;

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
                    this.sensor = potentialSensor;
                    break;
                }
            }

            if (null != this.sensor)
            {
                // Turn on the color stream to receive color frames
                this.sensor.ColorStream.Enable(ColorImageFormat.RgbResolution640x480Fps30);

                // Allocate space to put the pixels we'll receive
                this.colorPixels = new byte[this.sensor.ColorStream.FramePixelDataLength];

                // This is the bitmap we'll display on-screen
                this.colorBitmap = new WriteableBitmap(this.sensor.ColorStream.FrameWidth,
                    this.sensor.ColorStream.FrameHeight, 96.0, 96.0, PixelFormats.Bgr32, null);

                // Start the sensor!
                try
                {
                    this.sensor.Start();
                }
                catch (IOException)
                {
                    this.sensor = null;
                }
            }

            if (null == this.sensor)
            {
                // this.statusBarText.Text = Properties.Resources.NoKinectReady;
            }
        }

        public static string PortName
        {
            get { return portName; }
            set { portName = value; }
        }

        public static int BaudRate
        {
            get { return baudRate; }
            set { baudRate = value; }
        }

        public SerialPort Sp
        {
            get { return sp; }
            set { sp = value; }
        }

        public KinectSensor Sensor
        {
            get { return sensor; }
            set { sensor = value; }
        }

        public WriteableBitmap ColorBitmap
        {
            get { return colorBitmap; }
            set { colorBitmap = value; }
        }

        public WriteableBitmap CroppedBitmap
        {
            get { return croppedBitmap; }
            set { croppedBitmap = value; }
        }

        public byte[] ColorPixels
        {
            get { return colorPixels; }
            set { colorPixels = value; }
        }

        private static string portName = "COM3";
        private static int baudRate = 9600;
        public SerialPort sp = new SerialPort(portName, baudRate, Parity.None, 8, StopBits.One);


        /// <summary>
        /// Active Kinect sensor
        /// </summary>
        private KinectSensor sensor;

        /// <summary>
        /// Bitmap that will hold color information
        /// </summary>
        private WriteableBitmap colorBitmap;

        private WriteableBitmap croppedBitmap;

        /// <summary>
        /// Intermediate storage for the color data received from the camera
        /// </summary>
        private byte[] colorPixels;
    }
}