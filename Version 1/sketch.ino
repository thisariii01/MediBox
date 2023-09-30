//Include libraries
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHTesp.h>
#include <WiFi.h>
#include <time.h>
#include <Preferences.h>

//Pin Declaration
#define PB_DOWN 32
#define PB_UP 33
#define PB_OK 34
#define PB_CANCEL 35
#define BUZZER 4
#define LED 2

//OLED parameters
#define SCREEN_WIDTH  128 //OLED display width in pixels
#define SCREEN_HEIGHT 64 //OLED display height in pixels
#define OLED_RESET -1 //Reset pin number
#define SCREEN_ADDRESS 0x3c 

//DHT 
#define DHTpin 15
float temperature;
float humidity;

//Buzzer
int C = 262;
int D = 294;
int E = 330;
int F = 349;
int G = 392;
int A = 440;
int B = 494;
int C_H = 523;
int n_notes = 8;
int notes[] = {C, D, E, F, G, A, B, C_H};
int melody[] = {C, C, G, G, A, A, G, F, F, E, E, D, D, C, G, G, F, F, E, E, D, G, G, F, F, E, E, D, C, G, G, A, G, F, F, E, E, D, C };

//Alarm
bool alarm_enabled = true;
bool DHT_alert_enabled = true;
int n_alarm = 3;
int alarm_hours[] = {0, 0, 0};
int alarm_minutes[] = {0, 0, 0};
bool alarm_triggered[] = {false, false, false};

//Time
#define NTP_SERVER   "pool.ntp.org"
#define UTC_OFFSET_DST 0

int days;
int hours;
int minutes;
int seconds;
int current_zone = 35;
String time_zones[] = {"-12:00", "-11:30", "-11:00", "-10:30", "-10:00", "-9:30", "-9:00", "-8:30", "-8:00", "-7:30", "-7:00", "-6:30", "-6:00", "-5:30", "-5:00", "-4:30", "-4:00", "-3:30", "-3:00", "-2:30", "-2:00", "-1:30", "-1:00", "-0:30", "+0:00", "+0:30", "+1:00", "+1:30", "+2:00", "+2:30", "+3:00", "+3:30", "+4:00", "+4:30", "+5:00", "+5:30", "+6:00", "+6:30", "+7:00", "+7:30", "+8:00", "+8:30", "+9:00", "+9:30", "+10:00", "+10:30", "+11:00", "+11:30", "+12:00"};

//Menu
int max_modes = 5;
String options[] = {"1- Check Temperature And Humidity" , "2 - Set Time", "3 - Set Alarm", "4 - Disable Alarms ", "5- Disable Alerts"};

Preferences pref;
DHTesp dhtSensor;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);  //OLED display


//Function to run once
void setup() {
	Serial.begin(9600);
 
 // Pinmodes setup
	pinMode(BUZZER, OUTPUT);
	pinMode(LED, OUTPUT);
	pinMode(PB_CANCEL, INPUT);
	pinMode(PB_UP, INPUT);
	pinMode(PB_DOWN, INPUT);
	pinMode(PB_OK, INPUT);

	// DHT setup
	dhtSensor.setup(DHTpin, DHTesp::DHT22);

	// Display setup
	if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)){
		Serial.println(F("SSD1306 allocation failed"));
		for(;;);
	}
	display.display();
	delay(2000);

	// Clear the buffer
	display.clearDisplay();
	print_line("Welcome to Medibox", 2, 0, 0);
	delay(3000);

	WiFi.begin("Wokwi-GUEST", "", 6);
	while (WiFi.status() != WL_CONNECTED) {
		delay(250);
		display.clearDisplay();
		print_line("Connectingto WiFi...", 2, 0, 0);
	}
	
	display.clearDisplay();
	print_line("Connected to WiFi", 2, 0, 0);

	pref.begin("Time Zone", false);
  unsigned int tz = pref.getUInt("tz", 35);
  const int UTC_OFFSET  =   (tz-24)*1800;
	configTime(UTC_OFFSET, UTC_OFFSET_DST, NTP_SERVER);
	
	display.clearDisplay();
}

//Function to run repeatedly
void loop() {
	update_time_with_check_alarm();
	if (digitalRead(PB_OK) == LOW) {
		delay(200);
		go_to_menu();
	}
}

//Function to print a line of text in a given text size and position
void print_line(String text, int text_size, int row, int column) {
	display.setTextSize(text_size);             //Normal 1:1 pixel scale
	display.setTextColor(SSD1306_WHITE);        //Draw white box
	display.setCursor(column, row);             //Start at (row,cloumn) 
	display.println(text);

	display.display();
}

