#include <YunClient.h>
#include <YunServer.h>

int redPin = 9;   // Red LED,   connected to digital pin 9
int grnPin = 10;  // Green LED, connected to digital pin 10
int bluPin = 11;  // Blue LED,  connected to digital pin 11

int iRed = 0;
int iGreen = 0;
int iBlue = 0;

float fcurrentRed = 0;
float fcurrentGreen = 0;
float fcurrentBlue = 0;

float fstepRed = 0;
float fstepGreen = 0;
float fstepBlue = 0;

int incoming = 0;
int timeout = 1;
int initialTimer = 0;

String request = "";
bool test_led_status = false;
String response = "";

YunServer server;

void setup() {
  Serial.begin(115200);
  Serial.println("Opstarten ...");

  // output pins zetten
  pinMode(redPin, OUTPUT);
  pinMode(grnPin, OUTPUT);   
  pinMode(bluPin, OUTPUT);  
  
  cycleColors(150);
  // Initialize Yun bridge library.
  Bridge.begin();
  
  // Connect the server library to the Linio OS and start listening for connections.
  server.listenOnLocalhost();
  server.begin();
  
  cycleColors(250);
}

void loop() {
  // Handle any HTTP connections.
  YunClient client = server.accept();

  // iemand verbonden?
  if (client) {    
    Serial.println("client!");
    // nakijken dat commando met led begint
    String command = client.readStringUntil('/');
    command.trim();
    
    response = command + "\n";
    
    if (command == "led") {
      // fade
      String command = client.readStringUntil('/');
      command.trim();
      response = response + command + "\n";
      timeout = command.toInt();

      // kleuren
      command = client.readStringUntil('/');
      response = response + command + "\n";
      iRed = command.toInt();
      command = client.readStringUntil('/');
      response = response + command + "\n";
      iGreen = command.toInt();
      command = client.readStringUntil('/');
      response = response + command + "\n";
      iBlue = command.toInt();
      
      setColorSteps();
    }
    
    // response sturen
    client.println("Status: 200");
    client.println("Content-type: application/json");
    client.println();
    client.print("{\"status\":\"ok\"}");
    client.stop();
  }

  nextStep();
}

float getColorstep(float currentColor, int tobeColor) {
  if (currentColor == tobeColor) {
    return 0.00;
  } else {
    int diff = tobeColor - currentColor;
    float colorStep = (float)diff / (float)timeout;
    return colorStep;
  }    
}

void setColorSteps() {
  fstepRed = getColorstep(fcurrentRed, iRed);
  fstepGreen = getColorstep(fcurrentGreen, iGreen);
  fstepBlue = getColorstep(fcurrentBlue, iBlue);
  initialTimer = 0;
  Serial.print("stepRed: ");
  Serial.print(fstepRed);
  Serial.print(" stepGreen: ");
  Serial.print(fstepGreen);
  Serial.print(" stepBlue: ");
  Serial.println(fstepBlue);
}

void nextStep() {
  if (initialTimer < timeout) {
    fcurrentRed = fcurrentRed + fstepRed;
    fcurrentGreen = fcurrentGreen + fstepGreen;
    fcurrentBlue = fcurrentBlue + fstepBlue;

    setColor(fcurrentRed, fcurrentGreen, fcurrentBlue);
    initialTimer = initialTimer + 1; 
  }
}

void setColor(float red, float green, float blue) {
  analogWrite(redPin, (int)red);
  analogWrite(grnPin, (int)green);      
  analogWrite(bluPin, (int)blue);
  Serial.print(" red:");
  Serial.print(fcurrentRed);
  Serial.print(" green:");
  Serial.print(fcurrentGreen);
  Serial.print(" blue:");
  Serial.println(fcurrentBlue);
}

void cycleColors(int interval) {
  setColor(255, 0, 0);
  delay(interval);
  setColor(0, 255, 0);
  delay(interval);
  setColor(0, 0, 255);
  delay(interval);
  setColor(0, 0, 0);
  delay(interval);
}
