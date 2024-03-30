#ifndef MyDumbWifi_h
#define MyDumbWifi_h
#endif

#include <WiFi.h>

// Why 20 ? mmhh, why not...
#define NB_CHANNELS_MAX 20
class MyDumbWifi
{
public:
  MyDumbWifi();
  ~MyDumbWifi();
  bool connectToWiFi(const char *wifi_ssid, const char *wifi_key);
  void setDebug(bool debug);

protected:
  void retrieveWiFiChannels(const char *ssid);
  bool retryConnect(int32_t bestChannel);
  int32_t _bestWifiChannels[NB_CHANNELS_MAX];
  const char *_wifi_ssid;
  const char *_wifi_key;
  bool _debug;
};