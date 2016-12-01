using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Accord.Imaging;
using System.Drawing;
using System.IO;

namespace BmpSort
{
    class ImageProperties
    {
        #region Properties  

        int counter = 0;
        int blobs = 0;
        int corners = 0;
        int filesLoaded = 0;
        int whitePixels = 0;
        int[] properties;


        string[] emptyImages;
        string[] background;
        string[] ballImages;
        string[] errors;


        Bitmap currentBitmap;
        Accord.Imaging.Converters.ImageToArray arrayMaker = new Accord.Imaging.Converters.ImageToArray();
        Accord.Imaging.Converters.ArrayToImage imageMaker = new Accord.Imaging.Converters.ArrayToImage(400, 200);

        //List<double[]> valueList = new List<double[]>();
        Dictionary<int, int> backgrounds = new Dictionary<int, int>();

        #endregion Properties

        public ImageProperties(string fileName)
        {
            backgrounds.Add(0, 0);
            load_background_colors_textfile(fileName);
        }

        public int[] trainingOutput { get; set; }
        public int[][] trainingInput { get; set; }
        public int arrayCounter { get; set; }

        public int[] get_properties(System.Drawing.Image inputImage)
        {
            int[] result = {0, 0, 0};
            inputImage = clean_background(inputImage);
            result[0] = whitePixels;
            result[1] = blobs;
            result[2] = corners;

            return result;
        }


        // Fjern Blob+Corner detection og flyt i ny funktion
        private System.Drawing.Image clean_background(System.Drawing.Image inputImage)
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

            imageMaker.Convert(newImage, out currentBitmap);
            blobs = blobdetect(currentBitmap);
            corners = cornerdetect(currentBitmap);
            return currentBitmap;
        }

        private void load_background_colors(string path)
        {
            background = Directory.GetFiles(path, "*.*");
            foreach (string fil in background)
            {
                System.Drawing.Image image = System.Drawing.Image.FromFile(fil);
                add_ignore_colors(image);
            }
        }

        private void load_background_colors_textfile(string fileName)
        {
            int temp;
            StreamReader sr = new StreamReader(fileName);
            while (!sr.EndOfStream)
            {
                temp = Int32.Parse(sr.ReadLine());
                backgrounds.Add(temp, temp);
            }
        }

        private void add_ignore_colors(System.Drawing.Image image)
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
                running[2] = cornerdetect(currentBitmap);
                trainingOutput[arrayCounter] = 0;
                trainingInput[arrayCounter] = running;
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
                running[2] = cornerdetect(currentBitmap);
                trainingOutput[arrayCounter] = 1;
                trainingInput[arrayCounter] = running;
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
                running[2] = cornerdetect(currentBitmap);
                trainingOutput[arrayCounter] = 2;
                trainingInput[arrayCounter] = running;
                arrayCounter++;
            }
        }


        //Bliver aldrig brugt
        public void load_background_image(System.Drawing.Image inputImage)
        {
            add_ignore_colors(inputImage);
        }


        //Bliver aldrig brugt
        public void load_image(System.Drawing.Image input)
        {
            whitePixels = 0;
            properties = new int[5];
            counter++;
        }


        //Bliver aldrig brugt
        public void load_files()
        {
            emptyImages = Directory.GetFiles("images/Empty/", "*.*");
            ballImages = Directory.GetFiles("images/Blue/", "*.*");
            errors = Directory.GetFiles("images/Error/", "*.*");
            background = Directory.GetFiles("images/Background/", "*.*");
        }


        //Bliver aldrig brugt 
        public void get_properties()
        {
            properties[0] = blobdetect(currentBitmap); //no of blobs detected
            properties[1] = whitePixels; //no of white pixels in image
            properties[2] = cornerdetect(currentBitmap); //no of corners detected
        }

        public int blobdetect(Bitmap input)
        {
            Accord.Imaging.BlobCounter blobs = new BlobCounter();
            blobs.FilterBlobs = true;
            blobs.BlobsFilter = null;
            blobs.MinHeight = 35;
            blobs.MinWidth = 35;

            blobs.ProcessImage(input);

            return (blobs.ObjectsCount);
        }

        public int cornerdetect(Bitmap input)
        {
            Accord.Imaging.SusanCornersDetector susanCorners = new SusanCornersDetector(1,10);
            List<Accord.IntPoint> points = susanCorners.ProcessImage(input);
            return points.Count;
        }
    }
}