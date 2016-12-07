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

        Bitmap currentBitmap;
        Accord.Imaging.BlobCounter blobscounter = new BlobCounter();
        Accord.Imaging.Converters.ImageToArray arrayMaker = new Accord.Imaging.Converters.ImageToArray();
        Accord.Imaging.Converters.ArrayToImage imageMaker = new Accord.Imaging.Converters.ArrayToImage(400, 200);

        Dictionary<int, int> backgrounds = new Dictionary<int, int>();

        #endregion Properties

        public ImageProperties(string fileName)//constructor
        {
            backgrounds.Add(0, 0);
            load_background_colors_textfile("backgrounds.txt");
            load_model_from_dat();
        }

        private void load_model_from_dat() //loads the model data from .dat file
        {
            StreamReader incstream = new StreamReader("model3.dat");
            List<string> datastrings = new List<string>();

            while (!incstream.EndOfStream){
                datastrings.Add(incstream.ReadLine());
            }

            trainingInput = new int[datastrings.Count()][];
            trainingOutput = new int[datastrings.Count()];
            string[] temp3long = new string[3];
            counter = 0;

            foreach (string linje in datastrings){
                temp3long = linje.Split('|');
                add_training_input(int.Parse(temp3long[0]), int.Parse(temp3long[1]), counter);
                trainingOutput[counter] = int.Parse(temp3long[3]);
                counter++;
            }
            incstream.Close();
        }
        private void add_training_input(int a, int b, int place)
        {
            int[] temp = new int[] { a, b, };

            trainingInput[place] = temp;
        }

        #region Functions used when classifying an incomming image
        public int[] get_properties(System.Drawing.Image inputImage)
        {
            int[] result = {0, 0, 0};
            inputImage = clean_background(inputImage);
            result[0] = whitePixels;
            result[1] = blobs;
            result[2] = squaredetected;

            return result;
        }//returns properties for input image, used for model.decide() function
        private System.Drawing.Image clean_background(System.Drawing.Image inputImage)//removes background and makes black/white
        {
            int i = 0;
            Bitmap bitmap = new Bitmap(inputImage);
            Color[] colors = new Color[80000];
            Color[] newImage = new Color[80000];

            whitePixels = 0;
            arrayMaker.Convert(bitmap, out colors);

            foreach (Color farve in colors)
            {
                if (backgrounds.ContainsKey(farve.ToArgb()))
                {
                    newImage[i] = Color.Black;
                }
                else
                {
                    newImage[i] = Color.White;
                    whitePixels++;
                }
                i++;
            }
            if ((whitePixels > 4000) && (whitePixels < 8000))
                {
                    whitePixels = 1;
            }
            else
            {
                whitePixels = 0;
            }
            imageMaker.Convert(newImage, out currentBitmap);
            blobs = blobdetect(currentBitmap);
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
                running[1] = blobdetect(currentBitmap);
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
                running[1] = blobdetect(currentBitmap);
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
                running[1] = blobdetect(currentBitmap);
                running[2] = squaredetected;
                arrayCounter++;
            }
        }
        #endregion Load images for Testdata


        #region detectors
        public int blobdetect(Bitmap input)
        {
            SimpleShapeChecker shapechecker = new SimpleShapeChecker();
            blobscounter.FilterBlobs = true;
            blobscounter.BlobsFilter = null;
            blobscounter.MinHeight = 35;
            blobscounter.MinWidth = 35;

            blobscounter.ProcessImage(input);
            Blob[] blobs = blobscounter.GetObjectsInformation();
            for (int i = 0; i < blobs.Length; i++)
            {
                List<IntPoint> edgepoints = blobscounter.GetBlobsEdgePoints(blobs[i]);
                if ((edgepoints.Count>=4)&&(blobs[i].Area >100))
                {
                    List<IntPoint> corners = PointsCloud.FindQuadrilateralCorners(edgepoints);
                    if (shapechecker.IsQuadrilateral(edgepoints,out corners))
                    {
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