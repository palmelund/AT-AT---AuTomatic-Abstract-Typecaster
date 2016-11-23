using System;

namespace SerialIO
{
	public class DistanceMessage : IMessage
	{
	    /// <summary>
	    /// The distance between the distance sensor and the wall
	    /// </summary>
		public ushort Distance { get; set;}

	    /// <summary>
	    /// This message contains info about the distance between the distance sensor and the wall.
	    /// </summary>
	    /// <param name="distance">The distance</param>
		public DistanceMessage (ushort distance)
		{
			Distance = distance;
		}
	}
}

