using System;

namespace SerialIO
{
	public class RequestMessage : IMessage
	{
		public ComputerRequest Request { get; set; }

		public RequestMessage (ComputerRequest request)
		{
			Request = request;
		}
	}
}

