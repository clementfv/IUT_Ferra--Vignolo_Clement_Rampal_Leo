using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Threading;
using ExtendedSerialPort_NS;
using KeyboardHook_NS;

namespace RobotInterface
{
    public partial class MainWindow : Window
    {
        ExtendedSerialPort serialPort1;
        DispatcherTimer timerAffichage;
        Robot robot = new Robot();
        GlobalKeyboardHook _globalKeyboardHook;
        bool autoControlActivated = false;

        // Définition des Etats du Robot
        public enum StateRobot
        {
            STATE_ATTENTE = 0,
            STATE_ATTENTE_EN_COURS = 1,
            STATE_AVANCE = 2,
            STATE_AVANCE_EN_COURS = 3,
            STATE_TOURNE_GAUCHE = 4,
            STATE_TOURNE_GAUCHE_EN_COURS = 5,
            STATE_TOURNE_DROITE = 6,
            STATE_TOURNE_DROITE_EN_COURS = 7,
            STATE_TOURNE_SUR_PLACE_GAUCHE = 8,
            STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS = 9,
            STATE_TOURNE_SUR_PLACE_DROITE = 10,
            STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS = 11,
            STATE_ARRET = 12,
            STATE_ARRET_EN_COURS = 13,
            STATE_RECULE = 14,
            STATE_RECULE_EN_COURS = 15
        }

        // IDs des commandes 
        public enum CommandID
        {
            TEXT = 0x0080,
            LED = 0x0020,
            IR_DISTANCE = 0x0030,
            SPEED = 0x0040,
            STATE_TELEMETRY = 0x0050,
            SET_STATE = 0x0051,
            SET_MANUAL_CONTROL = 0x0052
        }

        // Machine à états de réception
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

        StateReception rcvState = StateReception.Waiting;
        int msgDecodedFunction = 0;
        int msgDecodedPayloadLength = 0;
        byte[] msgDecodedPayload;
        int msgDecodedPayloadIndex = 0;

        public MainWindow()
        {
            InitializeComponent();

            // Init Serial Port
            serialPort1 = new ExtendedSerialPort("COM4", 115200, Parity.None, 8, StopBits.One);
            serialPort1.DataReceived += SerialPort1_DataReceived;
            serialPort1.Open();

            // Init Timer Affichage
            timerAffichage = new DispatcherTimer();
            timerAffichage.Interval = TimeSpan.FromMilliseconds(100);
            timerAffichage.Tick += TimerAffichage_Tick;
            timerAffichage.Start();

            // Init Keyboard Hook
            _globalKeyboardHook = new GlobalKeyboardHook();
            _globalKeyboardHook.KeyPressed += _globalKeyboardHook_KeyPressed;
        }


        private void _globalKeyboardHook_KeyPressed(object? sender, KeyArgs e)
        {
            if (!autoControlActivated)
            {
                switch (e.keyCode)
                {
                    case KeyCode.UP:
                        UartEncodeAndSendMessage((int)CommandID.SET_STATE, 1, new byte[] { (byte)StateRobot.STATE_AVANCE });
                        break;
                    case KeyCode.DOWN:
                        UartEncodeAndSendMessage((int)CommandID.SET_STATE, 1, new byte[] { (byte)StateRobot.STATE_ARRET });
                        break;
                    case KeyCode.LEFT:
                        UartEncodeAndSendMessage((int)CommandID.SET_STATE, 1, new byte[] { (byte)StateRobot.STATE_TOURNE_SUR_PLACE_GAUCHE });
                        break;
                    case KeyCode.RIGHT:
                        UartEncodeAndSendMessage((int)CommandID.SET_STATE, 1, new byte[] { (byte)StateRobot.STATE_TOURNE_SUR_PLACE_DROITE });
                        break;
                    case KeyCode.PAGEDOWN:
                        UartEncodeAndSendMessage((int)CommandID.SET_STATE, 1, new byte[] { (byte)StateRobot.STATE_RECULE });
                        break;
                }
            }
        }


        private void SerialPort1_DataReceived(object sender, DataReceivedArgs e)
        {
            foreach (byte b in e.Data)
            {
                robot.byteListReceived.Enqueue(b);
            }
        }

        private void TimerAffichage_Tick(object sender, EventArgs e)
        {
            while (robot.byteListReceived.Count > 0)
            {
                byte b = robot.byteListReceived.Dequeue();
                DecodeMessage(b);
            }
        }


        private void UartEncodeAndSendMessage(int msgFunction, int msgPayloadLength, byte[] msgPayload)
        {
            byte[] frame = new byte[msgPayloadLength + 6];
            int index = 0;

            frame[index++] = 0xFE;
            frame[index++] = (byte)(msgFunction >> 8);
            frame[index++] = (byte)(msgFunction);
            frame[index++] = (byte)(msgPayloadLength >> 8);
            frame[index++] = (byte)(msgPayloadLength);

            for (int i = 0; i < msgPayloadLength; i++)
            {
                frame[index++] = msgPayload[i];
            }

            frame[index++] = CalculateChecksum(msgFunction, msgPayloadLength, msgPayload);

            if (serialPort1.IsOpen)
                serialPort1.Write(frame, 0, frame.Length);
        }

