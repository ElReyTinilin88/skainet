#include <Adafruit_Fingerprint.h>
#include <WiFi.h>

const char* ssid = "Totalplay-_2.4G";
const char* password = "jtkWdupDW3T8xC3j";
// Definir los pines RX y TX para la comunicación con el sensor
#define RX_PIN 16  // Pin RX del ESP32 (conectado a TX del AS608)
#define TX_PIN 17  // Pin TX del ESP32 (conectado a RX del AS608)
WiFiServer server(80);
HardwareSerial mySerial(1);  // puerto serial 1

String header;
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

uint8_t nextID = 1;  // ID inicial, el siguiente ID se incrementará para cada huella registrada

void setup() {
  Serial.begin(115200);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();


  Serial.println("\n\nAdafruit Fingerprint sensor");

  // Inicializamos Serial1 (puerto serie hardware) con los pines especificados
  mySerial.begin(57600, SERIAL_8N1, RX_PIN, TX_PIN);

  // Iniciamos el sensor de huellas
  if (finger.verifyPassword()) {
    Serial.println("¡Sensor de huellas encontrado!");
  } else {
    Serial.println("No se encontró el sensor de huellas :(");
    while (1) { delay(1); }
  }

  // Verificamos los parámetros del sensor
  Serial.println(F("Leyendo parámetros del sensor"));
  finger.getParameters();
  Serial.print(F("Status: 0x"));
  Serial.println(finger.status_reg, HEX);
  Serial.print(F("Sys ID: 0x"));
  Serial.println(finger.system_id, HEX);
  Serial.print(F("Capacity: "));
  Serial.println(finger.capacity);
  Serial.print(F("Security level: "));
  Serial.println(finger.security_level);
  Serial.print(F("Device address: "));
  Serial.println(finger.device_addr, HEX);
  Serial.print(F("Packet len: "));
  Serial.println(finger.packet_len);
  Serial.print(F("Baud rate: "));
  Serial.println(finger.baud_rate);
}

uint8_t readnumber(void) {
  uint8_t num = 0;
  while (num == 0) {
    while (!Serial.available())
      ;
    num = Serial.parseInt();
  }
  return num;
}

void loop() {
  WiFiClient client = server.available();  // Listen for incoming clients

  if (client) {  // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");                                             // print a message out in the serial port
    String currentLine = "";                                                   // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {  // if there's bytes to read from the client,
        char c = client.read();  // read a byte, then
        Serial.write(c);         // print it out the serial monitor
        header += c;
        if (c == '\n') {  // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            if (header.indexOf("GET /register") >= 0) {  // Verificar si la ruta es "/register"
              Serial.println("Petición recibida para /register");

              int id = registerFingerprint();

              if (id >= 0) {  // Si se ha registrado correctamente
                              // Enviar respuesta HTTP con el ID registrado
                client.println("HTTP/1.1 200 OK");
                client.println("Content-Type: application/json");                             // Respuesta JSON (mejor que HTML para datos)
                client.println("Access-Control-Allow-Origin: *");                             // Encabezado CORS
                client.println("Access-Control-Allow-Methods: GET, POST, PUT, DELETE");       // Métodos permitidos
                client.println("Access-Control-Allow-Headers: Content-Type, Authorization");  // Encabezados permitidos
                client.println("Connection: close");
                client.println();
                client.print("{\"status\":\"success\", \"message\":\"Huella registrada\", \"id\":");
                client.print(id);  // Devolver el ID de la huella registrada
                client.println("}");
              } else {
                // Si hubo un error al registrar la huella
                client.println("HTTP/1.1 500 Internal Server Error");
                client.println("Content-Type: application/json");
                client.println("Connection: close");
                client.println();
                client.println("{\"status\":\"error\", \"message\":\"Error al registrar la huella\"}");
              }
            } else {
              // Si no es /register, responder con un error 404
              client.println("HTTP/1.1 404 Not Found");
              client.println("Content-Type: text/html");
              client.println("Connection: close");
              client.println();
              client.println("<!DOCTYPE HTML><html><body>");
              client.println("<h1>404 No Encontrado</h1>");
              client.println("</body></html>");
            }
            break;
          } else {  // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}

uint8_t registerFingerprint() {
  Serial.println("¡Por favor, coloque su dedo para registrar la huella!");

  int p = -1;

  // Esperar hasta que se coloque un dedo
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    if (p == FINGERPRINT_NOFINGER) {
      Serial.print(".");  // Esperando el dedo
      delay(100);         // Reducir la velocidad de espera para evitar saturar el serial
    }
  }

  Serial.println("Dedo detectado.");

  // Convertir la imagen del dedo en plantilla
  p = finger.image2Tz(1);
  if (p != FINGERPRINT_OK) {
    Serial.println("Error al convertir la imagen en plantilla");
    return p;
  }

  // Retiramos el dedo
  Serial.println("Retire el dedo.");
  delay(2000);

  // Esperar hasta que se retire el dedo
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }

  Serial.println("Coloque el mismo dedo nuevamente");

  // Esperar hasta que se coloque nuevamente el dedo
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    if (p == FINGERPRINT_NOFINGER) {
      Serial.print(".");  // Esperando el dedo
      delay(100);         // Reducir la velocidad de espera para evitar saturar el serial
    }
  }

  // Convertir la segunda imagen en plantilla
  p = finger.image2Tz(2);
  if (p != FINGERPRINT_OK) {
    Serial.println("Error al convertir la segunda imagen en plantilla");
    return p;
  }

  // Crear el modelo
  p = finger.createModel();
  if (p != FINGERPRINT_OK) {
    Serial.println("Error al crear el modelo");
    return p;
  }

  // Guardar el modelo en la base de datos
  p = finger.storeModel(nextID);
  if (p == FINGERPRINT_OK) {
    Serial.print("Huella registrada correctamente con el ID: ");
    Serial.println(nextID);

    // Incrementar el siguiente ID para la siguiente huella
    nextID++;
    if (nextID > 127) {
      nextID = 1;  // Si llega al máximo de 127, reinicia el contador a 1
    }
  } else {
    Serial.println("Error al almacenar la huella.");
  }

  return --nextID;
}

