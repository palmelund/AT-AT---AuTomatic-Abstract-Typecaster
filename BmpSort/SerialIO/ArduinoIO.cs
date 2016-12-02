using System;
using System.IO.Ports;
using System.Linq;

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

    /// <summary>
    /// Comands that the Arduino can send to the Computer
    /// Currently only to take pictures for image processing.
    /// </summary>
	public enum ComputerCommand : byte
	{
		TakePicture = 0,
	}

    /// <summary>
    /// Requests for the computer to send data to the Arduino
    /// Currently only objectinfo
    /// </summary>
	public enum ComputerRequest : byte
	{
		ObjectInfo = 0
	}

    /// <summary>
    /// Is the shape we have seen a ball or not?
    /// </summary>
	public enum Shape : byte
	{
		Ball = 0,
		NotBall = 1
	}

    /// <summary>
    /// One of the three RGB values.
    /// </summary>
	public enum Color : byte
	{
		Green  = 0,
		Yellow = 1,
		Red    = 2,
		Blue   = 3,
	}

    /// <summary>
    /// The ArduinoIO calss contains methods for sending and recieving data to and from the Arduino.
    /// </summary>
    // ReSharper disable once InconsistentNaming
    public class ArduinoIO
	{
	    // 0x62 is defined as the <start-of-message> byte, and must be present at the beginning of all sent messages
	    private const byte Begin = 0x62;

		const byte IN_COMMAND  = 0x00;
		const byte IN_REQUEST  = 0x01;
		const byte IN_COLOR    = 0x02;
		const byte IN_DISTANCE = 0x03;

		const byte OUT_COMMAND  = 0x00;
		const byte OUT_OBJECT   = 0x01;
		const byte OUT_COLOR    = 0x02;
		const byte OUT_DISTNACE = 0x03;

	    private readonly SerialPort _port;

	    /// <summary>
	    /// Wrapper for serial communcation with the Arduino. Contains methods for easy formatting of all messages allowed to be sent to the Arduino.
	    /// </summary>
	    /// <param name="portName">The port the Arduino is connected to by USB. This variable can change depending on OS and number of connected devices.</param>
		public ArduinoIO(string portName)
		{
			_port = new SerialPort (portName, 9600);	//"/dev/ttyACM0"

			if (_port.IsOpen)
				_port.Close ();

			_port.Open ();
		}

	    /// <summary>
	    /// Sends a command to the arduino.
	    /// </summary>
	    /// <param name="command">The Command that should be sent to the Arduino.</param>
		public void SendCommand(ArduinoCommand command)
		{
		    // Begin is constant start symbol
		    // 0x02 is the number of bytes of information sent to the Arduino, currently always 2 for a command
		    // 0x00 is the Arduino type for the command message
		    // command is the byte representation for the command argument
			var b = new byte[] { Begin, 0x02, OUT_COMMAND, (byte)command };
			_port.Write (b, 0, b.Length);
		}

	    /// <summary>
	    /// Sends an object shape to the Arduino.
	    /// This is used to send the result from object recognition.
	    /// </summary>
	    /// <param name="shape">The shape of the object</param>
		public void SendObject(Shape shape)
		{
		    // Begin is the constant start symbol
		    // 0x02 is the number of bytes of information sent to the Arduino, currently always 2 for an object
		    // 0x01 is the Arduino type for the object message
		    // shape is the byte representation for the shape argument
			var b = new byte[] { Begin, 0x02, OUT_OBJECT, (byte)shape };
			_port.Write (b, 0, b.Length);
		}

	    /// <summary>
	    /// Sends a color to the Arduino
	    /// Used to set a predefined color as the default color.
	    /// This is part of a RGB color, and has to be sent three times to send the entire color
	    /// </summary>
	    /// <param name="color">One of the three RGB colors</param>
	    /// <param name="redIntensity">The red intensity of the color</param>
	    /// <param name="greenIntensity">The green intensity of the color</param>
	    /// <param name="blueIntensity">The blue intensity of the color</param>
	    public void SendColor(Color color, ushort redIntensity, ushort greenIntensity, ushort blueIntensity)
		{
		    // Begin is the constant start symbol
		    // 0x04 is the number of bytes of information sent to the Arduino, currently always 4 for a color
		    // 0x02 is the Arduino type for the color message
		    // color is the color that has to be set in the RGB value.
		    // intensity is the intensity of the color, and is split into two bytes to be transmitted correctly.
		    var b = new byte[] { Begin, 0x08, OUT_COLOR, (byte)color, (byte)(redIntensity >> 8), (byte)redIntensity, (byte)(greenIntensity >> 8), (byte)greenIntensity, (byte)(blueIntensity >> 8), (byte)blueIntensity };    // TODO: Make human readable
			_port.Write (b, 0, b.Length);
		    Console.Write("Sent message: ");
		    foreach (var i in b)
		    {
                Console.Write(i +" ");
		    }

		    Console.WriteLine();
		}

	    /// <summary>
	    /// Sends a distance to the Arduino
	    /// This distance will override the default value measured on startup
	    /// </summary>
	    /// <param name="distance">Distance in mm</param>    // TODO: Ensure correct unit of measurment
		public void SendDistance(ushort distance)
		{
		    // Begin is the constant start symbol
		    // 0x03 is the number of bytes of information sent to the Arduino, currently always 3 for a distance
		    // 0x04 is the Arduino type for the distance message
		    // distance is the distance in mm, and is split into two bytes to be transmitted correctly.
		    var b = new byte[] { Begin, 0x03, OUT_DISTNACE, (byte)(distance >> 8), (byte)distance };
			_port.Write (b, 0, b.Length);
		}

	    /// <summary>
	    /// Recieves a message from the Arduino. The type of message returned depends on what the Arduino sends.
	    /// </summary>
	    /// <returns>A message from the arduino. Has to be read type checked later to get message</returns>
	    /// <exception cref="ArgumentException">If the arduino sends a message that doesn't follow the defined values or arguments, an exception is thrown</exception>
		public bool AwaitMessage(out IMessage message)
		{
		    message = null;

		    //Console.WriteLine("Awaiting");

		    // Wait until we have a new message and its length in the buffer.
			while (_port.BytesToRead < 2)
			{
			    // Console.WriteLine(_port.BytesToRead);
			}

		    //Console.WriteLine("Two bytes found");

		    var b = new byte[2];
			_port.Read (b, 0, 2);

			//Console.WriteLine ("Startbytes: " + b [0] + " " + b [1]);

		    if (b[0] != Begin)
		    {
				Console.WriteLine ("Byte not start byte");
		        return false;
		    }

		    // Create a new byte array with the length of the message sent
			var data = new byte[b [1]];

			//Console.WriteLine ("Length: " + data.Length);

		    // Wait for the entire message to be in the buffer.
			while (_port.BytesToRead < data.Length)
			{
			}

		    _port.Read (data, 0, data.Length);

		    // Return the data as a message of the correct type.

		    //Console.WriteLine("Data[0]:" + data[0] + "\nLength: " + data.Length);

			switch (data [0]) {
			case IN_COMMAND:
				message = new CommandMessage ((ComputerCommand)data [1]);
			    return true;
			case IN_REQUEST:
				message = new RequestMessage ((ComputerRequest)data [1]);
			    return true;
			case IN_COLOR:
				//Console.Write ("Bytes recieved: ");
				//foreach (var i in data) {
				//	Console.Write (i + " ");
				//}
				//Console.WriteLine ();

				message = new ColorMessage ((Color)data [1], BitConverter.ToUInt16(data, 2), BitConverter.ToUInt16(data, 4), BitConverter.ToUInt16(data,6));
				    return true;
			    case IN_DISTANCE:
				message = new DistanceMessage (BitConverter.ToUInt16(data, 1));
			    return true;
			    default:
			    return false;
			}
		}

	    public void SendByte(byte[] b)
	    {
	        _port.Write(b, 0, b.Length);
	    }
	}
}