# DHT11 Temperature and Humidity IoT firmware
This is a platform.io based firmware for an ESP8266 (nodemcu) device using a DHT11 sensor, which will report temperature and humidity to an MQTT broker which uses no authentication, or username/password authentication.

The DHT11 sensor is a 1-wire sensor that is expected to be connected to `D1` (also known as `GPIO5`) of the ESP8266 (nodemcu) device.

I used this "in production" to monitor temps in a few different rooms of my house for months, until I realized that [tasmota](https://tasmota.github.io/docs/) is purpose built for this sort of thing, and works much more effectively. Look out for a custom template and details in a future repository.