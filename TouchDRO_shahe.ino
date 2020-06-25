// ###### Touch DRO adapter for Shahe 540x Scales #############
// Note:Scales connected through transistor switches, to bring signal level to 5v
//      this also inverts the signal
// copied from https://forum.yuriystoys.com/attachment.php?aid=215

//clock and data pins for each axis
#define XDATpin 4
#define XCLKpin 5
#define YDATpin 6
#define YCLKpin 7
#define ZDATpin 8
#define ZCLKpin 9

//Serial BaudRate - set to match bluetooth module setting
#define BaudRate 38400

const long CLKtimeout = 1000;     //timeout to flag new data packet

class axis {
  private:
    char label;                  //label for data output
    byte CLKpin;                 //Clock pin
    byte DATpin;                 //Data pin
    boolean CLKcurrent = HIGH;   //Current state of CLK pin
    boolean CLKprev = HIGH;      //Previous state of CLK pin
    unsigned long CLKtime = 0;   //timestamp of last CLK pulse
    int DATbit = 0;              //current data bit 
    long value = 0;              //buffer for value
   
    void DataOut(){
      Serial.print(label);       // Write out the value    
      Serial.print(value);
      Serial.print(';');
    }
    
  public:
    axis(char label, byte CLKpin, byte DATpin){
      this->label = label;
      this->CLKpin = CLKpin;
      this->DATpin = DATpin;
      pinMode(CLKpin,INPUT);
      pinMode(DATpin,INPUT);
    }
    
    void check(){                                                            // note: HIGH & LOW may need to be swapped for direct connection                                                  
      CLKprev = CLKcurrent;                                                  
      CLKcurrent = digitalRead(CLKpin);                               
      if ((CLKcurrent == LOW)&&(CLKprev == HIGH)) {                          // If CLK changed from HIGH to LOW
        if (micros() - CLKtime > CLKtimeout ) {                              //   If time since last CLK Pulse > timeout  
          DATbit = 0;                                                        //     Clear the value and Start a new Read
          value = 0;
        }  
        if (DATbit < 20) {                                                   //   If current bit is a value bit ( 0..19 )   
             if ( digitalRead (DATpin) == LOW ) value |= long(1) << DATbit;  //      put it into the value
        }                                      
        if (DATbit == 20) {                                                  //   If we've Reached the sign bit
            if (digitalRead (DATpin) == LOW ) value = 0 - value ;            //       If it's LOW make value negative
            DataOut();                                                       //   Write out the value
        }
        DATbit++;                                                            //   Increment bit counter
        CLKtime = micros();                                                  //   update timestamp of CLK pulse
      }
    }
};

  axis Xaxis('X',XCLKpin,XDATpin);
  axis Yaxis('Y',YCLKpin,YDATpin);
  axis Zaxis('Z',ZCLKpin,ZDATpin);

void setup() {
  Serial.begin(BaudRate);   
}

void loop() {
  Xaxis.check();
  Yaxis.check();
  Zaxis.check();
}

