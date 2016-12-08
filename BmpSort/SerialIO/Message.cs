using System;

namespace SerialIO
{
    /// <summary>
    /// An class used as a parent for all messages that can be received from and sent to the Arduino.
    /// </summary>
	public class Message
	{
		public MessageType Type { get; }

		public Message (MessageType type)
		{
			Type = type;
		}
	}
}

