#include "app.h"

CityForecast::CityForecast(std::string city, int refresh_rate) : city_(city), refresh_rate_(refresh_rate) {
    json city_info;
    
    GetCityInfo(city_info, city);
    
    country_ = city_info[0]["country"];
    latitude_ = std::to_string(city_info[0]["latitude"].get<double>());
    longitude_ = std::to_string(city_info[0]["longitude"].get<double>());
}

CityForecast::~CityForecast() {}

void CityForecast::Forecast(int days) const {
    std::stringstream output;
    
    std::string current_weather_request = GetCurrentWeatherInfo();
    std::string forecast_request = GetForecastInfo(days);
    
    json current_weather = json::parse(current_weather_request);
    json forecast = json::parse(forecast_request);

    // Current
    const std::string* current_image;
    std::string* current_weather_info = FormatInfo(&current_weather["minutely_15"], current_image, 0);
    
    output << "\n" <<"Weather report: " << city_ << ", " << country_ << "\n\n";
    
    for (int i = 0; i < kNumberOfDataLines; ++i) {
        output << "  " << current_image[i] << "  " << current_weather_info[i] << "\n";
    }
    
    // Forecast
    const std::string* image;
    std::string* forecast_info;
    
    for (int i = 0; i < days; ++i) {
        std::stringstream s1, s2, s3, s4, s5;
        
        output << TableHeading(i);
        
        for (int j = 0; j < kPartsOfADay; ++j) {
            int index = (6 * j + 24 * i);

            forecast_info = FormatInfo(&forecast["hourly"], image, index);
            
            s1 << "│ " << image[0] << "  " << std::left << std::setw(16) << forecast_info[0];
            s2 << "│ " << image[1] << "  " << std::left << std::setw(17) << forecast_info[1];
            s3 << "│ " << image[2] << "  " << std::left << std::setw(18) << forecast_info[2];
            s4 << "│ " << image[3] << "  " << std::left << std::setw(16) << forecast_info[3];
            s5 << "│ " << image[4] << "  " << std::left << std::setw(16) << forecast_info[4];
            
            delete [] forecast_info;
        }
        
        s1 << "│\n";
        s2 << "│\n";
        s3 << "│\n";
        s4 << "│\n";
        s5 << "│\n";
        
        output << s1.str() << s2.str() << s3.str() << s4.str() << s5.str();
        output << TableBottom();
    }
    
    std::cout << output.str();
    
    delete [] current_weather_info;
}

void CityForecast::GetCityInfo(json& city_info, std::string& city) const {
    std::ifstream cache_file;
    std::string formatted_city = SpaceToPlus(city);
    
    cache_file.open(kPathToCachedJSONs + formatted_city + kJSONExtension);
    
    if (cache_file) {
        city_info = json::parse(cache_file);
        cache_file.close();
    } else {
        std::string ninja_url = kNinjaAPIURL + formatted_city;
        
        cpr::Response request = cpr::Get(cpr::Url{ninja_url}, cpr::Header{{"X-Api-Key", kNinjaAPIKey}});
        city_info = json::parse(request.text);
        
        std::ofstream for_cache;
        for_cache.open(kPathToCachedJSONs + formatted_city + kJSONExtension);
        for_cache << city_info;
        for_cache.close();
    }
}

std::string CityForecast::GetCurrentWeatherInfo() const {
    return cpr::Get(cpr::Url{kWeatherAPIURL},
           cpr::Parameters{
                {"latitude", latitude_},
                {"longitude", longitude_},
                {"timezone", "auto"},
                {"minutely_15", "temperature_2m"},
                {"minutely_15", "apparent_temperature"},
                {"minutely_15", "precipitation"},
                {"minutely_15", "weather_code"},
                {"minutely_15", "wind_speed_10m"},
                {"minutely_15", "wind_direction_10m"},
                {"minutely_15", "visibility"},
                {"forecast_minutely_15", "1"}
    }).text;
}

std::string CityForecast::GetForecastInfo(int& days) const {
    return cpr::Get(cpr::Url{kWeatherAPIURL},
           cpr::Parameters{
                {"latitude", latitude_},
                {"longitude", longitude_},
                {"timezone", "auto"},
                {"hourly", "temperature_2m"},
                {"hourly", "apparent_temperature"},
                {"hourly", "precipitation"},
                {"hourly", "weather_code"},
                {"hourly", "wind_speed_10m"},
                {"hourly", "wind_direction_10m"},
                {"hourly", "visibility"},
                {"forecast_days", std::to_string(days)}
    }).text;
}

