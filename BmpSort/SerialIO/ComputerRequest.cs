using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SerialIO
{
    /// <summary>
    /// Requests for the computer to send data to the Arduino
    /// Currently only objectinfo
    /// </summary>
    public enum ComputerRequest : byte
    {
        ObjectInfo = 0
    }
}
