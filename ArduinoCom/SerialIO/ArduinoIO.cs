using System;
using System.IO.Ports;

namespace SerialIO
{
	public enum ArduinoCommand : byte
	{
		CalibrateRed = 0,
		CalibrateGreen = 1,
		CalibrateBlue = 2,
		CalibrateDistance = 3,
		Start = 4,
		Stop = 5
	}

	public enum ComputerCommand : byte
	{
		TakePicture = 0,
	}

	public enum ComputerRequest : byte
	{
		ObjectInfo = 0
	}

	public enum Shape : byte
	{
		Ball = 0,
		NotBall = 1
	}

	public enum Color : byte
	{
		Red = 0,
		Green = 1,
		Blue = 2
	}

	public class ArduinoIO
	{
		const byte BEGIN = 0x62;

		SerialPort port;

		public ArduinoIO(string portName)
		{
			port = new SerialPort (portName, 9600);	//"/dev/ttyACM0"

			if (port.IsOpen)
				port.Close ();
			port.Open ();
		}

		public void SendCommand(ArduinoCommand command)
		{
			byte[] b = new byte[] { BEGIN, 0x02, 0x00, (byte)command };
			port.Write (b, 0, b.Length);
		}

		public void SendObject(Shape shape)
		{
			byte[] b = new byte[] { BEGIN, 0x02, 0x01, (byte)shape };
			port.Write (b, 0, b.Length);
		}

		public void SendColor(Color color, ushort intensity)
		{
			byte[] b = new byte[] { BEGIN, 0x04, 0x02, (byte)color, (byte)(intensity >> 8), (byte)intensity };
			port.Write (b, 0, b.Length);
		}

		public void SendDistance(ushort distance)
		{
			byte[] b = new byte[] { BEGIN, 0x03, 0x03, (byte)(distance >> 8), (byte)distance };
			port.Write (b, 0, b.Length);
		}

		public IMessage AwaitMessage()
		{
			while (port.BytesToRead < 2)
				;

			byte[] b = new byte[2];
			port.Read (b, 0, 2);

			if (b[0] != BEGIN)
				throw new ArgumentException("Invalid start of message.");

			byte[] data = new byte[b [1]];

			while (port.BytesToRead < data.Length)
				;

			port.Read (data, 0, data.Length);

			switch (data [0]) {
			case 0:
				return new CommandMessage ((ComputerCommand)data [1]);
			case 1:
				return new RequestMessage ((ComputerRequest)data [1]);
			case 2:
				return new ColorMessage ((Color)data [1], BitConverter.ToUInt16(data, 2));
			case 3:
				return new DistanceMessage (BitConverter.ToUInt16(data, 1));
			default:
				throw new ArgumentException ("Unknown command");
			}
		}
	}
}

