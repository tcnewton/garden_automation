#include "Forecast.h"
#include "time.h"
#include <ArduinoJson.h>

/**
 * Constructor.
 */
Forecast_OpenWeather::Forecast_OpenWeather(String api_key){
  _api_key = api_key;
  _unit_type = "metric";  //or "imperial"

  _getSuccess = false;
}

void Forecast_OpenWeather::setLocation(float lat, float lon){
  //_lat = lat;
  //_lon = lon;

  char _llat[10], _llon[10];

  sprintf( _llat, "%d.%05d", (int) lat, (int)(lat*100000)%100000) ;
  sprintf( _llon, "%d.%05d", (int) lon, (int)(lon*100000)%100000) ;

  _lat = _llat;
  _lon = _llon;

}

void Forecast_OpenWeather::setUnit(String unit_type){
  if( unit_type == "imperial"){
    _unit_type = "imperial";
  } else {
    _unit_type = "metric";
  }
}

String Forecast_OpenWeather::latitude(){
  return _lat;
}

String Forecast_OpenWeather::longitude(){
  return _lon;
}

bool Forecast_OpenWeather::weatherNow(){

	_getSuccess = false;

  //------------ OpenWeather API ----------------------------
  String  url  = "https://api.openweathermap.org/data/2.5/onecall";
          url += "?lat="  + _lat;
          url += "&lon="   + _lon;
          url += "&exclude=" + _excludes;
          url += "&appid=" + _api_key;

  DEBUG_PRINTLN(url);

  _http.begin(url);  
  int httpCode = _http.GET();
  if(httpCode > 0) {
    if(httpCode == HTTP_CODE_OK) {
      _getSuccess = true;

      String payload = _http.getString();
      
      DEBUG_PRINTLN(payload);DEBUG_PRINTLN();
      
      _http.end();
      
      DynamicJsonDocument doc(1024);
      auto error = deserializeJson(doc, payload.c_str());

      if(error) {
        _getSuccess = false;
        return false;
      }else {
        const char* w      = doc["weather"][0]["main"];
        const char* w_icon = doc["weather"][0]["icon"];
        _weather = String(w);
        _weather_icon =  String(w_icon) + ".png";
        _temperature = doc["main"]["temp"];
        _feels_like = doc["main"]["feels_like"];
        _temp_min = doc["main"]["temp_min"];
        _temp_max = doc["main"]["temp_max"];
        _pressure = doc["main"]["pressure"];
        _humidity = doc["main"]["humidity"];
        _windspeed = doc["wind"]["speed"];
        _winddeg = doc["wind"]["deg"];
        _cloudiness = doc["clouds"]["all"];  // %
        _dt = doc["dt"];
        _sunrise = doc["sys"]["sunrise"];
        _sunset = doc["sys"]["sunset"];

        _getSuccess = true;
        return true;
      }
      // _weather      = payload.substring( 14+payload.indexOf("description"), payload.indexOf("icon")-3);
      // _temperature  = payload.substring( 6+payload.indexOf("temp\""),payload.indexOf("pressure")-2).toFloat();
      // _pressure     = payload.substring( 10+payload.indexOf("pressure"),payload.indexOf("humidity")-2).toFloat();
      // _humidity     = payload.substring( 10+payload.indexOf("humidity"),payload.indexOf("temp_min")-2).toFloat();
      // _windspeed    = payload.substring( 7+payload.indexOf("speed"), payload.indexOf("deg")-2).toFloat();
      // _winddeg      = payload.substring( 5+payload.indexOf("deg"), payload.indexOf("},\"clouds")).toFloat();
      // _cloudiness   = payload.substring( 15+payload.indexOf("clouds\":{\"all"),payload.indexOf("dt")-3).toFloat();
      
      // _sunrise      = (time_t) payload.substring( 9+payload.indexOf("sunrise"),payload.indexOf("sunset")-2).toFloat();
      // _sunset       = (time_t) payload.substring( 8+payload.indexOf("sunset"),payload.indexOf("},\"id\"")).toFloat();
      


      
    }
  } else {
    DEBUG_PRINT("[HTTP] GET... failed, error: "); DEBUG_PRINTLN(_http.errorToString(httpCode).c_str());
    _http.end();
    _getSuccess = false;
    return false;
  }
}

float Forecast_OpenWeather::readTemperature(){
  while( !_getSuccess ) { weatherNow(); }
  return _temperature;
}

float Forecast_OpenWeather::readTempMin(){
  while( !_getSuccess ) { weatherNow(); }
  return _temp_min;
}

float Forecast_OpenWeather::readTempMax(){
  while( !_getSuccess ) { weatherNow(); }
  return _temp_max;
}

float Forecast_OpenWeather::readHumidity(){
  while( !_getSuccess ) { weatherNow(); }
  return _humidity;
}

String Forecast_OpenWeather::readWeather(){
  while( !_getSuccess ) { weatherNow(); }
  return _weather;
}

bool Forecast_OpenWeather::wificonnect(char* ssid, char* pass){
  WiFi.begin(ssid, pass);
  
  DEBUG_PRINTLN();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    DEBUG_PRINT(".");
  }
  DEBUG_PRINTLN();
  DEBUG_PRINTLN("WiFi connected");
  DEBUG_PRINT("IP address: ");
  DEBUG_PRINTLN(WiFi.localIP());

}

String Forecast_OpenWeather::getVersion(){
  return (String)("[W3E_OpenWeather] Version ") + String(_version);
}