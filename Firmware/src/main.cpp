#include <Arduino.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include "Adafruit_miniTFTWing.h"
#include <SPI.h>
#include <FastLED.h>

#define NUM_LEDSBORDER 1 // change
#define NUM_LEDSLEGEND 1 // change
#define NUM_LEDSLEADER 1 // change

#define DATA_PINBORDER 22
#define DATA_PINLEGEND 19
#define DATA_PINLEADER 21

/*
const int ledborder = 22;
const int legendborder = 19;
const int leaderborder = 21;
*/
CRGB ledsborder[NUM_LEDSBORDER];
CRGB ledslegend[NUM_LEDSLEGEND];
CRGB ledsleader[NUM_LEDSLEADER];


CRGB colors[] = {CRGB::White, CRGB::Green, CRGB::Blue, CRGB::Black};


#define TFT_RST 8

#define TFT_CS 6
#define TFT_DC 7

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);

// put function declarations here:

const int onoff = 20;
const int config = 17;
const int action = 18;

const int ledswitch = 10;
const int threeswitch = 11;

const int touch = 16;

static String state = "Default";
static String selectstate = "left";
static String settingsstate = "Menu";

//debounce for config
static bool debounce = false;
static bool ledstate = false;


static int points;
static int xp;
static int riftboundthreshhold = 8;
static bool aspirants = false;
static bool activablelegend = false;



static bool isyourturn = false;
static bool islegendexhausted = false;

//debounce for touch sensors
static bool debouncetwo = false;

//debounce for action
static bool debounceaction = false;

//debounce for onoff
static bool debounceonoff = false;

static bool onoffstate = false;

static bool screenneedsupdate = false;

static CRGB bordercolor = CRGB::White;
//To Do:
//Led settings:
//On/Off  - done
//Led Colors - done
//Led turns different color based on turn - done I think
//Turn system: - need to do

// How will player know?
// Option 1: Pass turn button
// Option 2: No other idea
//Touch sensor --> led on/off
//Brightness

//To do today:
//Rewrite returncolor(), no need for it - done
//Turn based mode - done

//Connection mode? -- Nah, would have to redisgn PCB / Mat


static void ledhandling(String state) {
  if (state == "Turn") {
    if (isyourturn) {

      ledsborder[0] = CRGB::Green;
      islegendexhausted = false;
    //border color --> your color chosen
  } else {
    //border color --> red
    ledsborder[0] = CRGB::Red;
    
  }

  } else if (state == "Leader_Legend") {
    if (activablelegend) {
      if (islegendexhausted) {
        if (state == "Riftbound") {
          ledslegend[0] = CRGB::Red;
        } else if (state == "One Piece") {
          ledsleader[0] = CRGB::Red;
        }
        //border color --> exhausted color
        
      } else {  
        //border color --> legend color
        if (state == "Riftbound") {
          ledslegend[0] = bordercolor;
        } else if (state == "One Piece") {
          ledsleader[0] = bordercolor;
        }

      }
      //border color --> your color chosen
    } else {
      //border color --> chosen color
      if (state == "Riftbound") {
        ledslegend[0] = bordercolor;
      }
      if (state == "One Piece") {
        ledsleader[0] = bordercolor;
      }
    }
  }
  

}

static void wingame(boolean value) {
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);
  if (value) {
    tft.println("You win!");
  } else {
    tft.println("You lose!");
  }
  delay(5000);
  state = "Default";
  points = 0;
}