        private byte CalculateChecksum(int msgFunction, int msgPayloadLength, byte[] msgPayload)
        {
            byte checksum = 0xFE;
            checksum ^= (byte)(msgFunction >> 8);
            checksum ^= (byte)(msgFunction);
            checksum ^= (byte)(msgPayloadLength >> 8);
            checksum ^= (byte)(msgPayloadLength);

            foreach (byte b in msgPayload)
            {
                checksum ^= b;
            }
            return checksum;
        }


        private void DecodeMessage(byte c)
        {
            switch (rcvState)
            {
                case StateReception.Waiting:
                    if (c == 0xFE) rcvState = StateReception.FunctionMSB;
                    break;

                case StateReception.FunctionMSB:
                    msgDecodedFunction = (c << 8);
                    rcvState = StateReception.FunctionLSB;
                    break;

                case StateReception.FunctionLSB:
                    msgDecodedFunction += c;
                    rcvState = StateReception.PayloadLengthMSB;
                    break;

                case StateReception.PayloadLengthMSB:
                    msgDecodedPayloadLength = (c << 8);
                    rcvState = StateReception.PayloadLengthLSB;
                    break;

                case StateReception.PayloadLengthLSB:
                    msgDecodedPayloadLength += c;
                    msgDecodedPayload = new byte[msgDecodedPayloadLength];
                    msgDecodedPayloadIndex = 0;
                    if (msgDecodedPayloadLength == 0) rcvState = StateReception.CheckSum;
                    else rcvState = StateReception.Payload;
                    break;

                case StateReception.Payload:
                    msgDecodedPayload[msgDecodedPayloadIndex++] = c;
                    if (msgDecodedPayloadIndex >= msgDecodedPayloadLength)
                        rcvState = StateReception.CheckSum;
                    break;

                case StateReception.CheckSum:
                    byte calculatedChecksum = CalculateChecksum(msgDecodedFunction, msgDecodedPayloadLength, msgDecodedPayload);
                    if (calculatedChecksum == c)
                    {
                        ProcessDecodedMessage(msgDecodedFunction, msgDecodedPayloadLength, msgDecodedPayload);
                    }
                    rcvState = StateReception.Waiting;
                    break;

                default:
                    rcvState = StateReception.Waiting;
                    break;
            }
        }
        void ProcessDecodedMessage(int msgFunction, int msgPayloadLength, byte[] msgPayload)
        {
            switch ((CommandID)msgFunction)
            {
                case CommandID.TEXT:
                    textBoxReception.AppendText("Reçu : " + Encoding.UTF8.GetString(msgPayload) + "\n");
                    break;

                case CommandID.IR_DISTANCE:
                    IRLeft.Text = "Capteur Gauche : " + msgPayload[0] + " cm";
                    IRCenter.Text = "Capteur Centre : " + msgPayload[1] + " cm";
                    IRRight.Text = "Capteur Droit : " + msgPayload[2] + " cm";
                    break;

                case CommandID.SPEED:
                    MotorLeft.Text = "Vitesse Gauche : " + (sbyte)msgPayload[0] + "%";
                    MotorRight.Text = "Vitesse Droite : " + (sbyte)msgPayload[1] + "%";
                    break;

                case CommandID.STATE_TELEMETRY:
                    int timestamp = (msgPayload[1] << 24) + (msgPayload[2] << 16) + (msgPayload[3] << 8) + msgPayload[4];
                    StateRobot currentState = (StateRobot)msgPayload[0];
                    textBoxReception.AppendText($"State: {currentState} - Time: {timestamp} ms\n");
                    break;
            }
        }


        private void ButtonEnvoyer_Click(object sender, RoutedEventArgs e)
        {
            SendMessage();
            buttonEnvoyer.Background = (buttonEnvoyer.Background == Brushes.RoyalBlue) ? Brushes.Beige : Brushes.RoyalBlue;
        }

        private void SendMessage()
        {
            if (serialPort1.IsOpen)
            {
                serialPort1.WriteLine(textBoxEmission.Text);
                textBoxReception.AppendText("Envoyé : " + textBoxEmission.Text + "\n");
                textBoxEmission.Clear();
            }
        }

        private void TextBoxEmission_KeyUp(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter) SendMessage();
        }

        private void ButtonClear_Click(object sender, RoutedEventArgs e)
        {
            textBoxReception.Clear();
        }

        private void ButtonTest_Click(object sender, RoutedEventArgs e)
        {
            byte[] irPayload = new byte[] { (byte)new Random().Next(10, 80), (byte)new Random().Next(10, 80), (byte)new Random().Next(10, 80) };
            ProcessDecodedMessage((int)CommandID.IR_DISTANCE, 3, irPayload);
        }
    }
}