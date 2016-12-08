using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SerialIO
{
    /// <summary>
    /// One of the three RGB values.
    /// </summary>
    public enum Color : byte
    {
        Green = 0x00,
		Yellow = 0x01,
		Red = 0x02,
		Blue = 0x03,
		Unknown = 0x04
    }
}