void deleteAllFingerprints() {
  Serial.println("Borrando todas las huellas almacenadas...");
  for (uint8_t i = 1; i <= 127; i++) {
    int p = finger.deleteModel(i);
    if (p == FINGERPRINT_OK) {
      Serial.print("Huella con ID ");
      Serial.print(i);
      Serial.println(" borrada.");
    } else {
      Serial.print("Error al borrar la huella con ID ");
      Serial.println(i);
    }
  }
  Serial.println("Proceso de borrado completo.");
}

void viewStoredFingerprints() {
  Serial.println("Mostrando huellas almacenadas...");
  uint8_t storedFingerprintCount = 0;

  // Verificar cada ID desde 1 hasta 127
  for (uint8_t i = 1; i <= 127; i++) {
    int p = finger.loadModel(i);  // Cargar el modelo para verificar si existe
    if (p == FINGERPRINT_OK) {
      Serial.print("ID: ");
      Serial.println(i);
      storedFingerprintCount++;
    }
  }

  if (storedFingerprintCount == 0) {
    Serial.println("No hay huellas almacenadas.");
  } else {
    Serial.print("Total de huellas almacenadas: ");
    Serial.println(storedFingerprintCount);
  }
}

void readFingerprint() {
  Serial.println("¡Por favor, coloque su dedo para leerlo!");

  int p = -1;

  // Esperar hasta que se coloque un dedo en el sensor
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    if (p == FINGERPRINT_NOFINGER) {
      Serial.print(".");  // Esto indicará que el sistema está esperando
      delay(100);         // Reducir la velocidad de la espera para evitar saturar el serial
    }
  }

  Serial.println("Dedo detectado.");

  // Convertir la imagen del dedo en plantilla
  p = finger.image2Tz(1);
  if (p != FINGERPRINT_OK) {
    Serial.println("Error al convertir la imagen en plantilla");
    return;
  }


  for (uint8_t i = 1; i <= 127; i++) {

    p = finger.loadModel(i);
    if (p == FINGERPRINT_OK) {
      Serial.print("Comparando con el ID: ");
      Serial.println(i);

      p = finger.fingerFastSearch();
      if (p == FINGERPRINT_OK) {
        Serial.print("¡Huella encontrada! ID: ");
        Serial.println(i);
        return;
      } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
        Serial.println("Error en la comunicación");
      } else if (p == FINGERPRINT_ENROLLMISMATCH) {
        Serial.println("Las huellas no coinciden");
      }
    } else {
      Serial.println(i);
    }
  }

  Serial.println("No se encontraron coincidencias para la huella.");
  Serial.println("Puede proceder a guardarla si lo desea.");
}