import lejos.nxt.Motor;
import lejos.util.Delay;
import lejos.nxt.ColorSensor;
import lejos.nxt.SensorPort;
import lejos.nxt.SensorConstants;
import lejos.nxt.LCD;

public class Prototype2 {
	public static void main(String[] args) {

		ColorSensor sensor = new ColorSensor(SensorPort.S1);
		/*
		while(true) {
			ColorSensor.Color c = sensor.getColor();
			System.out.println("R:" + c.getRed() + " G:" + c.getGreen() + " B:" + c.getBlue());			
		} /**/
		/**/

		Motor.A.setSpeed(120);
		Motor.A.backward();

		Motor.B.setSpeed(120);
		Motor.B.forward();

		Feeder fthread = new Feeder();
		fthread.setDaemon(true);
		fthread.start();

		long current = System.currentTimeMillis();

		while(true) {
			ColorSensor.Color c = sensor.getColor();
			String r, g, b;

			if (c.getRed() > 75) {
				current = System.currentTimeMillis();
				ScheduleRed sr = new ScheduleRed();
				sr.setDaemon(true);
				sr.run();
			} else if (c.getGreen() > 75) {
				current = System.currentTimeMillis();
				ScheduleGreen sg = new ScheduleGreen();
				sg.setDaemon(true);
				sg.run();
			} else if (c.getBlue() > 75) {
				current = System.currentTimeMillis();
				ScheduleBlue sb = new ScheduleBlue();
				sb.setDaemon(true);
				sb.run();
			}
		}
		/**/
	}
}

class ScheduleRed extends Thread {
	public void run() {
		// Delay.msDelay(1000);
		// Do nothing, red is forward
	}
}

class ScheduleGreen extends Thread {
	public void run() {
		Delay.msDelay(3000);
		Motor.C.rotate(90, true);
		Delay.msDelay(2000);
		Motor.C.rotate(-90, true);
	}
}

class ScheduleBlue extends Thread {
	public void run() {
		Delay.msDelay(3000);
		Motor.C.rotate(-90, true);
		Delay.msDelay(2000);
		Motor.C.rotate(90, true);
	}
}

class Feeder extends Thread {
	public void run() {
		while(true) {
//			Motor.B.rotate(90, true);
//			Delay.msDelay(1500);
		}
	}
}