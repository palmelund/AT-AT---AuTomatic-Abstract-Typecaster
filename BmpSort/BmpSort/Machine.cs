using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Drawing;
using System.IO;
using Accord;
using Accord.MachineLearning;


namespace BmpSort
{
    public class Machine
    {
        ImageProperties properties;

        Accord.MachineLearning.Bayes.NaiveBayesLearning naiveBayes =
            new Accord.MachineLearning.Bayes.NaiveBayesLearning();

        Accord.MachineLearning.Bayes.NaiveBayes nb;

        public Machine(string fileName)
        {
            properties = new ImageProperties("backgrounds.txt");
            train_model();
        }

        private int[][] _input; //image variables

        public int[][] trainerInput
        {
            get { return _input; }
            set { _input = value; }
        }

        private int[] _output; //class labels

        public int[] trainerOutput
        {
            get { return _output; }
            set { _output = value; }
        }

        public void load_model_from_file(string inputfile)
        {
            Accord.IO.Serializer.Load<Accord.MachineLearning.Bayes.NaiveBayes>(inputfile);           
        }

        public void save_model_from_file(string outputfile)
        {
            Accord.IO.Serializer.Save<Accord.MachineLearning.Bayes.NaiveBayes>(nb, System.IO.Path.GetFullPath(System.IO.Path.Combine(System.AppDomain.CurrentDomain.BaseDirectory, outputfile)));
        }

        public void train_model()
        {           
            nb = naiveBayes.Learn(properties.trainingInput, properties.trainingOutput);           
        }

        public int decide(System.Drawing.Image image)
        {
            return (nb.Decide(properties.get_properties(image)));
        }
    }
}