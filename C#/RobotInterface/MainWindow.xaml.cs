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
        Waiting,          // On attend le début du message (0xFE)
        FunctionMSB,      // Lecture du premier octet de la fonction
        FunctionLSB,      // Lecture du deuxième octet de la fonction
        PayloadLengthMSB, // Lecture du premier octet de la longueur
        PayloadLengthLSB, // Lecture du deuxième octet de la longueur
        Payload,          // Lecture du payload
        CheckSum          // Lecture du checksum
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
            //textBoxReception.Text += "\n Reçu : " + message;
            //textBoxEmission.Text = "";
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

            var payload = Encoding.UTF8.GetBytes("Bonjour");
            UartEncodeAndSendMessage(0x0080, payload.Length, payload);
            textBoxReception.Text+=("Message envoyé !");
        }


        public void SerialPort1_DataReceived(object sender, DataReceivedArgs e)
        {
            foreach (byte b in e.Data)
            {
               robot.byteListReceived.Enqueue(b); 
               DecodeMessage(b);
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


                    sb.Append("0x" + b.ToString("X2") + " ");
                }

                textBoxReception.Text += sb.ToString() + "\n"; 
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
                        checksumCalc = c;
                    }
                    break;

                case StateReception.FunctionMSB:
                    msgFunction = c << 8;
                    checksumCalc ^= c;
                    rcvState = StateReception.FunctionLSB;
                    break;

                case StateReception.FunctionLSB:
                    msgFunction |= c;
                    checksumCalc ^= c;
                    rcvState = StateReception.PayloadLengthMSB;
                    break;

                case StateReception.PayloadLengthMSB:
                    msgPayloadLength = c << 8;
                    checksumCalc ^= c;
                    rcvState = StateReception.PayloadLengthLSB;
                    break;

                case StateReception.PayloadLengthLSB:
                    msgPayloadLength |= c;
                    checksumCalc ^= c;

                    
                    msgPayload = new byte[msgPayloadLength];
                    payloadIndex = 0;

                    if (msgPayloadLength == 0)
                        rcvState = StateReception.CheckSum;
                    else
                        rcvState = StateReception.Payload;
                    break;

                case StateReception.Payload:
                    msgPayload[payloadIndex++] = c;
                    checksumCalc ^= c;

                    if (payloadIndex >= msgPayloadLength)
                        rcvState = StateReception.CheckSum;
                    break;

                case StateReception.CheckSum:
                    checksumRecv = c;
                    if (checksumCalc == checksumRecv)
                    {
                       
                        textBoxReception.Text+=("Success, on a un message valide");
                    }
                    else
                    {
                        textBoxReception.Text+=("Message invalide (checksum incorrect) !");
                    }

                    rcvState = StateReception.Waiting; 
                    break;
            }
        } 

    }
}
