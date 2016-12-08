using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SerialIO
{
    /// <summary>
    /// Comands that the Arduino can send to the Computer
    /// Currently only to take pictures for image processing.
    /// </summary>
    public enum Command : byte
    {
        TakePicture = 0x00,
		Start = 0x01,
		Stop = 0x02
    }
}
