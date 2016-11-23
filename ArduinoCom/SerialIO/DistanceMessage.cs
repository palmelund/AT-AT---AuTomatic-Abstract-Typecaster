using System;

namespace SerialIO
{
	public class DistanceMessage : IMessage
	{
		public ushort Distance { get; set;}

		public DistanceMessage (ushort distance)
		{
			Distance = distance;
		}
	}
}

