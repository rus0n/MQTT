#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>


//**************************************
//*********** MQTT CONFIG **************
//**************************************
#define SERVER "things.ubidots.com"
#define DEVICE_ID "628a1c7486d7ca24ece66b2c"
#define TOKEN "BBFF-knhZG1ghExR1iinDAzfyQENHOHmZyI"
#define VARIABLE_TEM "temperatura"
#define VARIABLE_HUM "humedad"
#define VARIABLE_LUZ "luz"
#define TEMA "/v1.6/devices/esp32"
#define PORT 1883

//**************************************
//*********** WIFICONFIG ***************
//**************************************
const char* ssid = "MiFibra-C74E";
const char* password =  "xxxxxxx";



//**************************************
//*********** GLOBALES   ***************
//**************************************
WiFiClient espClient;
PubSubClient client(espClient);

#define DHTPIN 4
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

const int pinLed = 23;

char payload[250];

//************************
//** F U N C I O N E S ***
//************************
void reconnect();
void setup_wifi();

void callback(char* topic, byte* payload, unsigned int length);

void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(pinLed, OUTPUT);
  setup_wifi();
  client.setServer(SERVER, PORT);
  client.setCallback(callback);
}

void loop() {
  
  if (!client.connected()) {
		reconnect();
	}



  if (client.connected()){
    char topicSubscribe[100];
    sprintf(topicSubscribe, "%s/%s", TEMA, VARIABLE_LUZ);

    // if(client.subscribe(topicSubscribe)){
    //     //Serial.println("Suscripcion ok");
      
    //   }else{
    //     Serial.println("fallo Suscripciión");
    //   }

    float humedad = dht.readHumidity();
    float temperatura = dht.readTemperature();

    sprintf(payload,  "{\"%s\":{\"value\":%02.02f, \"context\":{\"temperatura\":%02.02f, \"humedad\":%02.02f}}}", VARIABLE_TEM, temperatura, temperatura, humedad);  
    // Serial.println(payload);

    client.publish(TEMA,payload);

    sprintf(payload,  "{\"%s\":{\"value\":%02.01f, \"context\":{\"temperatura\":%02.01f, \"humedad\":%02.01f}}}", VARIABLE_HUM, humedad, temperatura, humedad);  
    
    client.publish(TEMA,payload);

    delay(500);
  }
  client.loop();
}




//*****************************
//***    CONEXION WIFI      ***
//*****************************
void setup_wifi(){
	delay(10);
	// Nos conectamos a nuestra red Wifi
	Serial.println();
	Serial.print("Conectando a ssid: ");
	Serial.println(ssid);

	WiFi.begin(ssid, password);

	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}

	Serial.println("");
	Serial.println("Conectado a red WiFi!");
	Serial.println("Dirección IP: ");
	Serial.println(WiFi.localIP());
}



//*****************************
//***    CONEXION MQTT      ***
//*****************************

void reconnect() {

	while (!client.connected()) {
		Serial.print("Intentando conexión Mqtt...");
		// Intentamos conectar
		if (client.connect(DEVICE_ID,TOKEN, "")) {
			Serial.println("Conectado!");
			// Nos suscribimos
			// if(client.subscribe(root_topic_subscribe)){
      //   Serial.println("Suscripcion ok");
      // }else{
      //   Serial.println("fallo Suscripciión");
      // }
		} else {
			Serial.print("falló :( con error -> ");
			Serial.print(client.state());
			Serial.println(" Intentamos de nuevo en 5 segundos");
			delay(5000);
		}
	}
}


//*****************************
//***       CALLBACK        ***
//*****************************

void callback(char* topic, byte* payload, unsigned int length){
	String incoming = "";
	// Serial.print("Mensaje recibido desde -> ");
	// Serial.print(topic);
	// Serial.println("");

	for (int i = 0; i < length; i++) {
		incoming += (char)payload[i];
	}
	incoming.trim();
	
  //Serial.println("Mensaje -> " + incoming);

  String value = incoming.substring(10,11);

  Serial.println(value);

  if(value == "0"){
    digitalWrite(pinLed, LOW);
  } else if(value == "1"){
    digitalWrite(pinLed, HIGH);
  }

}