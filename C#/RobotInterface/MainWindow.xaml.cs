using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using ExtendedSerialPort_NS;
using System.IO.Ports;
using System.Windows.Threading;

namespace RobotInterface
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    /// 

    public enum StateReception
    {
        Waiting,         
        FunctionMSB,     
        FunctionLSB,      
        PayloadLengthMSB, 
        PayloadLengthLSB, 
        Payload,          
        CheckSum          
    }
    public partial class MainWindow : Window
    {
        ExtendedSerialPort serialPort1;
        private DispatcherTimer timerAffichage;
        Robot robot = new Robot();
        public MainWindow()
        {
            InitializeComponent();
            serialPort1 = new ExtendedSerialPort("COM4", 115200, Parity.None, 8, StopBits.One);
            serialPort1.DataReceived += SerialPort1_DataReceived;
            serialPort1.Open();
            timerAffichage = new DispatcherTimer();
            timerAffichage.Interval = new TimeSpan(0, 0, 0, 0, 100);
            timerAffichage.Tick += TimerAffichage_Tick;
            timerAffichage.Start();
        }


        int Click, Click2 = 0;
        private void SendMessage()
        {
            string message = textBoxEmission.Text;
            textBoxReception.Text += "\n Reçu : " + message;
            textBoxEmission.Text = "";
            serialPort1.WriteLine(message);
        }

        private void ButtonEnvoyer_Click(object sender, RoutedEventArgs e)
        {
            if (Click == 0)
            {
                buttonEnvoyer.Background = Brushes.RoyalBlue;
                Click = 1;
            }
            else if (Click == 1)
            {
                buttonEnvoyer.Background = Brushes.Beige;
                Click = 0;
            }
            SendMessage();
        }

        private void ButtonClear_Click(object sender, RoutedEventArgs e)
        {
            if (Click2 == 0)
            {
                buttonClear.Background = Brushes.RoyalBlue;
                Click2 = 1;
            }
            else if (Click2 == 1)
            {
                buttonClear.Background = Brushes.Beige;
                Click2 = 0;
            }
            textBoxReception.Clear();
        }

        private void TextBoxKey_Up(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter)
            {
                SendMessage();
                e.Handled = true;
            }
        }


        private void textBoxEmission_TextChanged(object sender, TextChangedEventArgs e)
        {

        }

        private void ButtonTest_Click(object sender, RoutedEventArgs e)
        {
            // byte[] byteList = new byte[20];
            //for (int i = 0; i < 20; i++)
            //{
            //  byteList[i] = (byte)(2 * i); 
            //}

            //if (serialPort1 != null && serialPort1.IsOpen)
            //{
            //serialPort1.Write(byteList, 0, byteList.Length);
            //  textBoxReception.Text+=("Tableau de bytes envoyé !");
            //}
            //else
            //{
            //    textBoxReception.Text+=("Le port série n'est pas ouvert !");
            //  }

            //var payload = Encoding.UTF8.GetBytes("Bonjour");
            //UartEncodeAndSendMessage(0x0080, payload.Length, payload);
            // textBoxReception.Text += ("Message envoyé !");
            string randomText = "Message " + random.Next(100, 999);
            byte[] textPayload = System.Text.Encoding.ASCII.GetBytes(randomText);
            ProcessDecodedMessage((int)CommandID.TEXT, textPayload.Length, textPayload);

            /*for (byte ledNum = 1; ledNum <= 2; ledNum++)
            {
                byte ledState = (byte)random.Next(0, 2);
                byte[] ledPayload = new byte[] { ledNum, ledState };
                ProcessDecodedMessage((int)CommandID.LED, ledPayload.Length, ledPayload);
            }*/

            byte[] irPayload = new byte[]
            {
        (byte)random.Next(10, 81), // gauche
        (byte)random.Next(10, 81), // centre
        (byte)random.Next(10, 81)  // droite
            };
            ProcessDecodedMessage((int)CommandID.IR_DISTANCE, irPayload.Length, irPayload);

            byte[] speedPayload = new byte[]
            {
        (byte)random.Next(0, 101), // moteur gauche
        (byte)random.Next(0, 101)  // moteur droit
            };
            ProcessDecodedMessage((int)CommandID.SPEED, speedPayload.Length, speedPayload);
        }


        public void SerialPort1_DataReceived(object sender, DataReceivedArgs e)
        {
            for (int i = 0; i < e.Data.Length; i++)
            {
                robot.byteListReceived.Enqueue(e.Data[i]);
            }
        }

        private void TimerAffichage_Tick(object sender, EventArgs e)
        {
            if (robot.byteListReceived.Count > 0)
            {
                StringBuilder sb = new StringBuilder();

                while (robot.byteListReceived.Count > 0)
                {
                    byte b = robot.byteListReceived.Dequeue();

                    DecodeMessage(b);

                    //sb.Append("0x" + b.ToString("X2") + " ");
                }

                //textBoxReception.Text += sb.ToString() + "\n";
            }
        }


        private byte CalculateChecksum(int msgFunction, int msgPayloadLength, byte[] msgPayload)
        {
            byte checksum = 0;
            checksum ^= 0xFE;
            checksum ^= (byte)(msgFunction >> 8);
            checksum ^= (byte)(msgFunction >> 0);
            checksum ^= (byte)(msgPayloadLength >> 8);
            checksum ^= (byte)(msgPayloadLength >> 0);
            for (int i = 0; i < msgPayloadLength; i++)
            {
                checksum ^= msgPayload[i];
            }

            return checksum;
        }

        private void UartEncodeAndSendMessage(int msgFunction, int msgPayloadLength, byte[] msgPayload)
        {
            int totalLength = 1 + 2 + 2 + msgPayloadLength + 1;
            byte[] frame = new byte[totalLength];

            int index = 0;
            frame[index++] = 0xFE;


            frame[index++] = (byte)(msgFunction >> 8);
            frame[index++] = (byte)(msgFunction >> 0);


            frame[index++] = (byte)(msgPayloadLength >> 8);
            frame[index++] = (byte)(msgPayloadLength >> 0);

            for (int i = 0; i < msgPayloadLength; i++)
                frame[index++] = msgPayload[i];


            frame[index++] = CalculateChecksum(msgFunction, msgPayloadLength, msgPayload);


            serialPort1.Write(frame, 0, frame.Length);
        }

        StateReception rcvState = StateReception.Waiting;
        int msgFunction = 0;
        int msgPayloadLength = 0;
        byte[] msgPayload;
        int payloadIndex = 0;
        byte checksumCalc = 0;
        byte checksumRecv = 0;

        private void DecodeMessage(byte c)
        {
            switch (rcvState)
            {
                case StateReception.Waiting:
                    if (c == 0xFE)
                    {
                        rcvState = StateReception.FunctionMSB;
                    }
                    break;

                case StateReception.FunctionMSB:
                    msgFunction = ((int)c << 8);
                    rcvState = StateReception.FunctionLSB;
                    break;

                case StateReception.FunctionLSB:
                    msgFunction += c;
                    rcvState = StateReception.PayloadLengthMSB;
                    break;

                case StateReception.PayloadLengthMSB:
                    msgPayloadLength = ((int)c << 8);
                    rcvState = StateReception.PayloadLengthLSB;
                    break;

                case StateReception.PayloadLengthLSB:
                    msgPayloadLength += c;
                    msgPayload = new byte[msgPayloadLength];
                    payloadIndex = 0;
                    rcvState = StateReception.Payload;
                    break;

                case StateReception.Payload:
                    msgPayload[payloadIndex] = c;
                    payloadIndex++;
                    if (payloadIndex >= msgPayloadLength)
                        rcvState = StateReception.CheckSum;
                    break;

                case StateReception.CheckSum:
                    checksumRecv = c;
                    checksumCalc = CalculateChecksum(msgFunction, msgPayloadLength, msgPayload);
                    if (checksumCalc == checksumRecv)
                    {
                        ProcessDecodedMessage(msgFunction, msgPayloadLength, msgPayload);
                    }
                    else
                    {
                        textBoxReception.AppendText("Message invalide (checksum incorrect)\n");
                    }
                    rcvState = StateReception.Waiting;
                    break;
                default:
                    rcvState = StateReception.Waiting;
                    break;
            }
        }

        private void CheckBox_Checked(object sender, RoutedEventArgs e)
        {

        }
        private Random random = new Random();

        

        enum CommandID
        {
            TEXT = 0x0080,
            //LED = 0x0020,
            IR_DISTANCE = 0x0030,
            SPEED = 0x0040
        }

        void ProcessDecodedMessage(int msgFunction, int msgPayloadLength, byte[] msgPayload)
        {
            switch ((CommandID)msgFunction)
            {
                case CommandID.TEXT:
                    string text = System.Text.Encoding.ASCII.GetString(msgPayload, 0, msgPayloadLength);
                    textBoxReception.AppendText("Texte reçu : " + text + "\n");
                    break;

                /*case CommandID.LED:
                    byte ledNumber = msgPayload[0];
                    byte ledState = msgPayload[1];

                    if (ledNumber == 1)
                    {
                        LED1 = ledState;
                    }
                    else if (ledNumber == 2)
                    {
                        LED2 = ledState;
                    }
                    else if (ledNumber == 3)
                    {
                        LED3 = ledState;
                    }
                    break;*/


                case CommandID.IR_DISTANCE:
                    IRLeft.Text = "Capteur Gauche" + " " + msgPayload[0].ToString() + " cm";
                    IRCenter.Text = "Capteur Centre" +" " + msgPayload[1].ToString() + " cm";
                    IRRight.Text = "Capteur Droit" +" " + msgPayload[2].ToString() + " cm";
                    break;

                case CommandID.SPEED:
                    MotorLeft.Text = "Vitesse Gauche" + " " + msgPayload[0].ToString() + "%";
                    MotorRight.Text = "Vitesse Droite" + " " + msgPayload[1].ToString() + "%";
                    break;

                default:
                    textBoxReception.AppendText($"Commande inconnue : 0x{msgFunction:X4}\n");
                    break;
            }
        }

    }

}
