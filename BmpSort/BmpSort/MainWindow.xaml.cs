using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Globalization;
using System.IO;
using System.Runtime.CompilerServices;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Media.Imaging;
using SerialIO;
using Shape = SerialIO.Shape;
using Color = SerialIO.Color;

namespace BmpSort
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window, INotifyPropertyChanged
    {
        private Bitmap _takenImage;
        public Bitmap TakenImage
        {
            get { return _takenImage; }
            set
            {
                _takenImage = value;
                OnPropertyChanged();
            }
        }
        
        private Bitmap _cleanedImage;
        public Bitmap CleanedImage
        {
            get { return _cleanedImage; }
            set
            {
                _cleanedImage = value;
                OnPropertyChanged();
            }
        }

        private Shape _class;
        public Shape Class
        {
            get { return _class; }
            set
            {
                _class = value;
                OnPropertyChanged();
            }
        }

        private string _comPort;
        public string ComPort
        {
            get { return _comPort; }
            set
            {
                _comPort = value;
                OnPropertyChanged();
            }
        }

        private string _bytes;
        public string Bytes
        {
            get { return _bytes; }
            set
            {
                _bytes = value;
                OnPropertyChanged();
            }
        }

        public bool CanConnect => _worker == null;

        private BackgroundWorker _worker;
        private BackgroundWorker Worker
        {
            get { return _worker; }
            set
            {
                _worker = value;
                OnPropertyChanged();
                OnPropertyChanged("CanConnect");
            }
        }


        private Kinect _kinect;
        private Machine _m;

        private ArduinoIO _aio;

        public ArduinoIO ArduinoIO
        {
            get { return _aio; }
            set
            {
                _aio = value;
                OnPropertyChanged();
            }
        }

        private const string BackgroundColors = "colors.txt";

        public MainWindow()
        {
            InitializeComponent();
            _m = new Machine();
        }

        private void ConnectButton_Click(object sender, RoutedEventArgs e)
        {
            _kinect = new Kinect();
            ArduinoIO = new ArduinoIO(ComPort);

            Worker = new BackgroundWorker()
            {
                WorkerReportsProgress = true,
                WorkerSupportsCancellation = true
            };
            Worker.DoWork += Worker_DoWork;
            Worker.ProgressChanged += Worker_ProgressChanged;
            Worker.RunWorkerAsync();

#if (BUFFERDEBUG)
            AIO.DataRecived += (o, args) =>
            {
                Console.Write(AIO.ReadExistingBytes());
            };
#else

#endif
        }

        private void Worker_DoWork(object sender, DoWorkEventArgs e)
        {
            var worker = (BackgroundWorker)sender;
            while (!worker.CancellationPending)
            {
                Message message;
                ArduinoIO.AwaitMessage(out message);

                if (message != null &&
                    message.Type == MessageType.Command &&
                    (message as CommandMessage).Command == Command.TakePicture)
                {
                    var image = _kinect.TakePicture(new Rectangle(262, 87, 110, 200));

                    // Decide on taken picture
                    var classification = _m.decide(image) == 1 ? Shape.Ball : Shape.NotBall;
                    ArduinoIO.SendObject(classification, Color.Unknown);
                    // TODO: Update when the kinect supports color recognition

                    worker.ReportProgress(0, image);
                    worker.ReportProgress(1, _m.Currentpicture);
                    worker.ReportProgress(2, classification);


                    //string time = System.DateTime.Now.ToString("hh'-'mm'-'ss", CultureInfo.CurrentUICulture.DateTimeFormat);
                    //string myPhotos = Directory.GetCurrentDirectory();
                    //string path = System.IO.Path.Combine(myPhotos, "picture-" + time + ".bmp");
                    //image.Save(path);
                }
            }
        }

        /// <summary>
        /// We use this function to update the user interface from the background worker.
        /// This might not be the best solution, but it just works™
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Worker_ProgressChanged(object sender, ProgressChangedEventArgs e)
        {
            int percentage = e.ProgressPercentage;

            switch (percentage)
            {
                case 0:
                    TakenImage = e.UserState as Bitmap;
                    break;
                case 1:
                    CleanedImage = e.UserState as Bitmap;
                    break;
                case 2:
                    if (e.UserState is Shape)
                        Class = (Shape)e.UserState;
                    break;
            }
        }

        private void SendByteButton_Click(object sender, RoutedEventArgs e)
        {
            string[] input = Bytes.Split(' ');
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
            SendByteArray(new byte[] { 0x00, 0x01 });
        }

        private void SendByteArray(byte[] bytes)
        {
            List<byte> msg = new List<byte> { 0x62, (byte)bytes.Length };
            msg.AddRange(bytes);

            _aio.SendByte(msg.ToArray());
        }

        public event PropertyChangedEventHandler PropertyChanged;

        protected virtual void OnPropertyChanged([CallerMemberName] string propertyName = null)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}
