//Reaction Time Test Project

// Part 1: Setup
#include <LiquidCrystal.h> // This is the lcd library i used

LiquidCrystal lcd(13, 12, 5, 4, 3, 2); //setup for lcd pins to match with library

int ledPin = 9;      // led connected to pin 9
int buttonPin = 18;  // Interrupt pin on Pin 18
int buzzerPin = 10;  // buzzer pin on pin 10

//variables I made to store info
unsigned long ledReactionTime = 0;    // store reaction time for led
unsigned long buzzerReactionTime = 0; // store reaction time for buzzer

unsigned long bestLedTime = 9999;     // store fastest time for led
unsigned long bestBuzzerTime = 9999;  // store fastest time for buzzer

// this is volatile becuae of the interupt (meaning buttonPressed value could change abnormally)
volatile bool buttonPressed = false;  // Flag for interrupt, bool keeps true and false variables (relating to high score)

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2); // show 16 columns and 2 rows on lcd
  
  pinMode(ledPin, OUTPUT);          // outputs (can be turned on and off)
  pinMode(buzzerPin, OUTPUT);       // outputs (can be turned on and off)
  pinMode(buttonPin, INPUT_PULLUP); // Button with internal pull-up resistor (so is in a constant state of HIGH noramlly)

  //hardware interupt here - the program cannot start running until the button is pressed.
  // interrupt on button press (watch for falling edge - it will run when button is pressed)
  attachInterrupt(digitalPinToInterrupt(buttonPin), buttonInterrupt, FALLING);
}

// Part 2 - The Main Loop
void loop() {
  if (buttonPressed) {     // wait for button to be pressed to run the loop
    buttonPressed = false; // Reset the flag

    lcd.setCursor(0, 0);        // starts test
    lcd.print("Reaction Test");
    delay(2000);
    lcd.clear();

    lcd.print("Press button to");
    lcd.setCursor(0, 1);
    lcd.print("start LED Test");

    
    while (digitalRead(buttonPin) == HIGH); // wait for button press 
    delay(500); // Debounce delay to make sure press is not accidental or resarts many times over

    ledReactionTime = runTest("LED", ledPin, false); // run LED test

    lcd.clear();  // wait for button press to do buzzer test
    lcd.print("Press button");
    lcd.setCursor(0, 1);
    lcd.print("for buzzer test");

    while (digitalRead(buttonPin) == HIGH); // Wait for button press
    delay(500); // Debounce delay

    buzzerReactionTime = runTest("Buzzer", buzzerPin, true);  // Run Buzzer test

    displayResults();     // compare and display results

    updateHighScores();  // update and display high scores

    lcd.clear(); // Restart the entire test on button press
    lcd.print("Press to retry");
    while (digitalRead(buttonPin) == HIGH); 
    delay(500); // Debounce delay

    lcd.clear(); // Clear screen
  }
}

//Part 3 - Actual Tests

unsigned long runTest(String testType, int outputPin, bool isBuzzer) { //function that shows tests that are used in loop
  lcd.clear();
  lcd.print("Get Ready...");
  Serial.println("Starting " + testType + " Test");

  delay(random(2000, 4000)); // random delay before start of either led or buzzer

  if (isBuzzer) {          // if doing buzzer test
    tone(outputPin, 1000); // start buzzer
  } else {                 //if doing led test
    digitalWrite(outputPin, HIGH); // turn on LED
  }

  lcd.clear();
  lcd.print("Press now!");
  unsigned long startTime = millis();   // start tracking time 

  while (digitalRead(buttonPin) == HIGH); // wait for button press

  unsigned long reactionTime = millis() - startTime;

  if (isBuzzer) {
    noTone(outputPin); // stop buzzer
  } else {
    digitalWrite(outputPin, LOW); // turn off LED
  }

  lcd.clear();          // shows reaction time for each test
  lcd.print("Time: ");
  lcd.print(reactionTime);
  lcd.print(" ms");
  Serial.println("Reaction time: " + String(reactionTime) + " ms");

  delay(2000);

  lcd.clear();
  lcd.print("Score: ");
  lcd.print(calculateScore(reactionTime));
  delay(1500);

  return reactionTime;
}

// Part 4 - Extra Displays(scores, compare times, and high scores)
//  calculate the score based on reaction time
String calculateScore(unsigned long reactionTime) {
  if (reactionTime < 150) return "Awesome";
  else if (reactionTime < 200) return "Good";
  else if (reactionTime < 350) return "Fair";
  else if (reactionTime < 500) return "Poor";
  else if (reactionTime < 100) return "Insane";
  else return "Awful";
}

void displayResults() {  // compare and display results
  lcd.clear();
  lcd.print("LED: ");
  lcd.print(ledReactionTime);
  lcd.print(" ms");
  delay(2000);

  lcd.setCursor(0, 1);
  lcd.print("Buzzer: ");
  lcd.print(buzzerReactionTime);
  lcd.print(" ms");
  delay(2000);

  lcd.clear();

  if (ledReactionTime < buzzerReactionTime) { //seeing which time is faster
    lcd.print("LED is faster!");
  } else if (ledReactionTime > buzzerReactionTime) {
    lcd.print("Buzzer is faster!");
  } else {
    lcd.print("Tie!");
  }

  delay(2000);
}

// update and display high scores
void updateHighScores() {
  bool newHighScore = false;   // this is where i use the bool variable to track true and false for the high score

  if (ledReactionTime < bestLedTime) { //new high score is true
    bestLedTime = ledReactionTime;
    newHighScore = true;  
  }

  if (buzzerReactionTime < bestBuzzerTime) {  // new high score is false
    bestBuzzerTime = buzzerReactionTime;
    newHighScore = true;
  }

  lcd.clear();  // simple code to display high scores since program initally rna
  lcd.print("High Scores:");
  delay(2000);

  lcd.clear();
  lcd.print("LED: ");
  lcd.print(bestLedTime);
  lcd.print(" ms");
  delay(2000);

  lcd.setCursor(0, 1);
  lcd.print("Buzzer: ");
  lcd.print(bestBuzzerTime);
  lcd.print(" ms");
  delay(2000);

  if (newHighScore) {
    lcd.clear();
    lcd.print("New Record!");   // congradulate user on new high score
    delay(2000);
  }
}

// Part 5 - Hardware Interupt (this triggers the reaction time test to start)
void buttonInterrupt() {  // hardware interupt continued on pin 18
  buttonPressed = true;  // set flag to indicate that the button was pressed

}

