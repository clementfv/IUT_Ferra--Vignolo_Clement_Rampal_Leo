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

namespace RobotInterface
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        ExtendedSerialPort serialPort1;
        public MainWindow()
        {
            InitializeComponent();
            serialPort1 = new ExtendedSerialPort("COM4", 115200, Parity.None, 8, StopBits.One);
            serialPort1.Open();
        }

        int Click = 0;

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

        private void TextBoxKey_Up(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter)
            {
                SendMessage();
                e.Handled = true;
            }
        }
    }
}