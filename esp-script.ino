#include <CustomJWT.h>
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include "DHT.h"
#include <WiFiClientSecure.h>

#define DHTTYPE DHT11
#define DHTPIN 27
DHT dht(DHTPIN,DHTTYPE);

//Cert for <api.scudoreloaded.it>
const char* rootCACertificate =
"-----BEGIN CERTIFICATE-----\n"
"MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw\n"
"TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh\n"
"cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4\n"
"WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu\n"
"ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY\n"
"MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc\n"
"h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+\n"
"0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U\n"
"A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW\n"
"T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH\n"
"B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC\n"
"B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv\n"
"KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn\n"
"OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn\n"
"jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw\n"
"qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI\n"
"rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV\n"
"HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq\n"
"hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL\n"
"ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ\n"
"3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK\n"
"NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5\n"
"ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur\n"
"TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC\n"
"jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc\n"
"oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq\n"
"4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA\n"
"mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d\n"
"emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=\n"
"-----END CERTIFICATE-----\n";


struct tm timeinfo;
long t0,t1;
char buff[256];
time_t now;



WiFiMulti WiFiMulti;

WiFiClientSecure* client;
HTTPClient https;

const char* API = "https://api.scudoreloaded.it/post/";

float prev_temperature = 20;
float prev_humidity = 55;

float temperature;
float humidity;
int light;
float mappedLight;



char *string = "{\"name\":\"Sala\"}"; //name of the board

long _time_var;
char _key_var[128] = "";
char _jwt_var[128] = "";

char token[256] = "";

char* key(long seed){
    sprintf(
        _key_var,
        "%s%ld%s",
        "d839552700ad004d4e968844eb85bcb5080214b5", //encoding header
        seed,
        "406a0d1827cfd86e73e71ad6801a12b6933e2bff" //encoding trailer
    );

    return _key_var;
}

char* jwt(char* key,char* payload){
    CustomJWT j(key, 256);
    j.allocateJWTMemory();
    j.encodeJWT(payload);
    strcpy(_jwt_var,j.out);
    j.clear();

    return _jwt_var;
}

void setClock() {
    Serial.print("Synchronizing clock...");
    configTime(7200, 0, "pool.ntp.org");

    while(!getLocalTime(&timeinfo)) {
        configTime(7200, 0, "pool.ntp.org");
        Serial.print(".");
        delay(10);
    }
    Serial.println();
    Serial.println(&timeinfo, "Synched! -> %A, %B %d %Y %H:%M:%S");
}




void setup() {
    
    Serial.begin(115200);
    Serial.println();
    Serial.println();
    Serial.println();
    Serial.println("-------------------------");

    // WiFi APs setup
    WiFi.mode(WIFI_STA);
    WiFiMulti.addAP("ESP32-AP", "aaaaaaaa");
    WiFiMulti.addAP("Dio Cancaro", "13,AlfaSierra");
    
    // connect to WiFi and print currest SSID
    Serial.print("Connecting to WiFi...");
    while ((WiFiMulti.run() != WL_CONNECTED)) {
        Serial.print(".");
    }
    Serial.println();
    Serial.printf("Connected to %s! \n",WiFi.SSID());

    pinMode(26,INPUT);

    // sync
    Serial.println("-------------------------");
    setClock();
    Serial.println("-------------------------");
}

void loop() {
    client = new WiFiClientSecure;
    if (client) {
        client->setCACert(rootCACertificate);
        //client->setInsecure();

        {
            Serial.print("[HTTPS] begin...\n");
                while (1) {
                    https.begin(API);
                    t0 = (long)millis();

                    temperature=dht.readTemperature();
                    humidity=dht.readHumidity();
                    
                    //DHT failure handling
                    if(isnan(temperature) || isnan(temperature)){
                        temperature=prev_temperature;
                        humidity=prev_humidity;
                    }
                    prev_temperature=temperature;
                    prev_humidity=humidity;

                    
                    /*

                    --Test data

                    sprintf(buff,"{" \
                        "\"temperature\": %f," \
                        "\"humidity\": %f," \
                        "\"light\": %d," \
                        "\"time\": %ld" \
                        "}", 20, 55, 100, time(&now));
                    sprintf(buff,"{" \
                        "\"temperature\": %f," \
                        "\"humidity\": %f," \
                        "\"light\": %d," \
                        "\"time\": %ld" \
                        "}", temperature, humidity, 100, time(&now));
                    */

                    --Real data
                    light=analogRead(26);
                    mappedLight=map(light,20,1005,0,100);
                    sprintf(buff,"{" \
                        "\"temperature\": %f," \
                        "\"humidity\": %f," \
                        "\"light\": %d," \
                        "\"time\": %ld" \
                        "}", temperature, humidity, mappedLight, time(&now));


                    sprintf(token,"Bearer %s", jwt(key(now),string));
                    https.addHeader("Content-Type", "application/json");
                    https.addHeader("Authorization",token);
                    int httpCode = https.POST(buff);
                    getLocalTime(&timeinfo);
                    Serial.print(&timeinfo, "%A, %B %d %Y %H:%M:%S -> ");

                    if (httpCode > 0) {     //<0 if error
                        
                        Serial.printf("[HTTPS] GET... code: %d\n", httpCode);

                        // file found at server
                        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
                            delay(1);
                            https.getString();
                            //Serial.printf("Temperature %f Humidity: %f \n",dht.readTemperature(),dht.readHumidity());
                        }
                    } else {
                        Serial.printf("[HTTPS] GET... failed, error: %s %d\n", https.errorToString(httpCode).c_str(), httpCode);
                        break;
                    }
                    https.end();
                    t1=(long)millis();
                    delay(5000);

                }
        }

        delete client;
    } else {
        Serial.println("Unable to create client");
    }

    Serial.println();
    Serial.println("Waiting 10s before the next round...");
    delay(10000);
}