std::string CityForecast::TableHeading(int& day_number) const {
        std::string date = DateFormatting(day_number);
        
        return "\
                                                       ┌─────────────┐\n\
┌──────────────────────────────┬───────────────────────┤ "+date+" ├───────────────────────┬──────────────────────────────┐\n\
│            Night             │            Morning    └──────┬──────┘      Noon             │            Evening           │\n\
├──────────────────────────────┼──────────────────────────────┼──────────────────────────────┼──────────────────────────────┤\n\
";
}

std::string CityForecast::TableBottom() const {
    return "└──────────────────────────────┴──────────────────────────────┴──────────────────────────────┴──────────────────────────────┘\n";
}

std::string* CityForecast::FormatInfo(json* current_weather, const std::string*& current_image, int index) const {
    std::string* array = new std::string[kNumberOfDataLines];

    array[0] = WeatherFormatting((*current_weather)["weather_code"][index], current_image);
    array[1] = TemperatureFormatting((*current_weather)["temperature_2m"][index], (*current_weather)["apparent_temperature"][index]);
    array[2] = WindFormatting((*current_weather)["wind_speed_10m"][index], (*current_weather)["wind_direction_10m"][index]);
    array[3] = VisibilityFormatting((*current_weather)["visibility"][index]);
    array[4] = PrecipitationFormatting((*current_weather)["precipitation"][index]);

    return array;
}

std::string CityForecast::WeatherFormatting(const int& weather_code, const std::string*& ascii_image) const {
    std::stringstream stream;

    if (weather_code == 0) {
        ascii_image = Images::sunny;
        stream << "Sunny";
    } else if (weather_code == 1 || weather_code == 2) {
        ascii_image = Images::partly_cloudy;
        stream << "Partly cloudy";
    } else if (weather_code == 3) {
        ascii_image = Images::cloudy;
        stream << "Cloudy";
    } else if (weather_code >= 40 && weather_code < 50) {
        ascii_image = Images::freezing_fog;
        stream << "Freezing fog";
    } else if ((weather_code >= 50 && weather_code < 60) || weather_code == 80) {
        ascii_image = Images::light_rain;
        stream << "Light rain";
    } else if ((weather_code >= 60 && weather_code < 70) || weather_code == 81 || weather_code == 82) {
        ascii_image = Images::heavy_rain;
        stream << "Heavy rain";
    } else if (weather_code >= 70 && weather_code < 80) {
        ascii_image = Images::snow;
        stream << "Snow";
    } else {
        ascii_image = Images::question;
        stream << "Unknown" << weather_code;
    }

    return stream.str();
}

std::string CityForecast::TemperatureFormatting(const int& temp, const int& apparent_temp) const {
    std::stringstream stream;
    
    stream << temp << "(" << apparent_temp << ") °C";
    
    return stream.str();
}

std::string CityForecast::WindFormatting(const double& wind_speed, const double& wind_direction) const {
    std::stringstream stream;

    if (wind_direction >= 0 && wind_direction < 45)         { stream << "↑ "; }
    else if (wind_direction >= 45 && wind_direction < 90)   { stream << "↗ "; }
    else if (wind_direction >= 90 && wind_direction < 135)  { stream << "→ "; }
    else if (wind_direction >= 135 && wind_direction < 180) { stream << "↘ "; }
    else if (wind_direction >= 180 && wind_direction < 225) { stream << "↓ "; }
    else if (wind_direction >= 225 && wind_direction < 270) { stream << "↙ "; }
    else if (wind_direction >= 270 && wind_direction < 315) { stream << "← "; }
    else if (wind_direction >= 315 && wind_direction <= 360) { stream << "↖ "; }
    
    stream << wind_speed << " km/h";
    
    return stream.str();
}

std::string CityForecast::VisibilityFormatting(const double& visibility) const {
    std::stringstream stream;
    
    stream << visibility << " m";
    
    return stream.str();
}

std::string CityForecast::PrecipitationFormatting(const double& precipitation) const {
    std::stringstream stream;
    
    stream << precipitation << " mm";
    
    return stream.str();
}
