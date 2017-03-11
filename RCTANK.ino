#include <SPI.h>
#include <Ethernet.h>

#define echoPin2 3 // Echo Pin
#define trigPin2 4 // Trigger Pin
#define echoPin 5 // Echo Pin
#define trigPin 6 // Trigger Pin
#define Pin 9
#define Pin 7
#define Pin 8

 
int maximumRange2 = 11;
int maximumRange = 55;    
int minimumRange = 0;  
long duration, distance; // Duration used to calculate distance
long xduration2, xdistance2; // Duration used to calculate distance
int Rforward = 7; // right forward pin
int Lforward = 8; // Left forward pin
int Reverse = 9; // Reverse pin


boolean running = false;

byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
}; //physical mac address
byte ip[] = {
  192, 168, 1, 179
}; // ip in lan
byte gateway[] = {
  192, 168, 1, 1
}; // internet access via router
byte subnet[] = {
  255, 255, 255, 0
}; //subnet mask
EthernetServer server(777); //server port

String readString;




void setup()
{

  pinMode(AutoToggle, OUTPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(Lforward, OUTPUT);
  pinMode(Rforward, OUTPUT);
  pinMode(Reverse, OUTPUT);
  digitalWrite(AutoToggle, HIGH);


  Ethernet.begin(mac, ip, gateway, subnet);    //start Ethernet
  server.begin();
  Serial.begin(9600, SERIAL_8N1 );                          //enable serial data print
  Serial.println("Initialisation complete");   //check that setup has completed (in serial monitor)
}

void go_right()
{ digitalWrite(Rforward, HIGH);
  delay(400);
  digitalWrite(Rforward, LOW);
  loop();
}

void go_rvrs()
{
  digitalWrite(Reverse, HIGH); // turn reverse motor on
  delay(500);
  digitalWrite(Reverse, LOW); // turn reverse motor off

}

void go_forward()
{
  Serial.println("moving forward");
  digitalWrite(Lforward, HIGH);  // turn left forward motor on
delay(400);
  digitalWrite(Rforward, HIGH);   // turn right forward motor on

  digitalWrite(Lforward, LOW);  // turn left forward motor off
  digitalWrite(Rforward, LOW);  // turn right forward motor off

  loop();
}


void go_left()
{ digitalWrite(Lforward, HIGH);
  delay(400);
  digitalWrite(Lforward, LOW);
  loop();
}

 
void scan()
{

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);

  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);

  //Calculate the distance (in cm) based on the speed of sound.
  distance = duration / 58.2;
  Serial.println("");
  Serial.print("Distance1: ");
  Serial.print(distance);
  Serial.println("-----------------------------------");

  digitalWrite(trigPin2, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(10);

  digitalWrite(trigPin2, LOW);
  xduration2 = pulseIn(echoPin2, HIGH);

  //Calculate the distance (in cm) based on the speed of sound.
  xdistance2 = xduration2 / 58.2;

  Serial.print("xDistance2: ");
  Serial.print(xdistance2);
  Serial.println("-----------------------------------");

  if ((xdistance2 < minimumRange2) || ((distance < maximumRange) && (distance > minimumRange))) {
    /* Send a positive number to computer and Turn MOTORS ON
      to indicate "out of range" */
    Serial.println("stop");
    stop_car();
    delay(2);
    go_rvrs();
    delay(250);
    go_right();
    delay(50);
  }
}

void stop_car()
{
  digitalWrite(Rforward, LOW); // turn forward motor off
  digitalWrite(Lforward, LOW); // turn forward motor off
  if ((xdistance2 > maximumRange2) || ((distance < maximumRange) && (distance > minimumRange))) {
    /* Send a positive number to computer and Turn MOTORS ON
      to indicate "out of range" */
    Serial.println("stop");
    
    delay(2);
    go_rvrs();
    delay(250);
    go_right();
    delay(50);
  }
}

