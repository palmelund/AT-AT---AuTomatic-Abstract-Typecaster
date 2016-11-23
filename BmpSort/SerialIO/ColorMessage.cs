using System;

namespace SerialIO
{
	public class ColorMessage : IMessage
	{
	    /// <summary>
	    /// The color part of a RGB value
	    /// </summary>
		public Color Color { get; set; }

	    /// <summary>
	    /// The intensity of the color
	    /// </summary>
		public ushort Intensity { get; set; }

	    /// <summary>
	    /// Creates a color message that can be sent to and received from the Arduino
	    /// </summary>
	    /// <param name="color">The color part of a RGB value</param>
	    /// <param name="intensity">The intensity of the color</param>
		public ColorMessage (Color color, ushort intensity)
		{
			Color = color;
			Intensity = intensity;
		}
	}
}

