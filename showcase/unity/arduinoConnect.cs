using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO.Ports;
using System;
using System.Threading;

public class arduinoConnect : MonoBehaviour
{
    public static arduinoConnect instance { set; get; }


    SerialPort serialPort;

    [SerializeField] string[] ports;

    public Thread serialThread;

    [SerializeField] bool isConnect;


    private void Awake()
    {
        instance = this;
    }

    void Start()
    {
    }

    public void arduinoOtoConnect()
    {
        if(!isConnect)
        {
            ports = SerialPort.GetPortNames();
            int portNumber = ports.Length;
            for (int i = portNumber - 1; i >= 0; i--)
            {
                try
                {
                    if (serialPort != null && serialPort.IsOpen)
                    {
                        serialPort.Close();
                        isConnect = false;
                        Thread.Sleep(2000); // Portun tamamen kapanmasını sağlamak için bekle
                    }

                    Debug.Log(ports[i]);
                    serialPort = new SerialPort(ports[i], 115200);
                    Debug.Log("Bağlantı kuruluyor: " + ports[i]);
                    serialPort.Open();

                    serialPort.WriteLine("          WHO_ARE_YOU#");
                    for (int k = 0; k < 50; k++)
                    {
                        string response = serialPort.ReadLine();
                        Debug.Log("arduino ilk veri: " + response);
                        if (response.Contains("arduino"))
                        {
                            isConnect = true;
                            Debug.Log("Arduino found on port: " + ports[i]);

                            serialPort.WriteLine("Value");
                            return;
                        }
                    }
                }
                catch (TimeoutException)
                {
                    Debug.LogWarning("Bağlantı zaman aşımı: " + ports[i]);
                }
                catch (Exception e)
                {
                    Debug.Log("Error with port " + ports[i] + ": " + e.Message);
                }
            }
        }
        else
        {
            //canvasManager.instance.bilgiText.text = "Zaten bir com'a bağla ilk önce bağlantıyı kesiniz.";
        }
    }

    public void arduinoManuelConnect(object com)
    {
        if (!isConnect)
        {
            try
            {
                if (serialPort != null && serialPort.IsOpen)
                {
                    serialPort.Close();
                    isConnect = false;
                    Thread.Sleep(2000); // Portun tamamen kapanmasını sağlamak için bekle
                }

                serialPort = new SerialPort(com.ToString(), 115200);
                serialPort.Open();
                Debug.Log("Bağlantı kuruluyor: " + com);
                serialPort.WriteLine("          WHO_ARE_YOU#");
                for (int k = 0; k < 50; k++)
                {
                    string response = serialPort.ReadLine();
                    Debug.Log("arduino ilk veri: " + response);

                    if (response.Contains("arduino"))
                    {
                        isConnect = true;
                        Debug.Log("Arduino found on port: " + com);

                        serialPort.WriteLine("Value");
                        return;
                    }
                }
            }
            catch (TimeoutException)
            {
                Debug.LogWarning("Bağlantı zaman aşımı: " + com);
            }
            catch (Exception e)
            {
                Debug.Log("Error with port " + com + ": " + e.Message);
            }
        }
        else
        {
        }
    }

    public void notConnectArduino()
    {
        if (serialPort != null && serialPort.IsOpen)
        {
            serialPort.Close();
            isConnect = false;
        }
        else
        {

        }
    }

    void Update()
    {
        if (serialPort != null && serialPort.IsOpen && isConnect)
        {
            try
            {
                string gelenVeri = serialPort.ReadLine();
                if (gelenVeri != "" && gelenVeri != null)
                {
                    Debug.Log("Arduino'dan Gelen Veri: " + gelenVeri);
                }
            }
            catch (TimeoutException)
            {
                Debug.LogWarning("Veri okuma zaman aşımı.");
            }
        }
    }

    public string[] getPortNames()
    {
        return SerialPort.GetPortNames();
    }

    void OnApplicationQuit()
    {
        if (serialPort != null && serialPort.IsOpen)
            serialPort.Close();

        if (serialThread != null && serialThread.IsAlive)
            serialThread.Abort();
    }
}
