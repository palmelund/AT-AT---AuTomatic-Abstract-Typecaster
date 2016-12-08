//#define BUFFERDEBUG

using System;
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
using SerialIO;


namespace BmpSort
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        #region Properties
        private Kinect kinect;

        private ImageProcessing IP;

        private Machine M;

        private int progress = 0;

        private ArduinoIO AIO;

        private Task t;

        private int classification;

        private string backgroundColors = "colors.txt";
        #endregion Properties

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
            IP = new ImageProcessing();
            M = new Machine(backgroundColors);


            ProgressBarARFF.Value = progress;
        }

        /// <summary>
        /// Execute shutdown tasks
        /// </summary>
        /// <param name="sender">object sending the event</param>
        /// <param name="e">event arguments</param>
        private void WindowClosing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            //kinect?.Sensor?.Stop();
            //AIO?.CloseConnection();
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
                                kinect.ColorBitmap.PixelWidth * sizeof(int),
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
                        kinect.ColorBitmap.PixelWidth * sizeof(int),
                        0);
                }
            }
        }

        /*
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
*/
        public void takePictureRAMOld()
        {
            //Check access to UI Thread.
            if (Application.Current.Dispatcher.CheckAccess())
            {
                // create a png bitmap encoder which knows how to save a .png file
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


                    // Decide on taken picture

                    classification = M.decide(IP.ToBitmap(kinect.CroppedBitmap));
                    //classification = 1;
                    ClassificationLabel.Content = "Class: " + classification;


                    // Add picture to UI
                    BitmapEncoder encoder = new BmpBitmapEncoder();
                    encoder.Frames.Add(BitmapFrame.Create(kinect.CroppedBitmap));
                    this.Image2.Source = encoder.Frames.ElementAt(encoder.Frames.Count - 1);
                });
            }
            else
            {
                //Other wise re-invoke the method with UI thread access
                Application.Current.Dispatcher.Invoke(new System.Action(() => takePictureRAM()));
            }
        }

        public void takePictureRAM()
        {
            //Check access to UI Thread.
            if (Application.Current.Dispatcher.CheckAccess())
            {
                // create a png bitmap encoder which knows how to save a .png file
                if (null == kinect.Sensor)
                {
                    //this.statusBarText.Text = Properties.Resources.ConnectDeviceFirst;
                    return;
                }
                Dispatcher.Invoke(() =>
                {
                    cropImage();

                    // create frame from the writable bitmap and add to encoder
                    //encoder.Frames.Add(BitmapFrame.Create(this.colorBitmap));


                    // Decide on taken picture

                    classification = M.decide(IP.ToBitmap(kinect.CroppedBitmap));
                    classification = 1;
                    //ClassificationLabel.Content = "Class: " + classification;


                    // Add picture to UI
                    BitmapEncoder encoder = new BmpBitmapEncoder();
                    encoder.Frames.Add(BitmapFrame.Create(kinect.CroppedBitmap));
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

                string myPhotos = "";
               
                string path = System.IO.Path.Combine(myPhotos, "picture-" + time + ".bmp");


                if (null == kinect.Sensor)
                {
                    //this.statusBarText.Text = Properties.Resources.ConnectDeviceFirst;
                    return;
                }
                Dispatcher.Invoke(() =>
                {
                    cropImage();

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
                Application.Current.Dispatcher.Invoke(takePictureSAVE);
            }
            
        }

        private void cropImage()
        {
            kinect.CroppedBitmap = IP.CopyPixelsTo(kinect.ColorBitmap,
                        new Int32Rect(140, 100, 400, 200),
                        new Int32Rect(125, 0, 110, 200));
        }

        private void ARFFbutton_Click(object sender, RoutedEventArgs e)
        {
            ARFFGenerator arff = new ARFFGenerator(M);
            arff.generate_arff_file(ref progress);
        }

        private void ConnectButton_Click(object sender, RoutedEventArgs e)
        {
            kinect = new Kinect();
            kinect.Sensor.ColorFrameReady += SensorColorFrameReady;
            AIO = new ArduinoIO(SerialPortTextBox.Text);
            //Lav thread til Serial IO listening.

#if (BUFFERDEBUG)
            AIO.DataRecived += (o, args) =>
            {
                Console.Write(AIO.ReadExistingBytes());
            };
#else
            t = Task.Run(() =>
            {
                while (true)
				{
					Message message;
                    AIO.AwaitMessage(out message);

				    if (message?.Type == MessageType.Command && 
					    (message as CommandMessage).Command == Command.TakePicture)
					{
						//takePictureRAM();
                        takePictureSAVE();
					}
                }
            });
#endif
            
            // Set the image we display to point to the bitmap where we'll put the image data
            Image1.Source = kinect.ColorBitmap;

        }

        private void SendByteButton_Click(object sender, RoutedEventArgs e)
        {
            string[] input = SerialByteTextBox.Text.Split(' ');
            
            byte[] bytes = new byte[input.Length];
            
            for (int i = 0; i < input.Length; i++)
            {
                byte b;
                bool parsable = byte.TryParse(input[i], out b);
                if (!parsable) throw new ArgumentException();
                bytes[i] = b;
            }

            SendByteArray(bytes);
        }

        private void SendStartCommandButton_OnClick(object sender, RoutedEventArgs e)
        {
            SendByteArray(new byte[]{0x00, 0x01});
        }

        private void SendByteArray(byte[] bytes)
        {
            List<byte> msg = new List<byte> { 0x62, (byte)bytes.Length };
            msg.AddRange(bytes);

            AIO.SendByte(msg.ToArray());
        }
    }
}