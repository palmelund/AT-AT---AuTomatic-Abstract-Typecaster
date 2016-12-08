using System;

namespace SerialIO
{
	public class ObjectMessage : Message
	{
		public Shape Shape { get; }
		public Color Color { get; }

		public ObjectMessage (MessageType type, Shape shape, Color color)
			: base(type)
		{
			Shape = shape;
			Color = color;
		}
	}
}

