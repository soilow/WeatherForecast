#include <libs/app.h>
#include <nlohmann/json.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>

const std::string kPathToConfig = kProjectPath + "config/config.json";

void ConfigParse(std::vector<std::string>& cities, int& refresh_rate, int& forecast_days_number) {
    using json = nlohmann::json;
    
    std::ifstream json_file(kPathToConfig);
    
    if (!json_file.is_open()) {
        std::cerr << "Can't open json config!\n";
        exit(EXIT_FAILURE);
    }
    
    json data;
    json_file >> data;

    json_file.close();
    
    cities = data["cities"];
    refresh_rate = data["refresh_rate"];
    forecast_days_number = data["forecast_days_number"];
}

void CheckCitiesRange(int& i, std::vector<CityForecast>& weather_cities) {
    if (i < 0) {
        i = weather_cities.size()-1;
    } else if (i == weather_cities.size()) {
        i = 0;
    }
}

void CheckDaysRange(int& forecast_days_number) {
    if (forecast_days_number < 1) {
        forecast_days_number = 1;
    }
}

void KeystrokeHandling (char& ch, int& i, int& forecast_days_number) {
    if (ch == 'n') {
        ++i;
    } else if (ch == 'p') {
        --i;
    } else if (ch == '+') {
        ++forecast_days_number;
    } else if (ch == '-') {
        --forecast_days_number;
    }
}

void WeatherThread(const int& refresh_rate, std::vector<CityForecast>& weather_cities, int& forecast_days_number, int& i, char& ch) {
    while (ch != 27) {
        std::this_thread::sleep_for(std::chrono::seconds(refresh_rate));
        
        std::system("clear");

        weather_cities[i].Forecast(forecast_days_number);
    }
}

int main() {
    std::vector<std::string> city_names;
    int refresh_rate;
    int forecast_days_number;
    
    ConfigParse(city_names, refresh_rate, forecast_days_number);
    
    std::vector<CityForecast> weather_cities;

    for (const std::string& i : city_names) {
        weather_cities.push_back(CityForecast(i, refresh_rate));
    }
    
    int i = 0;
    char ch = '\0';
    
    std::thread weather_thread_obj(WeatherThread, std::ref(refresh_rate), std::ref(weather_cities), std::ref(forecast_days_number), std::ref(i), std::ref(ch));
    
    do {
        std::system("clear");
        
        KeystrokeHandling(ch, i, forecast_days_number);
        
        CheckCitiesRange(i, weather_cities);
        CheckDaysRange(forecast_days_number);
        
        weather_cities[i].Forecast(forecast_days_number);
    } while ((ch = getch()) != 27);
    
    weather_thread_obj.detach();

    return 0;
}
