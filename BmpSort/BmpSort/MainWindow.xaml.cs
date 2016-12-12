//#define BUFFERDEBUG

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Imaging;
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
using System.Threading;
using System.Windows.Interop;
using Accord.IO;
using SerialIO;
using Color = SerialIO.Color;
using PixelFormat = System.Drawing.Imaging.PixelFormat;
using Shape = SerialIO.Shape;


namespace BmpSort
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window, INotifyPropertyChanged
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
            M = new Machine();


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

        public void takePictureRAM()
        {
            //Check access to UI Thread.
            if (Application.Current.Dispatcher.CheckAccess())
            {
                Dispatcher.Invoke(() =>
                {
                    BitmapSource image = kinect.TakePicture(new Int32Rect(125, 0, 110, 200));
                    Image2.Source = image;


                    // create frame from the writable bitmap and add to encoder
                    //encoder.Frames.Add(BitmapFrame.Create(this.colorBitmap));


                    // Decide on taken picture

                    classification = M.decide(IP.ToBitmap(image));
                    //classification = 1;
                    ClassificationLabel.Content = "Class: " + classification;

                    AIO.SendObject(classification == 1 ? Shape.Ball : Shape.NotBall, Color.Unknown); // TODO: Update when the kinect supports color recognition
                });
            }
            else
            {
                //Other wise re-invoke the method with UI thread access
                Application.Current.Dispatcher.Invoke(() => takePictureRAM());
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

                
                Dispatcher.Invoke(() =>
                {
                    BitmapSource image = kinect.TakePicture(new Int32Rect(125, 0, 110, 200));
                    Image2.Source = image;

                    image.Save(path);
                });
            }

            else
            {
                //Other wise re-invoke the method with UI thread access
                Application.Current.Dispatcher.Invoke(takePictureSAVE);
            }
            
        }

        private void ARFFbutton_Click(object sender, RoutedEventArgs e)
        {
            ARFFGenerator arff = new ARFFGenerator(M);
            arff.generate_arff_file(ref progress);
        }

        private void ConnectButton_Click(object sender, RoutedEventArgs e)
        {
            kinect = new Kinect();
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

                    //Message message;
                    //               AIO.AwaitMessage(out message);

                    //   if (message?.Type == MessageType.Command && 
                    //    (message as CommandMessage).Command == Command.TakePicture)
                    //{
                    //	takePictureRAM();
                    //                   //takePictureSAVE();
                    //}

                    takePictureRAM();
                    Thread.Sleep(1000);
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

        public event PropertyChangedEventHandler PropertyChanged;

        protected virtual void OnPropertyChanged([CallerMemberName] string propertyName = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}