﻿using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Imaging;

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
            int stride = croppedBitmap.PixelWidth*(croppedBitmap.Format.BitsPerPixel/8);
            var data = new byte[stride*croppedBitmap.PixelHeight];
            croppedBitmap.CopyPixels(data, stride, 0);
            tmp.WritePixels(destinationRoi, data, stride, 0);
            return tmp;
            //return tmp.Gray(); //Use this to return grayscale
        }

        /// <summary>
        /// Converting WriteableBitmap to Bitmap
        /// </summary>
        /// <param name="input"></param>
        /// <returns></returns>
        public Bitmap ToBitmap(WriteableBitmap input)
        {
            System.Drawing.Bitmap bmp;
            using (MemoryStream outStream = new MemoryStream())
            {
                BitmapEncoder enc = new BmpBitmapEncoder();
                enc.Frames.Add(BitmapFrame.Create((BitmapSource)input));
                enc.Save(outStream);
                bmp = new System.Drawing.Bitmap(outStream);
            }
            return bmp;
        }
    }
}