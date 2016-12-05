using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Controls;

namespace BmpSort
{
    class ARFFGenerator
    {
        List<string> stringlist = new List<string>();
        private Machine M;

        string path =
            System.IO.Path.GetFullPath(System.IO.Path.Combine(System.AppDomain.CurrentDomain.BaseDirectory, "..\\..\\",
                "Images"));


        public ARFFGenerator(Machine m)
        {
            M = m;

        }


        public void generate_arff_file(ref int progress)
        {
           
            for (int i = 0; i < M.trainerInput.Length; i++)
            {
                read_file_data_values(M.trainerInput[i][0], M.trainerInput[i][1], M.trainerInput[i][2],
                    M.trainerOutput[i]);
                report_progress(i, M.trainerInput.Length, ref progress);
            }
            write_arff_file();
        }


        public void read_file_data_values(int a, int b, int c, int classification)
        {
            string label;
            switch (classification)
            {
                case 0:
                {
                    label = "ball";
                    break;
                }
                case 1:
                {
                    label = "empty";
                    break;
                }
                case 2:
                {
                    label = "error";
                    break;
                }
                default:
                {
                    label = "ignore";
                    break;
                }
            }
            string line = (a.ToString() + "," + b.ToString() + "," + c.ToString() + "," + label);
            stringlist.Add(line);
            //double[] result = new double[] { a, b, c, d, e };
            //setupNetwork.add_training_data(result);
        }


        public void write_arff_file()
        {
            StreamWriter writer = new StreamWriter("output.arff");
            writer.WriteLine("@RELATION ball");
            writer.WriteLine(" ");
            writer.WriteLine("@ATTRIBUTE whitepixels NUMERIC");
            writer.WriteLine("@ATTRIBUTE blobcount NUMERIC");
            writer.WriteLine("@ATTRIBUTE cornercount NUMERIC");
            writer.WriteLine("@ATTRIBUTE class {ball,empty,error,ignore}");
            writer.WriteLine(" ");
            writer.WriteLine("@DATA");
            foreach (string line in stringlist)
            {
                writer.WriteLine(line);
            }


            writer.Close();
        }

        public void report_progress(int current, int max, ref int progress)
        {
            progress = (current/max)*100;
        }
    }
}