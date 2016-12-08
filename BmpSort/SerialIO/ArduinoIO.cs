using System;
using System.IO.Ports;
using System.Linq;
using System.Text;

namespace SerialIO
{
    /// <summary>
    /// The ArduinoIO calss contains methods for sending and recieving data to and from the Arduino.
    /// </summary>
    // ReSharper disable once InconsistentNaming
    public class ArduinoIO
	{
	    // 0x62 is defined as the <start-of-message> byte, and must be present at the beginning of all sent messages
	    private const byte Begin = 0x62;

		const byte MESSAGE_TYPE_COMMAND = 0x00;
		const byte MESSAGE_TYPE_OBJECT = 0x01;

		const byte MESSAGE_SIZE_COMMAND = 0x02;
		const byte MESSAGE_SIZE_OBJECT = 0x03;
	

        public event SerialDataReceivedEventHandler DataRecived {
            add { _port.DataReceived += value; }
            remove { _port.DataReceived -= value; }
        }

	    private readonly SerialPort _port;

	    /// <summary>
	    /// Wrapper for serial communcation with the Arduino. Contains methods for easy formatting of all messages allowed to be sent to the Arduino.
	    /// </summary>
	    /// <param name="portName">The port the Arduino is connected to by USB. This variable can change depending on OS and number of connected devices.</param>
		public ArduinoIO(string portName)
		{
		    _port = new SerialPort(portName, 9600) {Encoding = Encoding.ASCII}; //"/dev/ttyACM0"

		    if (_port.IsOpen)
				_port.Close ();

			_port.Open ();
		}

	    public string ReadExistingBytes() => _port.ReadExisting();

	    /// <summary>
	    /// Sends a command to the arduino.
	    /// </summary>
	    /// <param name="command">The Command that should be sent to the Arduino.</param>
		public void SendCommand(Command command)
		{
		    // Begin is constant start symbol
		    // 0x02 is the number of bytes of information sent to the Arduino, currently always 2 for a command
		    // 0x00 is the Arduino type for the command message
		    // command is the byte representation for the command argument
			var b = new byte[] { Begin, MESSAGE_SIZE_COMMAND, MESSAGE_TYPE_COMMAND, (byte)command };
			_port.Write (b, 0, b.Length);
		}

	    /// <summary>
	    /// Sends an object shape to the Arduino.
	    /// This is used to send the result from object recognition.
	    /// </summary>
	    /// <param name="shape">The shape of the object</param>
		public void SendObject(Shape shape, Color color = Color.Unknown)
		{
		    // Begin is the constant start symbol
		    // 0x02 is the number of bytes of information sent to the Arduino, currently always 2 for an object
		    // 0x01 is the Arduino type for the object message
		    // shape is the byte representation for the shape argument
			var b = new byte[] { Begin, MESSAGE_SIZE_OBJECT, MESSAGE_TYPE_OBJECT, (byte)shape, (byte)color };
			_port.Write (b, 0, b.Length);
		}

	    /// <summary>
	    /// Recieves a message from the Arduino. The type of message returned depends on what the Arduino sends.
	    /// </summary>
	    /// <returns>A message from the arduino. Has to be read type checked later to get message</returns>
	    /// <exception cref="ArgumentException">If the arduino sends a message that doesn't follow the defined values or arguments, an exception is thrown</exception>
		public bool AwaitMessage(out Message message)
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
			case MESSAGE_TYPE_COMMAND:
				message = new CommandMessage ((Command)data [1]);
			    return true;
			    default:
			    return false;
			}
		}

        /// <summary>
        /// Sends an array of bytes to the Arduino
        /// This does NOT include a command start byte, and thus should be treated with care in a runnin system.
        /// </summary>
        /// <param name="b">The EXACT senquence of bytes to send</param>
	    public void SendByte(byte[] b)
	    {
	        _port.Write(b, 0, b.Length);
	    }

	    public void CloseConnection()
	    {
	        _port.Close();
	    }

	    public int ReadBuffer()
	    {
	        return _port.ReadByte();
	    }
	}
}