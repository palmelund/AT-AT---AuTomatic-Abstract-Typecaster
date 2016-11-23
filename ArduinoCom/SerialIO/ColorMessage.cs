using System;

namespace SerialIO
{
	public class ColorMessage : IMessage
	{
		public Color Color { get; set; }
		public ushort Intensity { get; set; }

		public ColorMessage (Color color, ushort intensity)
		{
			Color = color;
			Intensity = intensity;
		}
	}
}