//Function to check if DHT readings fall under nominal values and alert if not
void check_DHT_and_alert() {
	humidity  = dhtSensor.getHumidity();
  temperature = dhtSensor.getTemperature();
	if (DHT_alert_enabled) {
		bool all_good = true;
		if (temperature > 32) {
			display.clearDisplay();
			print_line("HIGH TEMPERATURE!!!", 1, 0, 0);
			print_line("Press Cancel to Disable Alert", 1, 40, 0);
			all_good = false;
		}
		else if (temperature < 26) {
			display.clearDisplay();
			print_line("LOW TEMPERATURE!!!", 1, 0, 0);
			print_line("Press Cancel to Disable Alert", 1, 40, 0);
			all_good = false;
		}
		if (humidity > 80) {
			display.clearDisplay();
			print_line("HIGH HUMIDITY!!!", 1, 25, 0);
			print_line("Press Cancel to Disable Alert", 1, 40, 0);
			all_good = false;
		}
		else if (humidity < 60) {
			display.clearDisplay();
			print_line("LOW HUMIDITY!!!", 1, 25, 0);
			print_line("Press Cancel to Disable Alert", 1, 40, 0);
			all_good = false;
		}
		while (DHT_alert_enabled && all_good == false) {
			tone(BUZZER, F);
			digitalWrite(LED, HIGH);
			delay(500);
			noTone(BUZZER);
			digitalWrite(LED, LOW);
			if (digitalRead(PB_CANCEL) == LOW) {
				DHT_alert_enabled == false;
				options[4] = "5 - Enable Alerts";
				delay(200);
				display.clearDisplay();
				break;
			}
		}
	}
}

//Function to update the time using time library
void update_time() {
	struct tm timeinfo;
	getLocalTime(&timeinfo);

	char timeDay[3];
	strftime(timeDay, 3, "%d", &timeinfo);
	days = atoi(timeDay);

	char timeHour[3];
	strftime(timeHour, 3, "%H", &timeinfo);
	hours = atoi(timeHour);

	char timeMinute[3];
	strftime(timeMinute, 3, "%M", &timeinfo);
	minutes = atoi(timeMinute);

	char timeSecond[3];
	strftime(timeSecond, 3, "%S", &timeinfo);
	seconds = atoi(timeSecond);
}

//Function to automatically update time while checking for alarms
void update_time_with_check_alarm() {
	update_time();
	main_display();

	if (alarm_enabled) {
		for (int i = 0; i < n_alarm; i++) {
			if ((alarm_triggered[i] == false) && (alarm_hours[i] == hours) && (alarm_minutes[i] == minutes)) {
				ring_alarm();
				alarm_triggered[i] = true;
			}
		}
	}
}

//Function for ringing the alarm
void ring_alarm() {
	//Show message on display
	display.clearDisplay();
	print_line("Medicine ", 2, 0, 0);
	print_line("Time", 2, 25, 0);

	//Light up LED
	digitalWrite(LED, HIGH);

	//Ring the buzzer
	bool stopped_buzzer = false;
	while (stopped_buzzer == false && digitalRead(PB_CANCEL) == HIGH) {
		for (int i = 0; i < 10; i++) {
			if (digitalRead(PB_CANCEL) == LOW) {
				delay(200);
				stopped_buzzer = true;
				break;
			}
			twinkle();
		}
	}
	digitalWrite(LED, LOW);
	display.clearDisplay();
}

//Function to display the current time, temperature and humidity
void main_display() {
	display.clearDisplay();
	print_line("Smart Medibox", 1, 0, 25);

	// display time
	if (hours < 10) print_line(String(0) + String(hours), 2, 20, 25);
	else print_line(String(hours), 2, 20, 25);
	print_line(":", 2, 20, 45);
	if (minutes < 10) print_line(String(0) + String(minutes), 2, 20, 55);
	else print_line(String(minutes), 2, 20, 55);
	print_line(":", 2, 20, 75);
	if (seconds < 10) print_line(String(0) + String(seconds), 2, 20, 85);
	print_line(String(seconds), 2, 20, 85);

	check_DHT_and_alert();

	print_line("Main Menu : Press OK", 1, 45, 0);
	delay(1000);
	display.display();
}

//Function to wait for button press in the menu
int wait_for_button_press() {
	while (true) {
		if (digitalRead(PB_UP) == LOW) {
			delay(200);
			return PB_UP;
		}

		else if (digitalRead(PB_DOWN) == LOW) {
			delay(200);
			return PB_DOWN;
		}

		else if (digitalRead(PB_OK) == LOW) {
			delay(200);
			return PB_OK;
		}

		else if (digitalRead(PB_CANCEL) == LOW) {
			delay(200);
			return PB_CANCEL;
		}
		update_time();
	}
}

//Function to navigate through the menu
void go_to_menu() {
	int current_mode = 0;
	while (digitalRead(PB_CANCEL) == HIGH) {
		display.clearDisplay();
		print_line(options[current_mode], 1, 0, 0);

		int pressed = wait_for_button_press();

		if (pressed == PB_UP) {
			current_mode += 1;
			current_mode %= max_modes;
			delay(200);
		}

		else if (pressed == PB_DOWN) {
			current_mode -= 1;
			if (current_mode < 0) current_mode = max_modes - 1;
			delay(200);
		}

		else if (pressed == PB_OK) {
			Serial.println(current_mode);
			delay(200);
			run_mode(current_mode);
		}

		else if (pressed == PB_CANCEL) {
			delay(200);
			break;
		}
	}
}

