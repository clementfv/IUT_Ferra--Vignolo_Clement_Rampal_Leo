using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace RobotInterface
{
    internal class Robot
    {
     
        public string receivedText = "";
        public float distanceTelemetreDroit;
        public double positionXOdo;
        public double positionYOdo;
        public float angleRadianFromOdometry;
        public float distanceTelemetreCentre;
        public float vitesseLineaireFromOdometry;
        public float vitesseAngulaireFromOdometry;
        public float distanceTelemetreGauche;
        public Queue<byte> byteListReceived;
        public Robot()
        {
     byteListReceived = new Queue<byte>();
        }
    }
}