static void selectionarrows(String ref) {
  tft.fillScreen(ST77XX_BLACK);
  if (ref == "Riftbound") {
    if (selectstate == "up") {
      tft.setCursor(20, 20);
      tft.println("<");
    } else if (selectstate == "middle") {
      tft.setCursor(20, 40);
      tft.println("<");
    } else if (selectstate == "down") {
      tft.setCursor(20, 60);
      tft.println("<");
    } else if (selectstate == "downonemoretime") {
      tft.setCursor(20, 80);
      tft.println("<");
    }
  } else if (ref == "One Piece") {
    if (selectstate == "up") {
      tft.setCursor(20, 20);
      tft.println("<");
    } else if (selectstate == "middle") {
      tft.setCursor(20, 40);
      tft.println("<");
    } else if (selectstate == "down") {
      tft.setCursor(20, 60);
      tft.println("<");
    }

  } else if (ref == "Settings") {
    if (settingsstate == "Back") {
      tft.setCursor(20, 0);
      tft.println("<");
    } else if (settingsstate == "Led") {
      tft.setCursor(20, 20);
      tft.println("<");
    } else if (settingsstate == "Riftbound") {
      tft.setCursor(20, 40);
      tft.println("<");
    } else if (settingsstate == "One Piece") {
      tft.setCursor(20, 60);
      tft.println("<");
    }


    //if we're calling it with the settings state, we check which one, I think

    //we need to make sure we're in settings --> subsettings mode
    //CALL LED SETTINGS WHEN IN LED SETTINGS, AND FOR THE REST THE SAME
  } else if (ref == "LedSettings") {
    if (settingsstate == "Back") {
      tft.setCursor(20, 0);
      tft.println("<");
    } else if (settingsstate == "LedOnOff") {
      tft.setCursor(20, 20);
      tft.println("<");
    } else if (settingsstate == "Color") {
      tft.setCursor(20, 40);
      tft.println("<");
    } else if (settingsstate == "Brightness") {
      tft.setCursor(20, 60);
      tft.println("<");
    }

  } else if (ref == "Color") {
    if (settingsstate == "White") {
      tft.setCursor(20, 20);
      tft.println("<");
    } else if (settingsstate == "Green") {
      tft.setCursor(20, 40);
      tft.println("<");
    } else if (settingsstate == "Blue") {
      tft.setCursor(20, 60);
      tft.println("<");
    } else if (settingsstate == "Black") {
      tft.setCursor(20, 80);
      tft.println("<");
    }
  } else if (ref == "RiftboundSettings") {
    if (settingsstate == "Back") {
      tft.setCursor(20, 0);
      tft.println("<");
    } else if (settingsstate == "Aspirants") {
      tft.setCursor(20, 20);
      tft.println("<");
    } else if (settingsstate == "ActivableLegend") {
      tft.setCursor(20, 40);
      tft.println("<");
    }

  } else if (ref == "OnePieceSettings") {
    if (settingsstate == "Back") {
      tft.setCursor(20, 0);
      tft.println("<");
    } else if (settingsstate == "Lives") {
      tft.setCursor(20, 20);
      tft.println("<");
    }
  } else if (ref == "Default") {
    if (selectstate == "left") {
      tft.setCursor(20, 20);
      tft.println("<");
    } else if (selectstate == "middle") {
      tft.setCursor(20, 40);
      tft.println("<");
    } else if (selectstate == "right") {
      tft.setCursor(20, 60);
      tft.println("<");
    }
  }

}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  state = "Default";
  tft.initR(INITR_BLACKTAB); // Initialize a ST7735S chip, black tab
  tft.fillScreen(ST77XX_BLACK);

  pinMode(onoff, INPUT_PULLUP);
  pinMode(config, INPUT_PULLUP);
  pinMode(action, INPUT_PULLUP);

  pinMode(ledswitch, OUTPUT);
  pinMode(threeswitch, OUTPUT);

  pinMode(touch, INPUT);

  FastLED.addLeds<WS2812, DATA_PINBORDER, GRB>(ledsborder, NUM_LEDSBORDER);
  FastLED.addLeds<WS2812, DATA_PINLEGEND, GRB>(ledslegend, NUM_LEDSLEGEND);
  FastLED.addLeds<WS2812, DATA_PINLEADER, GRB>(ledsleader, NUM_LEDSLEADER);

}

