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


		const byte IN_COMMAND  = 0x00;
		const byte IN_REQUEST  = 0x01;
		const byte IN_COLOR    = 0x02;
		const byte IN_DISTANCE = 0x03;

		const byte OUT_COMMAND  = 0x00;
		const byte OUT_OBJECT   = 0x01;
		const byte OUT_COLOR    = 0x02;
		const byte OUT_DISTNACE = 0x03;

		public SerialPort Port { get; }

		public ArduinoIO(string portName)
		{
			Port = new SerialPort (portName, 9600);	//"/dev/ttyACM0"

			if (Port.IsOpen)
				Port.Close ();
			
			Port.Open ();
		}

		public void SendCommand(ArduinoCommand command)
		{
			byte[] b = new byte[] { BEGIN, 0x02, OUT_COMMAND, (byte)command };
			Port.Write (b, 0, b.Length);
		}

		public void SendObject(Shape shape)
		{
			byte[] b = new byte[] { BEGIN, 0x02, OUT_OBJECT, (byte)shape };
			Port.Write (b, 0, b.Length);
		}

		public void SendColor(Color color, ushort intensity)
		{
			byte[] b = new byte[] { BEGIN, 0x04, OUT_COLOR, (byte)color, (byte)(intensity >> 8), (byte)intensity };
			Port.Write (b, 0, b.Length);
		}

		public void SendDistance(ushort distance)
		{
			byte[] b = new byte[] { BEGIN, 0x03, OUT_DISTNACE, (byte)(distance >> 8), (byte)distance };
			Port.Write (b, 0, b.Length);
		}

		public IMessage AwaitMessage()
		{
			while (Port.BytesToRead < 2) ;

			byte[] b = new byte[2];
			Port.Read (b, 0, 2);

			if (b[0] != BEGIN)
				throw new ArgumentException("Invalid start of message.");

			byte[] data = new byte[b [1]];

			while (Port.BytesToRead < data.Length) ;

			Port.Read (data, 0, data.Length);

			switch (data [0]) {
			case IN_COMMAND:
				return new CommandMessage ((ComputerCommand)data [1]);
			case IN_REQUEST:
				return new RequestMessage ((ComputerRequest)data [1]);
			case IN_COLOR:
				return new ColorMessage ((Color)data [1], BitConverter.ToUInt16(data, 2));
			case IN_DISTANCE:
				return new DistanceMessage (BitConverter.ToUInt16(data, 1));
			default:
				throw new ArgumentException ("Unknown command");
			}
		}
	}
}

