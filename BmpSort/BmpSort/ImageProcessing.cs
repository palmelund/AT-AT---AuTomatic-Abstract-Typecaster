using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using PixelFormat = System.Windows.Media.PixelFormat;

namespace BmpSort
{
    class ImageProcessing
    {
        public WriteableBitmap CopyPixelsTo(BitmapSource sourceImage, Int32Rect sourceRoi,
            Int32Rect destinationRoi)
        {
            /*Taken from http://stackoverflow.com/questions/17750310/how-can-you-copy-part-of-a-writeablebitmap-to-another-writeablebitmap
            and rerwitten to fit our needs. 28/10/2016 - 14:22 */


            WriteableBitmap tmp = new WriteableBitmap(400, 200, 96.0, 96.0, PixelFormats.Bgr32, null);
            var croppedBitmap = new CroppedBitmap(sourceImage, sourceRoi);
            int stride = croppedBitmap.PixelWidth * (croppedBitmap.Format.BitsPerPixel / 8);
            var data = new byte[stride * croppedBitmap.PixelHeight];
            croppedBitmap.CopyPixels(data, stride, 0);
            tmp.WritePixels(destinationRoi, data, stride, 0);
            return tmp;
            //return tmp.Gray(); //Use this to return grayscale
        }

        /// <summary>
        /// Converting BitmapSource to Bitmap
        /// </summary>
        /// <param name="input"></param>
        /// <returns></returns>
        public Bitmap ToBitmap(BitmapSource input)
        {
            Bitmap bmp;
            using (var outStream = new MemoryStream())
            {
                var enc = new BmpBitmapEncoder();
                enc.Frames.Add(BitmapFrame.Create(input));
                enc.Save(outStream);
                bmp = new Bitmap(outStream);
            }
            return bmp;
        }
    }
}