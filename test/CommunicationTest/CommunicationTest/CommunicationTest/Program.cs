using System;
using SerialIO;

namespace CommunicationTest
{
	class MainClass
	{
		public static void Main (string[] args)
		{
			var io = new ArduinoIO ("/dev/ttyACM0");
			while (true) {
				
				Console.ReadKey ();

				io.SendCommand (ArduinoCommand.CalibrateRed);

				// Console.WriteLine ("Red color: ");
				IMessage message;
				for (int i = 0; i < 101; i++) {
					if (!io.AwaitMessage (out message))
						return;
					var c = message as ColorMessage;
					if (c == null)
						return;
					Console.WriteLine ( i + " - R: " + c.RedIntensity + " G: " + c.GreenIntensity + " B: " + c.BlueIntensity);
				}
				Console.WriteLine ("Enter to continue!");
			}
		    /*
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
*/
		}
	}
}
