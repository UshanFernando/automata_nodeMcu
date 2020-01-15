#include <ESP8266WiFi.h>
#include <Arduino.h>

String incoming = "";
const char *ssid = "Ghost-Server";
const char *password = "ushan&627628";
int status_arr[4] = {0, 0, 0, 0};
int ledPin = LED_BUILTIN; // GPIO13---D7 of NodeMCU
WiFiServer server(80);

String dataRV;

// Set array size to maximum number of integers you will receive.
uint16_t myArray[2];

void setup()
{
  Serial.begin(115200);
  //  SUART.begin(115200); //enable SUART Port
  delay(10);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("connected");

  // Start the server
  server.begin();
  Serial.println("server_started");

  // Print the IP address
  Serial.print("IP ");
  //  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
}

void readData(){

  if (Serial.available() > 0)
  {
    dataRV = Serial.readString();
    dataRV.trim();
    if (dataRV.indexOf("L1_SW") >= 0)
    {
      if (status_arr[0] == 0)
      {
        status_arr[0] = 1;
        Serial.println("L1_ON");
      }
      else
      {
        status_arr[0] = 0;
        Serial.println("L1_OFF");
      }
    }

    char charBuf[50];
    dataRV.toCharArray(charBuf, 50);

    if (charBuf[0] == '>')
    {
      int index = 0; //index to cull
      memmove(&charBuf[index], &charBuf[index + 1], strlen(charBuf) - index);
      int counter = 0;
      char *pch = strtok(charBuf, "\",");

      while (pch != NULL)
      {
        myArray[counter] = atoi(pch);
        pch = strtok(NULL, "\"+");
        counter += 1;
      }
    }
  }
}

void loop()
{

  readData();

  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client)
  {
    return;
  }

  // Wait until the client sends some data
  //  Serial.println("new client");
  while (!client.available())
  {
    delay(1);
  }

  // Read the first line of the request
  String request = client.readStringUntil('\r');
  //  Serial.println(request);
  client.flush();

  // Match the request

  if (request.indexOf("/L1=ON") != -1)
  {
    Serial.println("L1_ON");
    status_arr[0] = 1;
    digitalWrite(ledPin, LOW);
  }
  if (request.indexOf("/L1=OFF") != -1)
  {
    Serial.println("L1_OFF");
    status_arr[0] = 0;
    digitalWrite(ledPin, HIGH);
  }
  if (request.indexOf("/L2=ON") != -1)
  {
    Serial.println("L2_ON");
    status_arr[1] = 1;
  }
  if (request.indexOf("/L2=OFF") != -1)
  {
    Serial.println("L2_OFF");
    status_arr[1] = 0;
  }
  if (request.indexOf("/L3=ON") != -1)
  {
    Serial.println("L3_ON");
    status_arr[2] = 1;
  }
  if (request.indexOf("/L3=OFF") != -1)
  {
    Serial.println("L3_OFF");
    status_arr[2] = 0;
  }
  if (request.indexOf("/L4=ON") != -1)
  {
    Serial.println("L4_ON");
    status_arr[3] = 1;
  }
  if (request.indexOf("/L4=OFF") != -1)
  {
    Serial.println("L4_OFF");
    status_arr[3] = 0;
  }

  // Set ledPin according to the request
  //digitalWrite(ledPin, value);

  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");

  for (int i = 1; i < 5; i++)
  {
    client.print("L");
    client.print(i);
    client.print(" is now ");
    if (status_arr[i - 1] == 1)
    {
      client.print("On");
    }
    else
    {
      client.print("Off");
    }
    client.println("<br>");
  }

  client.println("<br><br>");
  client.println("<a href=\"/L1=ON\"\"><button>L1 On </button></a>");
  client.println("<a href=\"/L1=OFF\"\"><button>L1 Off </button></a><br />");
  client.println("<a href=\"/L2=ON\"\"><button>L2 On </button></a>");
  client.println("<a href=\"/L2=OFF\"\"><button>L2 Off </button></a><br />");
  client.println("<a href=\"/L3=ON\"\"><button>L3 On </button></a>");
  client.println("<a href=\"/L3=OFF\"\"><button>L3 Off </button></a><br />");
  client.println("<a href=\"/L4=ON\"\"><button>L4 On </button></a>");
  client.println("<a href=\"/L4=OFF\"\"><button>L4 Off </button></a><br />");
  client.println("<P> Temp : ");
  client.print(myArray[0]);
  client.println("<P>");
  client.println("<P> Huminidy : ");
  client.print(myArray[1]);
  client.println("<P>");
  client.println("</html>");

  delay(1);
  // Serial.println("Client disonnected");
  //Serial.println("");
}