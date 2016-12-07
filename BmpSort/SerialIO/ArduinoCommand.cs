using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SerialIO
{
    /// <summary>
    /// Commands that can be send to the Arduino.
    /// </summary>
    public enum ArduinoCommand : byte
    {
        CalibrateRed = 0,
        CalibrateGreen = 1,
        CalibrateBlue = 2,
        CalibrateYellow = 3,
        CalibrateDistance = 4,
        Start = 5,
        Stop = 6
    }
}