void loop() {
  scan();
  // Create a client connection
  EthernetClient client = server.available();
  if (client) {
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();

        if (readString.length() < 100)         //read char by char HTTP request
        {
          readString += c;                      //store characters to string
        }

        //if HTTP request has ended
        if (c == '\n') {

          //Webpage code (buttons)
          Serial.println(readString); //print to serial monitor for debuging
          
          client.println("HTTP/1.1 200 OK"); //send new page
          
          client.println("Content-Type: text/html");
          client.println();
          client.println("<!DOCTYPE html>");
          client.println("<HTML>");
          client.println("<HEAD>");

          client.println("<TITLE>Arduino Controller</TITLE>");
          client.println("<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.1.1/jquery.min.js'></script>");
          client.println(" <script type='text/javascript' src='http://www.openjs.com/scripts/events/keyboard_shortcuts/shortcut.js'></script>"); //here we start the javascript so the drone can be controlled via keyboard
          client.println("<script>");
          client.println(" shortcut.add('Down', function() {");
          client.println("location.href = 'http://74.50.151.107:777/?buttondown';");
          client.println(" });") ;
          client.println(" shortcut.add('Up', function() {");
          client.println("$.get(\"/?buttonup\");");
          client.println(" });") ;
          client.println(" shortcut.add('Right', function() {");
          client.println("location.href = 'http://74.50.151.107:777/?buttonright';");
          client.println(" });") ;
          client.println(" shortcut.add('Left', function() {");
          client.println("location.href = 'http://74.50.151.107:777/?buttonleft';");
          client.println(" });") ;

          client.println("</script>") ;
          client.println("</HEAD>");
          client.println("<BODY style='background-color:black'>");

         client.println("<center>");
          client.println("<div style='height:600px; width:740px'>");
          
client.println("<embed type='application/x-vlc-plugin'");
   client.println("target='rtsp://192.168.1.151:554/user=admin&password=&channel=1&stream=1.sdp?'");
   client.println("id='vlcPlayer'");
   client.println("name='vlcPlayer'");
   client.print("class='vlcPlayer'");
  
   client.println("autoplay='yes'>");

          client.println("</div>");
          client.println("<br>");
          client.println(" <a href=\"/?buttonup\"\"><button style='font-size:200%; '>FORWARD</button></a>");
          client.println("<br />");
          client.println(" <button onclick='$.get(\"/?buttonleft\")' style='font-size:200%; '>Left</button>");


          client.println("<a  href=\"/?buttonright\"\"><button style='font-size:200%; '>Right</button></a>");
          client.println("<br />");
          client.println("<a href=\"/?buttondown\"\"><button style='font-size:200%; '>BCK</button></a><br><br><br>");
          client.println("<a href=\"/?autoon\"\"><button style='font-size:200%; '>AUTODRIVE ON</button></a><br>");
          client.println("<a href=\"/?autooff\"\"><button style='font-size:200%; '>AUTODRIVE OFF</button></a><br>");


          client.println("</body>");
          client.println("</html>");
          delay(1);
          //stopping client
          client.stop();


          if (readString.indexOf("?buttonleft") > 0)       //scans HTML page for left button press
        {

          go_left();
            readString = "";
          }


          if ((readString.indexOf("?buttonup") > 0) && (xdistance2 < maximumRange2) && (distance > maximumRange || distance < minimumRange)) {

            Serial.println("moving forward");
            digitalWrite(Rforward, HIGH); // 
         
            digitalWrite(Lforward, HIGH);
        
            delay(500);
            digitalWrite(Rforward, LOW);

            digitalWrite(Lforward, LOW); 

            readString = "";               
          }




          if (readString.indexOf("?buttonright") > 0)
        {
          go_right();

            readString = "";
          }




          if (readString.indexOf("?buttondown") > 0)
        {
          go_rvrs();
            readString = "";                               // clear string for next check
          }
    
          else
          {


            readString = "";  // clear string for next check
          }



        }
      }
    }
  }
}