void loop() {
  // put your main code here, to run repeatedly:

  if (digitalRead(onoff) == LOW) {
    if (debounceonoff == false) {
      debounceonoff = true;
      if (onoffstate) {
        onoffstate = false;
        digitalWrite(ledswitch, HIGH);
        digitalWrite(threeswitch, LOW);
      } else {
        onoffstate = true;
        digitalWrite(ledswitch, LOW);
        digitalWrite(threeswitch, HIGH);
      }
      
    }
    delay(50);
  } else {
    debounceonoff = false;
  } 



  //Screen
  if (state == "Default") {
    if (screenneedsupdate) {
      tft.setCursor(0, 0);
      tft.setTextColor(ST77XX_WHITE);
      tft.setTextSize(1);
      tft.println("Select gamemode: ");
      tft.setCursor(0, 20);
      tft.println("Riftbound");
      tft.setCursor(0, 40);
      tft.println("One Piece");
      tft.setCursor(0, 60);
      tft.println("Settings");
      selectionarrows("Default");
      screenneedsupdate = false;
    }
    
    
    

    if(digitalRead(config) == LOW) {
      if (debounce == false) {

        debounce = true;
        screenneedsupdate = true;
        if (selectstate == "left") {
          selectstate = "middle"; 
        } else if (selectstate == "middle") {
          selectstate = "right";
        } else if (selectstate == "right") {
          selectstate = "left";
        }
    
      } 
    
      
      delay(50);
    } else {
      debounce = false;
    }

    if (digitalRead(action) == LOW) {
      if (state == "Default") {
      if (debounceaction == false) {
        debounceaction = true;
        screenneedsupdate = true;

        //remove selectionarrows in a sec
        if (selectstate == "left") {
          state = "Riftbound";
          ledsleader[0] = CRGB::Black;
          
          points = 0;
          xp = 0;
          selectstate = "up";
        } else if (selectstate == "middle") {
          state = "One Piece";
          //idk bruh
          ledslegend[0] = CRGB::Black;
          points = points;
          selectstate = "up";
        } else if (selectstate == "right") {
          state = "Settings";
          selectstate = "up";
          settingsstate = "Back";
          
        }

        //Make sure the state isn't set for something previously selected, so that the screen doesn't mess up
        selectstate = "";

      } 

    } 
    delay(50);
  } else {
    debounceaction = false;
  }

  }

  //We need a way to make the user know which selection is happening
  //Arrows like this <
  //How?
  //Write unnecessary boolean values
  //Or compare the selectstate with the current option, write the arrows on that index

  if (state == "Riftbound") {
    if (screenneedsupdate) {
      screenneedsupdate = false;
      tft.fillScreen(ST77XX_BLACK);
      tft.setCursor(0, 0);
      tft.setTextColor(ST77XX_WHITE);
      tft.setTextSize(1);
      tft.println("Riftbound");
      tft.setCursor(0, 20);
      tft.println("Points:" + String(points));
      tft.setCursor(0, 40);
      tft.println("XP:" + String(xp));
      tft.setCursor(0, 60);
      tft.println("Pass Turn");
      tft.setCursor(0, 80);
       selectionarrows("Riftbound");
      tft.println("Exit");
    }
    
    

    if (digitalRead(config) == LOW) {
      if (debounce == false) {
        debounce = true;
        screenneedsupdate = true;
        if (selectstate == "up") {
          selectstate = "middle"; 

        } else if (selectstate == "middle") {
          selectstate = "down";
        } else if (selectstate == "down") {
          selectstate = "downonemoretime";
        } else if (selectstate == "downonemoretime") {
          selectstate = "up";
        }
      }
      delay(50);
    } else {
      debounce = false;
    }
   
    if (digitalRead(action) == LOW) {
      screenneedsupdate = true;
      if (debounceaction == false) {
        debounceaction = true;
        if (selectstate == "up") {

          points++;
          if (points >= riftboundthreshhold) {
            wingame(true);
          }
        } else if (selectstate == "middle") {
          xp++;
        } else if (selectstate == "down") {
          state = "WaitingForTurn";
        } else if (selectstate == "downonemoretime") {
          state = "Default";
        }
      }
      delay(50);
    } else {
      debounceaction = false;
    }


  } else if (state == "One Piece") {
    if (screenneedsupdate) {
      tft.fillScreen(ST77XX_BLACK);
      tft.setCursor(0, 0);
      tft.setTextColor(ST77XX_WHITE);
      tft.setTextSize(1);
      tft.println("One Piece");
      tft.setCursor(0, 20);
      tft.println("Lives: " + String(points));
      tft.setCursor(0, 40);
      tft.println("Pass Turn");
      tft.setCursor(0, 60);
      tft.println("Exit");
      
      selectionarrows("One Piece");
      screenneedsupdate = false;
    }
    

    if (digitalRead(config) == LOW) {
      if (debounce == false) {
        debounce = true;
        screenneedsupdate = true;
        if (selectstate == "up") {
          selectstate = "middle"; 

        } else if (selectstate == "middle") {
          selectstate = "down";
        } else if (selectstate == "down") {
          selectstate = "up";
        }
      }
      delay(50);
    } else {
      debounce = false;
    }

    if (digitalRead(action) == LOW) {
      if (debounceaction == false) {
        debounceaction = true;
        screenneedsupdate = true;
        if (selectstate == "up") {
          points--;
          if (points < 0) {
            wingame(false);
          }
        } else if (selectstate == "down") {
          state = "Default";
        }
      }
      delay(50);
    } else {
      debounceaction = false;
    }

    //code here later
  } else if (state == "Settings") {

    if (settingsstate == "Menu") {   
      if (screenneedsupdate) {
      tft.fillScreen(ST77XX_BLACK);
      tft.setCursor(0, 0);
      tft.setTextColor(ST77XX_WHITE);
      tft.setTextSize(1);
      tft.println("Back");
      tft.setCursor(0, 20);
      tft.println("LED");
      tft.setCursor(0, 40);
      tft.println("Riftbound");
      tft.setCursor(0, 60);
      tft.println("One Piece");
      selectionarrows("Settings");
      screenneedsupdate = false;
      } 
      
      //Scrolling through options
      if (digitalRead(config) == LOW) {
        if (debounce == false) {
          debounce = true;
          screenneedsupdate = true;
          if (settingsstate == "Back") {
            settingsstate = "Led"; 

          } else if (settingsstate == "Led") {
            settingsstate = "Riftbound";
          } else if (settingsstate == "Riftbound") {
            settingsstate = "One Piece";
          } else if (settingsstate == "One Piece") {
            settingsstate = "Back";
          }
        }
        delay(50);
      } else {
        debounce = false;
      }
      

      //Selecting options
      if (digitalRead(action) == LOW) {
        if (debounceaction == false) {
          debounceaction = true;
          screenneedsupdate = true;
          if (settingsstate == "Back") {
            state = "Default";
          } else if (settingsstate == "Led") {
            settingsstate = "LedSettings";
          } else if (settingsstate == "Riftbound") {
            settingsstate = "RiftboundSettings";
          } else if (settingsstate == "One Piece") {
            settingsstate = "One PieceSettings";
          }
        }
        delay(50);
      } else {
        debounceaction = false;
      }


      //Led options
    } else if (settingsstate == "LedSettings") {
      if (screenneedsupdate) {
        tft.fillScreen(ST77XX_BLACK);
        tft.setCursor(0, 0);
        tft.setTextColor(ST77XX_WHITE);
        tft.setTextSize(1);
        tft.println("Back");
        tft.setCursor(0, 20);
        tft.println("Led On/Off" + String(digitalRead(ledswitch) == HIGH ? "On" : "Off"));
        tft.setCursor(0, 40);
        tft.println("Color");
        tft.setCursor(0, 60);
        tft.println("Brightness");
        selectionarrows("LedSettings");
        screenneedsupdate = false;

      }
      
      
      if (digitalRead(config) == LOW) {
        if (debounce == false) {
          debounce = true;
          screenneedsupdate = true;
          if (settingsstate == "Back") {
            settingsstate = "LedOnOff"; 

          } else if (settingsstate == "LedOnOff") {
            settingsstate = "Color";
          } else if (settingsstate == "Color") {
            settingsstate = "Brightness";
          } else if (settingsstate == "Brightness") {
            settingsstate = "Back";
          }
        }
        delay(50);
      } else {
        debounce = false;
      }
      

      if (digitalRead(action) == LOW) {
        if (debounceaction == false) {
          debounceaction = true;
          screenneedsupdate = true;
          if (settingsstate == "Back") {
            settingsstate = "";
            state = "Default";
          } else if (settingsstate == "LedOnOff") {
            ledstate = !ledstate;
            digitalWrite(ledswitch, ledstate ? HIGH : LOW);
          } else if (settingsstate == "Color") {
            if (screenneedsupdate) {
              tft.fillScreen(ST77XX_BLACK);
              tft.setCursor(0, 0);
              tft.setTextColor(ST77XX_WHITE);
              tft.setTextSize(1);
              tft.println("Select Color:");
              tft.setCursor(0, 20);
              tft.println("White");
              tft.setCursor(0, 40);
              tft.println("Green");
              tft.setCursor(0, 60);
              tft.println("Blue");
              tft.setCursor(0, 80);
              tft.println("Black");
              selectionarrows("Color");
              screenneedsupdate = false;
            }
            
            
             if (digitalRead(config) == LOW) {
              if (debounce == false) {
                debounce = true;
                screenneedsupdate = true;
                if (settingsstate == "Color") {
                  settingsstate = "White";
                } else if (settingsstate == "White") {
                  settingsstate = "Green";
                } else if (settingsstate == "Green") {
                  settingsstate = "Blue";
                } else if (settingsstate == "Blue") {
                  settingsstate = "Black";
                } else if (settingsstate == "Black") {
                  settingsstate = "Color";
                }
              }
              delay(50);
            } else {
              debounce = false;
            }
            

             if (digitalRead(action) == LOW) {
              if (debounceaction == false) {
                debounceaction  = true;
                screenneedsupdate = true;
                // Handle color selection based on settingsstate
                if (settingsstate == "White") {
                  FastLED.setBrightness(180); //Full blast of white will draw too much power
                  bordercolor = CRGB::White;
                  ledsborder[0] = bordercolor;
                  FastLED.show();
                } else if (settingsstate == "Green") {
                  bordercolor = CRGB::Green;
                  ledsborder[0] = bordercolor;
                  FastLED.show();
                } else if (settingsstate == "Blue") {
                  bordercolor = CRGB::Blue;
                  ledsborder[0] = bordercolor;
                  FastLED.show();
                } else if (settingsstate == "Black") {
                  bordercolor = CRGB::Black;
                  FastLED.setBrightness(0);
                  FastLED.show();
                }
              }
              delay(50);
            } else {
              debounceaction = false;
            }



            // Handle color change
          } else if (settingsstate == "Brightness") {
            // Handle brightness change
            if (screenneedsupdate) {
              tft.fillScreen(ST77XX_BLACK);
              tft.setCursor(0, 0);
              tft.setTextColor(ST77XX_WHITE);
              tft.setTextSize(1);
              tft.println("Select Brightness:");
              tft.setCursor(0, 20);
              tft.println("Low");
              tft.setCursor(0, 40);
              tft.println("Medium");
              tft.setCursor(0, 60);
              tft.println("High");
              screenneedsupdate = false;
            }
            
            
             if (digitalRead(config) == LOW) {
              if (debounce == false) {
                debounce = true;
                screenneedsupdate = true;
                if (settingsstate == "Brightness") {
                  settingsstate = "Low";
                } else if (settingsstate == "Low") {
                  settingsstate = "Medium";
                } else if (settingsstate == "Medium") {
                  settingsstate = "High";
                } else if (settingsstate == "High") {
                  settingsstate = "Brightness";
                }
              }
              delay(50);
            } else {
              debounce = false;
            }

             if (digitalRead(action) == LOW) {
              if (debounceaction == false) {
                debounceaction = true;
                screenneedsupdate = true;
                // Handle brightness selection based on settingsstate
                if (settingsstate == "Low") {
                  FastLED.setBrightness(50);
                  FastLED.show();
                } else if (settingsstate == "Medium") {
                  FastLED.setBrightness(128);
                  FastLED.show();
                } else if (settingsstate == "High") {
                  if (bordercolor == CRGB::White) {
                    FastLED.setBrightness(180); //Full blast of white will draw too much power
                    bordercolor = CRGB::Black; //Reset border color so that if the user goes back to white, it will be at the correct brightness
                  } else {
                    FastLED.setBrightness(255);
                  }
                  
                  FastLED.show();
                }
              }
              delay(50);
            } else {
              debounceaction = false;
            }
        } else {
          debounce = false;
        }
      }


    } else if (settingsstate == "RiftboundSettings") {
      if (digitalRead(action) == LOW) {
        if (debounce == false) {
          debounce = true;
          // Handle Riftbound settings
          if (screenneedsupdate) {
            tft.fillScreen(ST77XX_BLACK);
            tft.setCursor(0, 0);
            tft.setTextColor(ST77XX_WHITE);
            tft.setTextSize(1);
            tft.println("Back");
            tft.setCursor(0, 20);
            tft.println("Aspirants?" + String(aspirants));
            tft.setCursor(0, 40);
            tft.println("Activable Legend?" + String(activablelegend));
            screenneedsupdate = false;
          }
          

          if (digitalRead(config) == LOW) {
            if (debounce == false) {
              debounce = true;
              screenneedsupdate = true;
              if (settingsstate == "Back") {
                settingsstate = "Aspirants";
                
              } else if (settingsstate == "Aspirants") {
                settingsstate = "Activable Legend";
              } else if (settingsstate == "Activable Legend") {
                settingsstate = "Back";
              }
              delay(50);
            } else debounce = false;
          }

          if (digitalRead(action) == LOW) {
            if (debounceaction == false) {
              debounceaction = true;
              screenneedsupdate = true;
              if (settingsstate == "Back") {
                settingsstate = "Back";
                state = "Settings";
              } else if (settingsstate == "Aspirants") {
                aspirants = !aspirants;
                riftboundthreshhold = aspirants ? 8 : 9; // Example: if aspirants are enabled, reduce the threshold
              } else if (settingsstate == "Activable Legend") {
                activablelegend = !activablelegend;
              }
              delay(50);
            } else {
              debounceaction = false;
            }
          }

          delay(50);
        } else {
          debounce = false;
        }
      }
    } else if (settingsstate == "One PieceSettings") {
      if (screenneedsupdate) {
        tft.fillScreen(ST77XX_BLACK);
        tft.setCursor(0, 0);
        tft.setTextColor(ST77XX_WHITE);
        tft.setTextSize(1);
        tft.println("Back");
        tft.setCursor(0, 20);
        tft.println("Activable Leader?" + String(activablelegend));
        tft.setCursor(0, 40);
        tft.println("Lives?" + String(points));
        screenneedsupdate = false;
      }
      
      if (digitalRead(config) == LOW) {
        if (debounce == false) {
          debounce = true;
          screenneedsupdate = true;
          if (settingsstate == "Back") {
            settingsstate = "Activable Leader";
            
          } else if (settingsstate == "Activable Leader") {
            settingsstate = "Lives";
          } else if (settingsstate == "Lives") {
            settingsstate = "Back";
          }
          delay(50);
        } else debounce = false;
        
      }
      if (digitalRead(action) == LOW) {
        if (debounceaction == false) {
          debounceaction = true;
          screenneedsupdate = true;
          if (settingsstate == "Back") {
            settingsstate = "Back";
            state = "Settings";
          } else if (settingsstate == "Activable Leader") {
            activablelegend = !activablelegend;
          } else if (settingsstate == "Lives") {
            points++;
          }
        }
        delay(50);
    } else {
          debounceaction = false;
      
  }



  }

} 


//we cannot use the same debounce here
if (state == "WaitingForTurn") {
  ledhandling("Turn");

  if (digitalRead(action) == LOW) {
    if (debounceaction == false) {
      debounceaction = true;
      isyourturn = true;
      state = "Turn";
      screenneedsupdate = true;
    }
    delay(50);
  } else {
    debounceaction = false;
  }

}
if (digitalRead(touch) == HIGH) {
  if (debouncetwo == false) {
    debouncetwo = true;
    ledhandling("Leader_Legend");

  } 
  
} else {
  debouncetwo = false;
}

FastLED.show();

}

}

// put function definitions here:
