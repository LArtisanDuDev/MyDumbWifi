#include "MyDumbWifi.h"

// My wifi system is a pure mess. Pod are swapping there channels on each reboot
// My esp board is keeping connecting on last connected channel even if this channel
// in now unreachable.
// And more, sometimes a pod don't let my board connecting (why ?) then i try the
// next best reachable pod....

MyDumbWifi::MyDumbWifi()
{
    _debug = false;
    for (int i = 0; i < NB_CHANNELS_MAX; i++)
    {
        _bestWifiChannels[i] = -1;
    }
}

MyDumbWifi::~MyDumbWifi()
{
}

void MyDumbWifi::setDebug(bool debug) {
    _debug =debug;
}


void MyDumbWifi::retrieveWiFiChannels(const char *ssid)
{
    int iter = 0;
    if (int32_t n = WiFi.scanNetworks())
    {
        for (uint8_t i = 0; i < n; i++)
        {
            if (_debug)
            {
                Serial.print("SSID :");
                Serial.print(WiFi.SSID(i));
                Serial.print(" Channel :");
                Serial.print(WiFi.channel(i));
                Serial.print(" RSSI :");
                Serial.print(WiFi.RSSI(i));
                Serial.print(" BSSID :");
                Serial.println(WiFi.BSSIDstr(i));
            }
            if (strcmp(ssid, WiFi.SSID(i).c_str()) == 0)
            {
                _bestWifiChannels[iter++] = WiFi.channel(i);
            }
        }
    }

    if (_debug)
    {
        Serial.print("Array channels : ");
        for (int i = 0; i < NB_CHANNELS_MAX; i++)
        {
            Serial.print(_bestWifiChannels[i]);
            Serial.print(" ");
        }
        Serial.println("");
    }
}

bool MyDumbWifi::connectToWiFi(const char *wifi_ssid, const char *wifi_key)
{
    _wifi_ssid = wifi_ssid;
    _wifi_key = wifi_key;
    if (_debug)
    {
        Serial.println("connectToWiFi()");
    }
    retrieveWiFiChannels(_wifi_ssid);

    int iter = 0;
    bool connected = false;
    while (iter < NB_CHANNELS_MAX && _bestWifiChannels[iter] != -1 && connected == false)
    {
        connected = retryConnect(_bestWifiChannels[iter++]);
    }
    return connected;
}

bool MyDumbWifi::retryConnect(int32_t bestChannel)
{

    if (_debug)
    {
        Serial.print("Trying connection on channel : ");
        Serial.println(bestChannel);
    }

    uint8_t wifiAttempts = 0;
    while (wifiAttempts == 0 || (WiFi.status() != WL_CONNECTED && wifiAttempts < 30))
    {
        if (wifiAttempts % 10 == 0)
        {
            WiFi.disconnect(true, true);
            WiFi.mode(WIFI_STA);
            if (bestChannel != -1)
            {
                WiFi.begin(_wifi_ssid, _wifi_key, bestChannel);
            }
            else
            {
                WiFi.begin(_wifi_ssid, _wifi_key);
            }
        }
        wifiAttempts++;
        Serial.print(".");
        delay(500);
    }
    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.print("Connected with IP: ");
        Serial.println(WiFi.localIP());
        Serial.print("Channel : ");
        Serial.println(WiFi.channel());
        if (_debug)
        {
            WiFi.printDiag(Serial);
        }
        return true;
    }
    else
    {
        WiFi.disconnect(true, true);
        return false;
    }
}
