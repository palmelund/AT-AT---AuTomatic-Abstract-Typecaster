using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace outputARGBcolorvalues
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
            writecolors();
        }
        public void writecolors()
        {
            textBox1.AppendText("Green: " + Color.Green.ToArgb() + "\n");
            textBox1.AppendText("Blue: " + Color.Blue.ToArgb() + "\n");
            textBox1.AppendText("Red: " + Color.Red.ToArgb() + "\n");
            textBox1.AppendText("Yellow: " + Color.Yellow.ToArgb() + "\n");
        }
    }
}
