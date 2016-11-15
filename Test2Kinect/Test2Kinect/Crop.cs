using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace Test2Kinect
{
    public static class Crop
    {
        public static WriteableBitmap CopyPixelsTo(BitmapSource sourceImage, Int32Rect sourceRoi,
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

        



    }
}
