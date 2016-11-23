using System;
using SerialIO;

namespace CommunicationTest
{
	class MainClass
	{
		public static void Main (string[] args)
		{
			var io = new ArduinoIO ("/dev/ttyACM0");

			Console.ReadKey ();
			io.SendColor (Color.Red, 255);
			var colorMessage = io.AwaitMessage () as ColorMessage;
			Console.WriteLine (colorMessage.Color);
			Console.WriteLine (colorMessage.Intensity);

			io.SendCommand (ArduinoCommand.Start);
			var commandMessage = io.AwaitMessage () as CommandMessage;
			Console.WriteLine (commandMessage.Command);

			io.SendDistance (500);
			var distanceMessage = io.AwaitMessage () as DistanceMessage;
			Console.WriteLine (distanceMessage.Distance);

			io.SendObject (Shape.Ball);
			var requestMessage = io.AwaitMessage () as RequestMessage;
			Console.WriteLine (requestMessage.Request);

			Console.ReadKey ();
		}
	}
}
