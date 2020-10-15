#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include "DHT.h"

/*****************************************************************************/
/* START - User defined values. Everything in this block must be configured
/* with your specific information in order to compile and work.
/*****************************************************************************/
// The hostname of the device. This is what it will report itself as on
// the network.
#define device_name "<device name>"

// The names of the humidity and temperature MQTT topics it will publish to
#define humidity_topic "sensor/<room name>/humidity"
#define temperature_topic "sensor/<room name>/temperature"

// WIFI & MQTT Setup
#define wifi_ssid "<your ssid>"
#define wifi_password "<your wpa2 password>"

#define mqtt_server ""
#define mqtt_user ""
#define mqtt_pass ""

// If enabled, the ultra low power consumption "deep sleep" mode of the
// ESP8266 will be used, and an update will be sent every 10 minutes.
// In practice, this works, but still drains a 9v battery in less than a
// day. Connecting to a continuous power source is advisable.
bool batteryMode = false;

// If enabled, the section below must be completed, and the device will be
// configured with the provided static network information.
// Otherwise, DHCP will be used for network configuration.
bool useStaticIp = false;
//Static IP address configuration
IPAddress staticIP(192, 168, 0, 2); //ESP static ip
IPAddress gateway(192, 168, 0, 1);   //IP Address of your WiFi Router (Gateway)
IPAddress subnet(255, 255, 255, 0);  //Subnet mask
IPAddress dns(8, 8, 8, 8);     //DNS
/*****************************************************************************/
/* END - User defined values. DO NOT CHANGE ANYTHING BELOW THIS LINE
/*****************************************************************************/

// DHT11 Setup
#define DHTPIN D1
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi()
{
    // We start by connecting to a WiFi network
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(wifi_ssid);

    if (useStaticIp) {
        WiFi.config(staticIP, gateway, subnet, dns);
    }

    WiFi.begin(wifi_ssid, wifi_password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void reconnect()
{
    // Loop until we're reconnected
    while (!client.connected())
    {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (client.connect(device_name))
        {
            Serial.println("connected");
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

void sendUpdate() {
    if (!client.connected()) {
        reconnect();
    }
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    float f = dht.readTemperature(true);

    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t) || isnan(f))
    {
        Serial.println(F("Failed to read from DHT sensor!"));
        return;
    }

    // Compute heat index in Fahrenheit (the default)
    float hif = dht.computeHeatIndex(f, h);
    // Compute heat index in Celsius (isFahreheit = false)
    float hic = dht.computeHeatIndex(t, h, false);

    Serial.print(F("Humidity: "));
    Serial.print(h);
    Serial.print(F("%  Temperature: "));
    Serial.print(t);
    Serial.print(F("°C "));
    Serial.print(f);
    Serial.print(F("°F  Heat index: "));
    Serial.print(hic);
    Serial.print(F("°C "));
    Serial.print(hif);
    Serial.println(F("°F"));
    client.publish(temperature_topic, String(f).c_str(), true);
    client.publish(humidity_topic, String(h).c_str(), true);
}

void setup()
{
    Serial.begin(115200);
    Serial.setTimeout(1000);
    while (!Serial) {}
    setup_wifi();
    client.setServer(mqtt_server, 1883);
    dht.begin();
}

void loop() {
    if (batteryMode)
    {
        sendUpdate();
        client.disconnect();
        ESP.deepSleep(600e6);
    }
    else {
        delay(2000);
        client.loop();
        sendUpdate();
    }
}