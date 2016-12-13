using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Accord;
using Accord.Imaging;
using System.Drawing;
using System.IO;
using Accord.Math.Geometry;
using System.Windows;

namespace BmpSort
{
    class ImageProperties
    {
        #region Properties  

        int counter = 0;
        int blobs = 0;
        int squaredetected = 0;
        int isCircle;
        int filesLoaded = 0;
        int whitePixels = 0;
        public int[] trainingOutput { get; set; }
        public int[][] trainingInput { get; set; }
        public int arrayCounter { get; set; }
        public int[] properties= new int[3];

        string[] emptyImages;
        string[] background;
        string[] ballImages;
        string[] errors;

        public Bitmap currentBitmap;
        Accord.Imaging.BlobCounter blobscounter = new BlobCounter();
        Accord.Imaging.Converters.ImageToArray arrayMaker = new Accord.Imaging.Converters.ImageToArray();
        Accord.Imaging.Converters.ArrayToImage imageMaker = new Accord.Imaging.Converters.ArrayToImage(110, 200);

        Dictionary<int, int> backgrounds = new Dictionary<int, int>();

        #endregion Properties

        public ImageProperties(string fileName)//constructor
        {
            backgrounds.Add(Color.Black.ToArgb(), Color.Black.ToArgb());
            load_background_colors_textfile(fileName);
            load_model_from_dat();
        }

        private void load_model_from_dat() //loads the model data from .dat file
        {
            StreamReader incstream = new StreamReader(@"Resources\bayes.mod");
            List<string> datastrings = new List<string>();

            while (!incstream.EndOfStream){
                datastrings.Add(incstream.ReadLine());
            }

            trainingInput = new int[datastrings.Count()][];
            trainingOutput = new int[datastrings.Count()];
            string[] temp5long = new string[5];
            counter = 0;

            foreach (string linje in datastrings){
                temp5long = linje.Split('|');
                add_training_input(int.Parse(temp5long[0]), int.Parse(temp5long[1]), int.Parse(temp5long[2]), int.Parse(temp5long[3]), counter);
                trainingOutput[counter] = int.Parse(temp5long[4]);
                counter++;
            }
            incstream.Close();
        }
        private void add_training_input(int a, int b, int c, int d, int place)
        {
            int white = 0;
            if ((d<3900)&&(d>2700))
            {
                white = 1;
            }
            int[] temp = new int[] { a, b, c, white};

            trainingInput[place] = temp;
        }

        #region Functions used when classifying an incomming image
        public int[] get_properties(System.Drawing.Image inputImage)
        {
            int[] result = {0, 0, 0, 0};
            blobs = 0;
            isCircle = 0;
            squaredetected = 0;
            whitePixels = 0;

            clean_background(inputImage);

            result[0] = blobs;
            result[1] = isCircle;
            result[2] = squaredetected;
            result[3] = whitePixels;

            return result;
        }//returns properties for input image, used for model.decide() function
        private System.Drawing.Image clean_background(System.Drawing.Image inputImage)//removes background and makes black/white
        {
            int i = 0;
            blobs = 0;
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
            if ((pixels<3900)&&(pixels>2700))
            {
                whitePixels = 1;
            }
            imageMaker.Convert(newImage, out currentBitmap);
            blobs = blobdetection(currentBitmap);
            return currentBitmap;
        }
        #endregion Functions used when classifying an incomming image

        #region Functions concerning background colors
        private void load_background_colors_textfile(string fileName)
        {
            int temp;
            StreamReader sr = new StreamReader(fileName);
            while (!sr.EndOfStream)
            {
                temp = Int32.Parse(sr.ReadLine());
                if (!backgrounds.ContainsKey(temp))
                {
                    backgrounds.Add(temp, temp);
                }
                
            }
        }//load backgroundcolors from file
        private void load_background_colors(string path)
        {
            background = Directory.GetFiles(path, "*.*");
            foreach (string fil in background)
            {
                System.Drawing.Image image = System.Drawing.Image.FromFile(fil);
                add_ignore_colors(image);
            }
        }//load backgroundcolors from images
        private void add_ignore_colors(System.Drawing.Image image)//adds entire image to background colors
        {
            Bitmap bitmap = new Bitmap(image);
            Color[] colors = new Color[80000];
            arrayMaker.Convert(bitmap, out colors);
            foreach (Color farve in colors)
            {
                if (!backgrounds.ContainsKey(farve.ToArgb()))
                {
                    backgrounds.Add(farve.ToArgb(), farve.ToArgb());
                }
            }
        }
        #endregion Functions concerning background colors

