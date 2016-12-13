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

        public Machine()
        {
            properties = new ImageProperties("backgrounds.txt");
            train_model();
        }

        public Bitmap Currentpicture { get; set; }

        public int[][] TrainerInput { get; set; }

        public int[] TrainerOutput { get; set; }

        public void load_model_from_file(string inputfile)
        {
            Accord.IO.Serializer.Load<Accord.MachineLearning.Bayes.NaiveBayes>(inputfile);           
        }

        public void save_model_from_file(string outputfile)
        {
            Accord.IO.Serializer.Save<Accord.MachineLearning.Bayes.NaiveBayes>(nb, Path.GetFullPath(Path.Combine(AppDomain.CurrentDomain.BaseDirectory, outputfile)));
        }

        public void train_model()
        {           
            nb = naiveBayes.Learn(properties.trainingInput, properties.trainingOutput);           
        }

        public int decide(Image image)
        {
            int result = nb.Decide(properties.get_properties(image));
            Currentpicture = properties.currentBitmap;
            return result;
        }
    }
}