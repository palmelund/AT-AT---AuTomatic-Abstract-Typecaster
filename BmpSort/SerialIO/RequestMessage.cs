using System;

namespace SerialIO
{
	public class RequestMessage : IMessage
	{
	    /// <summary>
	    /// The request sent from the Arduino for data
	    /// </summary>
		public ComputerRequest Request { get; set; }

	    /// <summary>
	    /// The request from the Arduino for data.
	    /// This is data that the Arduino wants at the point the message is sent.
	    /// </summary>
	    /// <param name="request">The info that the arduino requests</param>
		public RequestMessage (ComputerRequest request)
		{
			Request = request;
		}
	}
}

