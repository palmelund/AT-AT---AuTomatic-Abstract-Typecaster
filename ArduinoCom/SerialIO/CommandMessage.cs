using System;

namespace SerialIO
{
	public class CommandMessage : IMessage
	{
		public ComputerCommand Command { get; set; }
		public CommandMessage (ComputerCommand command)
		{
			Command = command;
		}
	}
}

