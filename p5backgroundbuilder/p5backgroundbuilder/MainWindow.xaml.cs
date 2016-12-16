using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Drawing;
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
using System.IO;
using Accord;
using Accord.Imaging;
using Accord.Math.Geometry;

namespace p5backgroundbuilder
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
            backgrounds.Add(System.Drawing.Color.Black.ToArgb(),System.Drawing.Color.Black.ToArgb());
            load_files_button();
            
        }

        #region Fields
        string[] backgroundfiles;
        string[] ballfiles;
        string[] dicefiles;
        List<float[]> inputs = new List<float[]>();
        List<float> outputs = new List<float>();
        List<System.Drawing.Image> backgroundImageList = new List<System.Drawing.Image>();        
        List<System.Drawing.Image> ballImageList = new List<System.Drawing.Image>();        
        List<System.Drawing.Image> diceImageList = new List<System.Drawing.Image>();
        int backcounter = 0;
        int ballcounter = 0;
        int dicecounter = 0;
        int squaredetected = 0;
        int isCircle = 0;
        int whitepixels = 0;
        int blob = 0;
        Bitmap currentBitmap;
        Accord.Imaging.Converters.ImageToArray arrayMaker = new Accord.Imaging.Converters.ImageToArray();
        Accord.Imaging.Converters.ArrayToImage imageMaker = new Accord.Imaging.Converters.ArrayToImage(110, 200);
        Accord.Imaging.BlobCounter blobscounter = new BlobCounter();
        Dictionary<int, int> backgrounds = new Dictionary<int, int>();

        #endregion Fields

        private void load_files_button() //Reads backgrounds from .txt file, and loads images in to lists
        {
            load_background_from_file();
            load_files(backgroundfiles, "smallimages/empty/",backgroundImageList);
            load_files(ballfiles, "smallimages/balls/",ballImageList);
            load_files(dicefiles, "smallimages/dice/",diceImageList);
            nextBackgroundImage.Source = GetImageStream(backgroundImageList[ballcounter]);
        }
        private void load_files(string[] array, string path, List<System.Drawing.Image> list)
        {
            array = Directory.GetFiles(path, "*.*");
            foreach (string fil in array)
            {
                list.Add(System.Drawing.Image.FromFile(fil));
            }
        }       

        private void load_next_background()
        {
            if (backcounter < backgroundImageList.Count)
            {
                backcounter++;
                BackCountlabel.Content = ("Background: " + backcounter + "/" + backgroundImageList.Count);
            }

            nextBackgroundImage.Source = GetImageStream(backgroundImageList[ballcounter]);
        }
        private void add_background_to_base(System.Drawing.Image imagefile)
        {
            Bitmap bitmap = new Bitmap(imagefile);
            System.Drawing.Color[] colors = new System.Drawing.Color[80000];
            arrayMaker.Convert(bitmap, out colors);

            foreach (System.Drawing.Color farve in colors)
            {
                if (!backgrounds.ContainsKey(farve.ToArgb()))
                {
                    backgrounds.Add(farve.ToArgb(), farve.ToArgb());
                }
                else
                {

                }
            }
            
        }
        private void addall()
        {
            foreach (System.Drawing.Image image in backgroundImageList)
            {
                add_background_to_base(image);
            }
            update_backgrounds();
            BackCountlabel.Content = ("Background: " + backcounter + "/" + backgroundImageList.Count);
        }
        private void load_background_from_file()
        {
            StreamReader reader = new StreamReader("backgrounds.txt");
            while (!reader.EndOfStream)
            {
                int value = int.Parse(reader.ReadLine());
                backgrounds.Add(value, value);
            }
            reader.Close();
        }
        private void load_next_ball()
        {
            if (ballcounter<ballImageList.Count-1)
            {
                ballcounter++;
                Ballcountlabel.Content = ("Ball: " + ballcounter + "/" + ballImageList.Count);
            }
            
            originalBallImage.Source = GetImageStream(ballImageList[ballcounter]);
            update_backgrounds();
            
        }
        private void load_prev_ball()
        {
            if (ballcounter>0)
            {
                ballcounter--;
                Ballcountlabel.Content = ("Ball: " + ballcounter + "/" + ballImageList.Count);
            }
            
            originalBallImage.Source = GetImageStream(ballImageList[ballcounter]);
            clean_background_small(ballImageList[ballcounter]);
            cleanedBallImage.Source = GetImageStream(currentBitmap);
        }
        private void load_next_dice()
        {
            if (dicecounter<diceImageList.Count-1)
            {
                dicecounter++;
                Dicelabel.Content = ("Dice: " + dicecounter + "/" + diceImageList.Count);
            }
            
            originalDiceImage.Source = GetImageStream(diceImageList[dicecounter]);
            update_backgrounds();
        }
        private void load_prev_dice()
        {
            if (dicecounter > 0)
            {
                dicecounter--;
                Dicelabel.Content = ("Dice: " + dicecounter + "/" + diceImageList.Count);
            }

            originalDiceImage.Source = GetImageStream(diceImageList[dicecounter]);
            clean_background_small(diceImageList[dicecounter]);
            cleanedDiceImage.Source = GetImageStream(currentBitmap);
        }
        private void load_files_button(object sender, RoutedEventArgs e)
        {
            load_files_button();
            countlabel.Content = (backgroundImageList.Count+ballImageList.Count+diceImageList.Count);
        }
        private void update_backgrounds()// recleans backgrounds after more color is added to bacground dictionary
        {
            clean_background_small(ballImageList[ballcounter]);
            cleanedBallImage.Source = GetImageStream(currentBitmap);
            clean_background_small(diceImageList[dicecounter]);
            cleanedDiceImage.Source = GetImageStream(currentBitmap);
        }
        public static BitmapSource GetImageStream(System.Drawing.Image myImage) //converts a Image to a BitmapSource
        {
            Bitmap bitmap = new Bitmap(myImage);
            IntPtr bmpPt = bitmap.GetHbitmap();
            BitmapSource bitmapSource =
             System.Windows.Interop.Imaging.CreateBitmapSourceFromHBitmap(
                   bmpPt,
                   IntPtr.Zero,
                   Int32Rect.Empty,
                   BitmapSizeOptions.FromEmptyOptions());

            //freeze bitmapSource and clear memory to avoid memory leaks
            bitmapSource.Freeze();
            return bitmapSource;
        }
        private int blobdetection(Bitmap input)//Performs blobcount, iscirce and isquadrilateral on the input and sets values
        {
            SimpleShapeChecker circleChecker = new SimpleShapeChecker();
            SimpleShapeChecker squareChecker = new SimpleShapeChecker();
            squareChecker.AngleError = 7;                  //default=7 graders tolerance
            squareChecker.LengthError = 0.2f;               //default=0.1 (10%) siders længdeforskel
            squareChecker.MinAcceptableDistortion = 0.6f;
            squareChecker.RelativeDistortionLimit = 0.045f;

            circleChecker.MinAcceptableDistortion = 0.6f;   //default=0.5 (in pixels)
            circleChecker.RelativeDistortionLimit = 0.03f;  //default=0.03 (3%)

            blobscounter.FilterBlobs = true;
            blobscounter.BlobsFilter = null;
            blobscounter.MinHeight = 60;
            blobscounter.MinWidth = 60;
            squaredetected = 0;
            isCircle = 0;
            Graphics g = Graphics.FromImage(input);
            System.Drawing.Pen redPen = new System.Drawing.Pen(System.Drawing.Color.Red, 2);
            System.Drawing.Rectangle recta = new System.Drawing.Rectangle(0, 0, input.Width, input.Height);
            blobscounter.ProcessImage(input);
            Blob[] blobs = blobscounter.GetObjectsInformation();
            for (int i = 0; i < blobs.Length; i++)
            {
                Accord.Point center;
                float radius;
                List<IntPoint> edgepoints = blobscounter.GetBlobsEdgePoints(blobs[i]);
                List<IntPoint> corners;
                #region iscircle
                if (circleChecker.IsCircle(edgepoints, out center, out radius))
                {
                    isCircle = 1;
                    g.DrawEllipse(redPen,
               (int)(center.X - radius),
               (int)(center.Y - radius),
               (int)(radius * 2),
               (int)(radius * 2));

                }
                #endregion iscircle
                #region isSquare
                if (squareChecker.IsQuadrilateral(edgepoints, out corners))
                {
                    System.Drawing.Point[] points = new System.Drawing.Point[4];
                    points[0].X = corners[0].X;
                    points[0].Y = corners[0].Y;
                    points[1].X = corners[1].X;
                    points[1].Y = corners[1].Y;
                    points[2].X = corners[2].X;
                    points[2].Y = corners[2].Y;
                    points[3].X = corners[3].X;
                    points[3].Y = corners[3].Y;

                    g.DrawPolygon(redPen,points);
                    squaredetected = 1;
                }
                #endregion isSquare
            }

            redPen.Dispose();
            g.Dispose();
            if (blobscounter.ObjectsCount == 1)
            {
                return 1;
            }
            else
            {
                return 0;
            }
        } 
        private System.Drawing.Bitmap clean_background_small(System.Drawing.Image inputImage) //removes background and converts to black/white
        {          
            int i = 0;
            blob = 0;
            int pixels = 0;
            Bitmap bitmap = new Bitmap(inputImage);
            System.Drawing.Color[] colors = new System.Drawing.Color[22000];
            System.Drawing.Color[] newImage = new System.Drawing.Color[22000];

            arrayMaker.Convert(bitmap, out colors);

            foreach (System.Drawing.Color farve in colors)
            {
                if (backgrounds.ContainsKey(farve.ToArgb()))
                {
                    newImage[i] = System.Drawing.Color.Black;
                }
                else
                {
                    newImage[i] = System.Drawing.Color.White;                    
                    pixels++;
                }
                i++;
            }
            whitepixels = pixels;
            imageMaker.Convert(newImage, out currentBitmap);
            blob=blobdetection(currentBitmap);
            return currentBitmap;
        }
        private void output_color_file()// Writes the background data currently loaded out to backgroundsout.txt
        {
            StreamWriter writer = new StreamWriter("backgroundsout.txt");
            foreach (KeyValuePair<int,int> set in backgrounds)
            {
                writer.WriteLine(set.Key);
            }
            writer.Close();
        }
        private void output_model_file()// writes the testdata parameters out to file
        {
            StreamWriter writer = new StreamWriter("bayes.mod");
            foreach (System.Drawing.Image image in ballImageList)
            {
                clean_background_small(image);
                writer.WriteLine(blob + "|" + isCircle + "|" + squaredetected + "|" + whitepixels + "|" + 1);
            }

            foreach (System.Drawing.Image image in diceImageList)
            {
                clean_background_small(image);
                writer.WriteLine(blob + "|" + isCircle + "|" + squaredetected + "|" + whitepixels + "|" + 0);
            }
            writer.Close();
        }

        // Simple functionality for the UI buttons, passing argument calls when clicked
        #region Buttons
        private void next_ball_button(object sender, RoutedEventArgs e)
        {
            load_next_ball();
        }
        private void prev_ball_button(object sender, RoutedEventArgs e)
        {
            load_prev_ball();
        }
        private void next_dice_button(object sender, RoutedEventArgs e)
        {
            load_next_dice();
        }
        private void prev_dice_button(object sender, RoutedEventArgs e)
        {
            load_prev_dice();
        }
        private void add_next_back(object sender, RoutedEventArgs e)
        {
            if (backcounter<backgroundImageList.Count)
            {
                add_background_to_base(backgroundImageList[backcounter]);               
            }
            load_next_background();
            update_backgrounds();
        }
        private void next_back(object sender, RoutedEventArgs e)
        {
            load_next_background();
        }
        private void add_all_back(object sender, RoutedEventArgs e)
        {
            foreach (System.Drawing.Image image in backgroundImageList)
            {
                add_background_to_base(image);
            }
            update_backgrounds();
            BackCountlabel.Content = ("Background: " + backcounter + "/" + backgroundImageList.Count);
        }
        private void build_arff(object sender, RoutedEventArgs e)
        {
            StreamWriter writer = new StreamWriter("output.arff");
            writer.WriteLine("@RELATION ball");
            writer.WriteLine(" ");
            writer.WriteLine("@ATTRIBUTE blobcount NUMERIC");
            writer.WriteLine("@ATTRIBUTE circle NUMERIC");
            writer.WriteLine("@ATTRIBUTE square NUMERIC");
            writer.WriteLine("@ATTRIBUTE whiteamount NUMERIC");
            writer.WriteLine("@ATTRIBUTE class {notball,ball}");
            writer.WriteLine(" ");
            writer.WriteLine("@DATA");

            foreach (System.Drawing.Image image in ballImageList)
            {
                clean_background_small(image);
                writer.WriteLine(blob + "," + isCircle + "," + squaredetected + "," + whitepixels + ",ball");
            }

            foreach (System.Drawing.Image image in diceImageList)
            {
                clean_background_small(image);
                writer.WriteLine(blob + "," + isCircle + "," + squaredetected + "," + whitepixels + ",notball");
            }
            writer.Close();
            
        }
        private void output_button(object sender, RoutedEventArgs e)
        {
            output_color_file();
            output_model_file();
        }
        #endregion Buttons
    }
}
