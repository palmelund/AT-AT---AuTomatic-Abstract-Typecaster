using System;

namespace SerialIO
{
	public class CommandMessage : Message
	{
	    /// <summary>
	    /// The command that was sent by the Arduino to execute on the computer
	    /// </summary>
		public Command Command { get; }

	    /// <summary>
	    /// The command that was sent by the Arduino to execute on the computer
	    /// </summary>
		public CommandMessage (MessageType type, Command command)
			: base(type)
		{
			Command = command;
		}
	}
}

