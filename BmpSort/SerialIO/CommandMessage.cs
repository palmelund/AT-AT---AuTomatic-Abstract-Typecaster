using System;

namespace SerialIO
{
	public class CommandMessage : IMessage
	{
	    /// <summary>
	    /// The command that was sent by the Arduino to execute on the computer
	    /// </summary>
		public ComputerCommand Command { get; set; }

	    /// <summary>
	    /// The command that was sent by the Arduino to execute on the computer
	    /// </summary>
	    public CommandMessage (ComputerCommand command)
		{
			Command = command;
		}
	}
}

