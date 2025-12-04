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
    public partial class MainWindow : Window
    {
        ExtendedSerialPort serialPort1;
        private DispatcherTimer timerAffichage;
        Robot robot = new Robot();
        public MainWindow()
        {
            InitializeComponent();
            serialPort1 = new ExtendedSerialPort("COM6", 115200, Parity.None, 8, StopBits.One);
            serialPort1.DataReceived += SerialPort1_DataReceived;
            serialPort1.Open();
            timerAffichage = new DispatcherTimer();
            timerAffichage.Interval = new TimeSpan(0, 0, 0, 0, 100);
            timerAffichage.Tick += TimerAffichage_Tick;
            timerAffichage.Start();
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



        int Click, Click2 = 0;

        private void SendMessage()
        {
            string message = textBoxEmission.Text;
           // textBoxReception.Text += "\n Reçu : " + message;
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
            byte[] byteList = new byte[20];
            for (int i = 0; i < 20; i++)
            {
                byteList[i] = (byte)(2 * i); 
            }

            if (serialPort1 != null && serialPort1.IsOpen)
            {
                serialPort1.Write(byteList, 0, byteList.Length);
                MessageBox.Show("Tableau de bytes envoyé !");
            }
            else
            {
                MessageBox.Show("Le port série n'est pas ouvert !");
            }
        }


        public void SerialPort1_DataReceived(object sender, DataReceivedArgs e)
        {
            // e.Data contient les octets reçus
            foreach (byte b in e.Data)
            {
                robot.byteListReceived.Enqueue(b); // Ajouter chaque octet à la FIFO
            }
        }


    }
}