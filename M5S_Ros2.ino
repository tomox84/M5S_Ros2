#include <M5Stack.h>

#include <ros2arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>

// Wi-Fi
const char* ssid = "Buffalo-G-0188";
const char* pswd = "ipf4i4xypksdt";
boolean isConnected=false;

// Ros2
#define AGENT_IP   "192.168.11.21"
#define AGENT_PORT 2018 //AGENT port number
#define PUBLISH_FREQUENCY 2 //hz
void publishString(std_msgs::String* msg, void* arg)
{
  (void)(arg);

  static int cnt = 0;
  sprintf(msg->data, "Hello ros2arduino %d", cnt++);
}

class StringPub : public ros2::Node
{
public:
  StringPub()
  : Node("ros2arduino_pub_node")
  {
    ros2::Publisher<std_msgs::String>* publisher_ = this->createPublisher<std_msgs::String>("arduino_chatter");
    this->createWallFreq(PUBLISH_FREQUENCY, (ros2::CallbackFunc)publishString, nullptr, publisher_);
  }
};


WiFiUDP udp;

void setup() {
  M5.begin();

  M5.Lcd.fillScreen(BLUE);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.println("Hello M5S_ROS2");
  M5.Lcd.println("");
  
  // Connect Wi-Fi
  M5.Lcd.print("Connecting Wi-Fi.");
  WiFi.begin(ssid, pswd);
  for( int cnt=0; cnt<20; cnt++) {
    isConnected = (WiFi.status() == WL_CONNECTED);
    if(isConnected) break;
    M5.Lcd.print(".");
    delay(500);
  }
  M5.Lcd.println("");
  M5.Lcd.println(isConnected ? "Wi-Fi Connected":"Not Connected");
  M5.Lcd.print("IP Address ");
  M5.Lcd.println(WiFi.localIP());
  M5.Lcd.println("");
  
  // ボタンの何かを押すまで待機
  M5.Lcd.fillRect(0, 224, 320, 16, WHITE);
  M5.Lcd.setTextColor(RED);
  M5.Lcd.setCursor(28 + 1, 224 + 1);
  M5.Lcd.print("-- Press Any Buttom --");
  while ((M5.BtnA.wasPressed() == false) && (M5.BtnB.wasPressed() == false) && (M5.BtnC.wasPressed() == false)) {
    M5.update();
    delay(50);
  }

  //ROS2の初期化
  ros2::init(&udp, AGENT_IP, AGENT_PORT);


  // ディスプレイの後処理
  M5.Lcd.fillScreen(BLUE);
  M5.Lcd.setTextColor(WHITE, BLUE);

  M5.update();
}



void loop() {
  M5.Lcd.setCursor(0, 0);
  M5.Lcd.println("at " + String(millis()));

  if (M5.BtnA.wasPressed()) {
    Serial.println("Button A Pressed");
    M5.Lcd.fillScreen(RED);
  }
  if (M5.BtnB.wasPressed()) {
    Serial.println("Button B Pressed");
    M5.Lcd.fillScreen(YELLOW);
  }

  static StringPub StringNode;

  ros2::spin(&StringNode);
  
  M5.update();
  delay(50);
}
