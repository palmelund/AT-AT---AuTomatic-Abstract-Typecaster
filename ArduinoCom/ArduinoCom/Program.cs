using System;
using SerialIO;

namespace ArduinoCom
{
    internal class MainClass
	{
	    private static void Main()
		{
			var io = new ArduinoIO ("/dev/ttyACM0");
		    io.SendColor(Color.Red, 227);
		    io.AwaitMessage();
		}
	}
}
