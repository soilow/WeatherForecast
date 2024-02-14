#pragma once

#include <images/images.cpp>
#include <utils/utils.h>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

#include <iostream>
#include <string>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <fstream>

using json = nlohmann::json;

const std::string kWeatherAPIURL = "https://api.open-meteo.com/v1/forecast";

const std::string kNinjaAPIURL = "https://api.api-ninjas.com/v1/city?name=";
const std::string kNinjaAPIKey = "CgSdECuZO5TLqLHvcoV5CQ==IzsjLPNMz2hIUSbq";

const std::string kPathToCachedJSONs = kProjectPath + "cache/";
const std::string kJSONExtension = ".json";

const int kNumberOfDataLines = 5;
const int kPartsOfADay = 4;

class CityForecast {
public:
    CityForecast(std::string city, int refresh_rate);
    ~CityForecast();
    
    void Forecast(int days) const;
private:
    std::string city_;
    int refresh_rate_;
    
    std::string country_;
    std::string latitude_;
    std::string longitude_;
    
    void GetCityInfo(json& city_info, std::string& city) const;
    std::string GetCurrentWeatherInfo() const;
    std::string GetForecastInfo(int& days) const;
    
    std::string TableHeading(int& day_number) const;
    std::string TableBottom() const;
    
    std::string* FormatInfo(json* current_weather, const std::string*& current_image, int index) const;
    std::string WeatherFormatting(const int& weather_code, const std::string*& ascii_image) const;
    std::string TemperatureFormatting(const int& temp, const int& apparent_temp) const;
    std::string WindFormatting(const double& wind_speed, const double& wind_direction) const;
    std::string VisibilityFormatting(const double& visibility) const;
    std::string PrecipitationFormatting(const double& precipitation) const;
};
