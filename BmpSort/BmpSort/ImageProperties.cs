using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Accord.Imaging;
using System.Drawing;
using System.IO;
using System.Windows;

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
        public int[] trainingOutput { get; set; }
        public int[][] trainingInput { get; set; }
        public int arrayCounter { get; set; }

        string[] emptyImages;
        string[] background;
        string[] ballImages;
        string[] errors;

        Bitmap currentBitmap;
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
  
        public int[] get_properties(System.Drawing.Image inputImage)
        {
            int[] result = {0, 0, 0};
            inputImage = clean_background(inputImage);
            result[0] = whitePixels;
            result[1] = blobs;
            result[2] = corners;

            return result;
        }//gets image properties for new images
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
            corners = 1; // cornerdetect(currentBitmap);
            return currentBitmap;
        }

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

        public void load_files() //gets training image filenames from folders
        {
            emptyImages = Directory.GetFiles("Images/Empty/", "*.*");
            ballImages = Directory.GetFiles("Images/Blue/", "*.*");
            errors = Directory.GetFiles("Images/Error/", "*.*");
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
                //outputList.Add(0);
                //trainingOutput[arrayCounter] = 0;
                //inputList.Add(running);
                //trainingInput[arrayCounter] = running;
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
                //outputList.Add(1);
                //trainingOutput[arrayCounter] = 1;
                //inputList.Add(running);
                //trainingInput[arrayCounter] = running;
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
                //outputList.Add(2);
                //trainingOutput[arrayCounter] = 2;
                //inputList.Add(running);
                //trainingInput[arrayCounter] = running;
                arrayCounter++;
            }

        }      


        //Bliver aldrig brugt 
        public void get_properties()
        {
            properties[0] = blobdetect(currentBitmap); //no of blobs detected
            properties[1] = whitePixels; //no of white pixels in image
            properties[2] = 0; //cornerdetect(currentBitmap); //no of corners detected
        }


        #region detectors
        public int blobdetect(Bitmap input)
        {
            Accord.Imaging.BlobCounter blobs = new BlobCounter();
            blobs.FilterBlobs = true;
            blobs.BlobsFilter = null;
            blobs.MinHeight = 35;
            blobs.MinWidth = 35;

            blobs.ProcessImage(input);
            if (blobs.ObjectsCount == 1)
            {
                return 1;
            }
            else
            {
                return 0;
            }          
        }
        public int cornerdetect(Bitmap input)
        {
            return 1;
        } 
        #endregion detectors
    }
}