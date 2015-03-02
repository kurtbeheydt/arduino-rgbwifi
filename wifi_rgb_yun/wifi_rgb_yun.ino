#include <YunClient.h>
#include <YunServer.h>

int redPin = 9;   // Red LED,   connected to digital pin 9
int grnPin = 10;  // Green LED, connected to digital pin 10
int bluPin = 11;  // Blue LED,  connected to digital pin 11

float iRed = 255;
float iGreen = 0;
float iBlue = 0;

float fcurrentRed = 255;
float fcurrentGreen = 0;
float fcurrentBlue = 0;

float fstepRed = 0;
float fstepGreen = 0;
float fstepBlue = 0;

int incoming = 0;
int timeout = 1;
int initialTimer = 0;
String effect = "breathe"; // full|breathe|distort
int sequencespeed = 30;
int BreatheDir = 0;
float marge = 200;

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
    client.setTimeout(5);
    
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
      command = client.readStringUntil('/');
      command.trim();
      response = response + command + "\n";
      if (command) {
        effect = command;
      } else {
        effect = "full";
      } 
      
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
  if (effect == "breathe") {
    breath();
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

void breath() {
 
  if (BreatheDir == 0) {
    if ((fcurrentRed > 0) || (fcurrentGreen > 0) || (fcurrentBlue > 0)) {
      fcurrentRed = ((fcurrentRed - 1) >= 0) ? (fcurrentRed - 1) : 0;
      fcurrentGreen = ((fcurrentGreen - 1) >= 0) ? (fcurrentGreen - 1) : 0;
      fcurrentBlue = ((fcurrentBlue - 1) >= 0) ? (fcurrentBlue - 1) : 0;
      setColor(fcurrentRed, fcurrentGreen, fcurrentBlue);
    } else {
      BreatheDir = 1;
    }
  } else {
    if ((fcurrentRed < iRed) || (fcurrentGreen < iGreen) || (fcurrentBlue < iBlue)) {
      fcurrentRed = (fcurrentRed < iRed) ? (fcurrentRed + 1) : iRed;
      fcurrentGreen = (fcurrentGreen < iGreen) ? (fcurrentGreen + 1) : iGreen;
      fcurrentBlue = (fcurrentBlue < iBlue) ? (fcurrentBlue + 1) : iBlue;
      setColor(fcurrentRed, fcurrentGreen, fcurrentBlue);
    } else {
      BreatheDir = 0;
    }
  }
  
  Serial.print("target rood: ");
  Serial.print(iRed);
  Serial.print("rood: ");
  Serial.print(fcurrentRed);
  Serial.print(" - groen: ");
  Serial.print(fcurrentGreen);
  Serial.print(" - blauw: ");
  Serial.println(fcurrentBlue);
  
  delay(sequencespeed);
}