        #region Load images for Testdata
        public void load_files() //gets training image filenames from folders
        {
            emptyImages = Directory.GetFiles("Images/Empty/", "*.*");
            ballImages = Directory.GetFiles("Images/Blue/", "*.*");
            errors = Directory.GetFiles("Images/Error/", "*.*");
            filesLoaded = (emptyImages.Length + ballImages.Length + errors.Length);
        }
        public void load_ball_training(string path)
        {
            int[] running = {0, 0, 0};
            ballImages = Directory.GetFiles(path, "*.*");
            foreach (string fil in ballImages)
            {
                System.Drawing.Image image = System.Drawing.Image.FromFile(fil);
                clean_background(image);
                running[0] = whitePixels;
                running[1] = blobdetection(currentBitmap);
                running[2] = squaredetected;
                arrayCounter++;
            }
        }
        public void load_empty_training(string path)
        {
            int[] running = {0, 0, 0};
            emptyImages = Directory.GetFiles(path, "*.*");
            foreach (string fil in emptyImages)
            {
                System.Drawing.Image image = System.Drawing.Image.FromFile(fil);
                clean_background(image);
                running[0] = whitePixels;
                running[1] = blobdetection(currentBitmap);
                running[2] = squaredetected;
                arrayCounter++;
            }
        }
        public void load_error_training(string path)
        {
            int[] running = {0, 0, 0};
            errors = Directory.GetFiles(path, "*.*");
            foreach (string fil in errors)
            {
                System.Drawing.Image image = System.Drawing.Image.FromFile(fil);
                clean_background(image);
                running[0] = whitePixels;
                running[1] = blobdetection(currentBitmap);
                running[2] = squaredetected;
                arrayCounter++;
            }
        }
        #endregion Load images for Testdata


        #region detectors
        public int blobdetection(Bitmap input)
        {
            SimpleShapeChecker circlechecker = new SimpleShapeChecker();
            SimpleShapeChecker squarechecker = new SimpleShapeChecker();
            squarechecker.AngleError = 7;                  //default=7 graders tolerance
            squarechecker.LengthError = 0.1f;               //default=0.1 (10%) siders længdeforskel
            squarechecker.MinAcceptableDistortion = 0.6f;
            squarechecker.RelativeDistortionLimit = 0.04f;

            circlechecker.MinAcceptableDistortion = 1.1f;   //default=0.5 (in pixels)
            circlechecker.LengthError = 0.2f;
            circlechecker.RelativeDistortionLimit = 0.04f;  //default=0.03 (3%)

            blobscounter.FilterBlobs = true;
            blobscounter.BlobsFilter = null;
            blobscounter.MinHeight = 35;
            blobscounter.MinWidth = 35;

            Graphics g = Graphics.FromImage(input);
            System.Drawing.Pen redPen = new System.Drawing.Pen(System.Drawing.Color.Red, 2);

            blobscounter.ProcessImage(input);
            Blob[] blobs = blobscounter.GetObjectsInformation();
            for (int i = 0; i < blobs.Length; i++)
            {
                List<IntPoint> edgepoints = blobscounter.GetBlobsEdgePoints(blobs[i]);
                if ((edgepoints.Count>=4)&&(blobs[i].Area >100))
                {
                    Accord.Point center;
                    float radius;
                    #region iscircle
                    if (circlechecker.IsCircle(edgepoints, out center, out radius))
                    {
                        isCircle = 1;
                        g.DrawEllipse(redPen,
                   (int)(center.X - radius),
                   (int)(center.Y - radius),
                   (int)(radius * 2),
                   (int)(radius * 2));

                    }
                    #endregion iscircle
                    #region issquare
                    List<IntPoint> corners = PointsCloud.FindQuadrilateralCorners(edgepoints);
                    if (squarechecker.IsQuadrilateral(edgepoints,out corners))
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

                        g.DrawPolygon(redPen, points);
                        squaredetected = 1;
                    }
                    else
                    {
                        squaredetected = 0;
                    }
                }
                else
                {
                    squaredetected = 0;
                }
            }
            #endregion issquare
            if (blobscounter.ObjectsCount == 1)
            {
                return 1;
            }
            else
            {
                return 0;
            }          
        } 
        #endregion detectors
    }
}