//Function to decide what to do when each option of the menu is selected
void run_mode(int mode){
	if (mode == 0) display_TH();

	else if(mode == 1) set_time();

	else if (mode ==2) {
    set_alarm_menu();
  }

  else if (mode == 3) {
    alarm_enabled = !alarm_enabled;
    display.clearDisplay();
    if (!alarm_enabled) {
			print_line("Alarms are disabled", 1, 0, 0);
      options[3] = "4 - Enable Alarms";  
    }
    else {
			print_line("Alarms are enabled", 1, 0, 0);
      options[3] = "4 - Disable Alarms";
    }
    delay(1000);
  }

  else if (mode == 4) {
    DHT_alert_enabled = !DHT_alert_enabled;
    display.clearDisplay();
    if (!DHT_alert_enabled) {
      print_line("T&H Alerts are disabled", 1, 0, 0);
      options[4] = "5 - Enable Alerts";
    }
    else {
      print_line("T&H Alerts are enabled", 1, 0, 0);
      options[4] = "5 - Disable Alerts";
    }
    delay(1000);
  }
}

//Function to display the current temperature and humidity
void display_TH() {
	while (digitalRead(PB_OK) == HIGH) {
		display.clearDisplay();
		humidity  = dhtSensor.getHumidity();
  	temperature = dhtSensor.getTemperature();
		print_line("T:" + String(temperature, 2) +  "C" + "  " + "H:" + String(humidity, 1) + "%" + "   " + "Press OK", 2 , 0, 0);
	}
  delay(1000);
}

//Function to set the time by selecting the time zone
void set_time() {
	while (digitalRead(PB_CANCEL) == HIGH) {
    display.clearDisplay();
    print_line(time_zones[current_zone], 0, 0, 2);

    int pressed = wait_for_button_press();

    if (pressed == PB_UP) {
      current_zone += 1;
      current_zone = current_zone % 49;
    }

    else if (pressed == PB_DOWN) {
      current_zone -= 1;
      if (current_zone < 0) {
        current_zone = 48;
      }
    }

    else if (pressed == PB_OK) {
      pref.putUInt("tz", current_zone);
      pref.end();
      display.clearDisplay();
      print_line("Restarting...", 2, 0, 0);
      delay(3000);
      ESP.restart();
    }

    else if (pressed == PB_CANCEL) {
      break;
    }
  }
  display.clearDisplay();
  delay(1000);
}

//Function to set the time by taking the user input
void set_alarm (int alarm) {
	int temp_hour = alarm_hours[alarm];
	while (true) {
		display.clearDisplay();
		print_line("Enter hour: " + String(temp_hour), 1, 0, 2);

		int pressed = wait_for_button_press();
		if (pressed == PB_UP) {
			delay(200);
			temp_hour += 1;
			temp_hour = temp_hour % 24;
		}

		else if (pressed == PB_DOWN) {
			delay(200);
			temp_hour -= 1;
			if(temp_hour < 0) temp_hour = 23;
		}

		else if (pressed == PB_OK) {
			delay(200);
			alarm_hours[alarm] = temp_hour;
			break;
		}

		else if(pressed == PB_CANCEL){
			delay(200);
			break;
		}

	}
	int temp_minute = alarm_minutes[alarm];
	while (true) {
		display.clearDisplay();
		print_line("Enter minute: " + String(temp_minute), 1, 0, 2);

		int pressed = wait_for_button_press();
		if (pressed == PB_UP) {
			delay(200);
			temp_minute += 1;
			temp_minute = temp_minute % 60;
		}

		else if (pressed == PB_DOWN) {
			delay(200);
			temp_minute -=1 ;
			if (temp_minute <0 ) temp_minute = 59;
		}

		else if (pressed == PB_OK) {
			delay(200);
			alarm_minutes[alarm] = temp_minute;
			break;
			}

		else if (pressed == PB_CANCEL) {
			delay(200);
			break;
			}

	}
	display.clearDisplay();
	print_line("Alarm is set",1,0,2);
	delay(1000);
}

//Function to guide through the menu and set three alarms
void set_alarm_menu() {
	int i =0;
	while (digitalRead(PB_CANCEL) == HIGH) {
		display.clearDisplay();
		print_line("Set Alarm " + String(i+1), 1, 0, 1);
		int pressed = wait_for_button_press();

		if (pressed == PB_UP) {
			i += 1;
			i = i % n_alarm;
		}
		else if (pressed == PB_DOWN) {
			i -= 1;
			if (i < 0)	i = n_alarm - 1;
		}
		else if (pressed == PB_OK) {
			set_alarm(i);
			break;
		}
	}
	display.clearDisplay();
	delay(1000);
}

void twinkle() {
	int n = 39;
	for (int i=0; i<39; i++) {
		tone(BUZZER, melody[i]);
		delay(500);
		noTone(BUZZER);
		delay(100);
	}
}
