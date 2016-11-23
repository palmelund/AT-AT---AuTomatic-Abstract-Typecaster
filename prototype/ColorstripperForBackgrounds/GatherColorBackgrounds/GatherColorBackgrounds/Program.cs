using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Threading.Tasks;
using System.Drawing;
using Accord.Imaging;

namespace GatherColorBackgrounds
{
    class Program
    {
        
        static void Main(string[] args)
        {
            string[] backFiles = Directory.GetFiles("images/Background/", "*.*");
            Accord.Imaging.Converters.ImageToArray converter = new Accord.Imaging.Converters.ImageToArray();
            Dictionary<int, int> colors= new Dictionary<int, int>();
            colors.Add(0,0);
            
            foreach (string billede in backFiles)
            {
                System.Drawing.Image image = System.Drawing.Image.FromFile(billede);
                Bitmap bitmap = new Bitmap(image);
                Color[] colorarray = new Color[80000];
                
                converter.Convert(bitmap, out colorarray);

                foreach (Color farve in colorarray)
                {
                    if (!colors.ContainsKey(farve.ToArgb()))
                    {
                        colors.Add(farve.ToArgb(), farve.ToArgb());
                    }
                }
                
            }
            StreamWriter sw = new StreamWriter("output.txt");
            foreach (KeyValuePair<int,int> values in colors)
            {
                sw.WriteLine(values.Key);
            }
            sw.Close();
        }
    }
}
