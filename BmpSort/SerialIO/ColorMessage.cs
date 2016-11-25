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
		public ushort RedIntensity { get; set; }
	    public ushort GreenIntensity { get; set; }
	    public ushort BlueIntensity { get; set; }

	    /// <summary>
	    /// Creates a color message that can be sent to and received from the Arduino
	    /// </summary>
	    /// <param name="color">The color part of a RGB value</param>
	    /// <param name="redIntensity">TODO</param>
	    /// <param name="greenIntensity">TODO</param>
	    /// <param name="blueIntensity">TODO</param>
	    public ColorMessage (Color color, ushort redIntensity, ushort greenIntensity, ushort blueIntensity)
		{
			Color = color;
			RedIntensity = redIntensity;
		    GreenIntensity = greenIntensity;
		    BlueIntensity = blueIntensity;
		}
	}
}

