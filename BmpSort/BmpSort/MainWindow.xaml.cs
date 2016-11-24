﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using Microsoft.Kinect;
using System.IO;
using System.Globalization;
using Accord;
using System.IO.Ports;
using System.Runtime.CompilerServices;

namespace BmpSort
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private Kinect kinect;

        private ImageProcessing IP;

        private Machine M;

        private int progress = 0;

        /// <summary>
        /// Initializes a new instance of the MainWindow class.
        /// </summary>
        public MainWindow()
        {
            InitializeComponent();
        }


        /// <summary>
        /// Execute startup tasks
        /// </summary>
        /// <param name="sender">object sending the event</param>
        /// <param name="e">event arguments</param>
        private void WindowLoaded(object sender, RoutedEventArgs e)
        {
            kinect = new Kinect();
            kinect.Sensor.ColorFrameReady += SensorColorFrameReady;
            IP = new ImageProcessing();
            //M = new Machine(System.IO.Path.GetFullPath(System.IO.Path.Combine(System.AppDomain.CurrentDomain.BaseDirectory, "..\\..\\", "Images\\Background")));
            kinect.Sp.DataReceived += portReceiveData; // Add DataReceived Event Handler
            kinect.Sp.Open();
            // Set the image we display to point to the bitmap where we'll put the image data
            Image1.Source = kinect.ColorBitmap;
            ProgressBarARFF.Value = progress;
            // Add an event handler to be called whenever there is new color frame data
        }

        /// <summary>
        /// Execute shutdown tasks
        /// </summary>
        /// <param name="sender">object sending the event</param>
        /// <param name="e">event arguments</param>
        private void WindowClosing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            if (null != kinect.Sensor)
            {
                kinect.Sensor.Stop();
            }
            kinect.sp.Close();
        }

        /// <summary>
        /// Event handler for Kinect sensor's ColorFrameReady event
        /// </summary>
        /// <param name="sender">object sending the event</param>
        /// <param name="e">event arguments</param>
        private void SensorColorFrameReady(object sender, ColorImageFrameReadyEventArgs e)
        {
            using (ColorImageFrame colorFrame = e.OpenColorImageFrame())
            {
                if (colorFrame != null)
                {
                    // Copy the pixel data from the image to a temporary array
                    colorFrame.CopyPixelDataTo(kinect.ColorPixels);

                    Boolean Error = true;

                    // Write the pixel data into our bitmap
                    while (Error == true)
                    {
                        try
                        {
                            Error = false;
                            kinect.ColorBitmap.WritePixels(
                                new Int32Rect(0, 0, kinect.ColorBitmap.PixelWidth, kinect.ColorBitmap.PixelHeight),
                                kinect.ColorPixels,
                                kinect.ColorBitmap.PixelWidth*sizeof(int),
                                0);
                        }
                        catch (System.Runtime.InteropServices.COMException)
                        {
                            Error = true;
                        }
                    }
                }
            }
        }

        private void SensorColorPictureFrameReady(object sender, ColorImageFrameReadyEventArgs e)
        {
            using (ColorImageFrame colorFrame2 = e.OpenColorImageFrame())
            {
                if (colorFrame2 != null)
                {
                    // Copy the pixel data from the image to a temporary array
                    colorFrame2.CopyPixelDataTo(kinect.ColorPixels);

                    // Write the pixel data into our bitmap
                    kinect.ColorBitmap.WritePixels(
                        new Int32Rect(0, 0, kinect.ColorBitmap.PixelWidth, kinect.ColorBitmap.PixelHeight),
                        kinect.ColorPixels,
                        kinect.ColorBitmap.PixelWidth*sizeof(int),
                        0);
                }
            }
        }


        private void portReceiveData(object sender, SerialDataReceivedEventArgs e)
        {
            // Serial port from arduino assigned
            SerialPort portArduino = (SerialPort) sender;

            //Read from arduino
            string input = portArduino.ReadTo("end");
            //Split input to get correct command
            string[] data = input.Split(':');
            //Switch on command to call correct function
            switch (data[1].Trim())
            {
                case "1":
                {
                    takePictureRAM();
                    break;
                }
                case "2":
                {
                    takePictureSAVE();
                    break;
                }
                default:
                {
                    //Default something.

                    break;
                }
            }
        }

        public void takePictureRAM()
        {
            //Check access to UI Thread.
            if (Application.Current.Dispatcher.CheckAccess())
            {
                // create a png bitmap encoder which knows how to save a .png file
                BitmapEncoder encoder = new PngBitmapEncoder();

                if (null == kinect.Sensor)
                {
                    //this.statusBarText.Text = Properties.Resources.ConnectDeviceFirst;
                    return;
                }
                Dispatcher.Invoke(() =>
                {
                    kinect.CroppedBitmap = IP.CopyPixelsTo(kinect.ColorBitmap,
                        new Int32Rect(140, 100, 400, 200),
                        new Int32Rect(0, 0, 400, 200));

                    // create frame from the writable bitmap and add to encoder
                    //encoder.Frames.Add(BitmapFrame.Create(this.colorBitmap));
                    encoder.Frames.Add(BitmapFrame.Create(kinect.CroppedBitmap));

                    // add frame to the window
                    this.Image2.Source = encoder.Frames.ElementAt(encoder.Frames.Count - 1);
                });
            }
            else
            {
                //Other wise re-invoke the method with UI thread access
                Application.Current.Dispatcher.Invoke(new System.Action(() => takePictureRAM()));
            }
        }

        public void takePictureSAVE()
        {
            //Check access to UI Thread.
            if (Application.Current.Dispatcher.CheckAccess())
            {
                // create a png bitmap encoder which knows how to save a .png file
                BitmapEncoder encoder = new BmpBitmapEncoder();
                string time = System.DateTime.Now.ToString("hh'-'mm'-'ss", CultureInfo.CurrentUICulture.DateTimeFormat);

                string myPhotos = "C:/Users/bogi1/Desktop/UNI/S5/Projekt/TrainingData/Background";

                string path = System.IO.Path.Combine(myPhotos, "Background-" + time + ".bmp");


                if (null == kinect.Sensor)
                {
                    //this.statusBarText.Text = Properties.Resources.ConnectDeviceFirst;
                    return;
                }
                Dispatcher.Invoke(() =>
                {
                    kinect.CroppedBitmap = IP.CopyPixelsTo(kinect.ColorBitmap,
                        new Int32Rect(140, 100, 400, 200),
                        new Int32Rect(0, 0, 400, 200));

                    // create frame from the writable bitmap and add to encoder
                    //encoder.Frames.Add(BitmapFrame.Create(this.colorBitmap));
                    encoder.Frames.Add(BitmapFrame.Create(kinect.CroppedBitmap));

                    // add frame to the window
                    this.Image2.Source = encoder.Frames.ElementAt(encoder.Frames.Count - 1);


                    // write the new file to disk
                    try
                    {
                        using (FileStream fs = new FileStream(path, FileMode.Create))
                        {
                            encoder.Save(fs);
                        }

                        //this.statusBarText.Text = string.Format(CultureInfo.InvariantCulture, "{0} {1}", Properties.Resources.ScreenshotWriteSuccess, path);
                    }
                    catch (IOException)
                    {
                        //this.statusBarText.Text = string.Format(CultureInfo.InvariantCulture, "{0} {1}", Properties.Resources.ScreenshotWriteFailed, path);
                    }
                });
            }

            else
            {
                //Other wise re-invoke the method with UI thread access
                Application.Current.Dispatcher.Invoke(new System.Action(() => takePictureSAVE()));
            }
        }

        private void Trainbutton_Click(object sender, RoutedEventArgs e)
        {
            M =
                new Machine(
                    System.IO.Path.GetFullPath(System.IO.Path.Combine(System.AppDomain.CurrentDomain.BaseDirectory,
                        "..\\..\\", "Images\\Background")));
            string path =
                System.IO.Path.GetFullPath(System.IO.Path.Combine(System.AppDomain.CurrentDomain.BaseDirectory,
                    "..\\..\\", "Images"));
            M.train_model(System.IO.Path.Combine(path, "Ball"), System.IO.Path.Combine(path, "Empty"),
                System.IO.Path.Combine(path, "Error"));
        }

        private void ARFFbutton_Click(object sender, RoutedEventArgs e)
        {
            ARFFGenerator arff = new ARFFGenerator(M);
            arff.generate_arff_file(ref progress);
        }
    }
}