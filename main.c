#include <LiquidCrystal.h>


LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

byte car[8] = {
  B00000,
  B00000,
  B00010,
  B11101,
  B11101,
  B00010,
  B00000,
  B00000
};

byte car2[8] = {
  B00000,
  B00000,
  B01000,
  B10111,
  B10111,
  B01000,
  B00000,
  B00000
};

const int buttonSwitchRightLane = 6;
const int buttonSwitchLeftLane = 7;

int currentCar2XPos,currentCar2YPos; //sparar bil 2's position
int car2Speed = 500; //hur snabbt bil 2 ska röra sig
unsigned long int car2recentMove; //sparar bil 2's senaste rörelse

const int menuSize = 2;
const int lcdColumn = 16;
bool isCar2Present = false;
const int car2Char = 6;
const int carChar = 7;
bool gameOver = false;
bool isPlaying = false;
bool isCarRightLane = true;
int score = 0;

void setup() {
  lcd.begin(16, 2);
  lcd.createChar(carChar, car);
  lcd.createChar(car2Char, car2);
  pinMode(buttonSwitchRightLane, INPUT_PULLUP);
  pinMode(buttonSwitchLeftLane, INPUT_PULLUP);
  randomSeed(analogRead(0));
}

void startGame(); // Declare the startGame function

void handleGame(); // Declare the handleGame function

void loop() {
  lcd.clear();
  startGame();
}


void startGame() {
  isPlaying = true;
  while (isPlaying) {
    handleGame();
  }
}

void putCarRightLane() {
  lcd.setCursor(1, 1);
  lcd.write(carChar);
  lcd.setCursor(1, 0);
  lcd.print(" ");
  
  isCarRightLane = true;
}

void putCarLeftLane() {
  lcd.setCursor(1, 0);
  lcd.write(carChar);
  lcd.setCursor(1, 1);
  lcd.print(" ");
  
  isCarRightLane = false;
}

int checkCollision() { //kollar om bil2 har kolliderat med bil1
    if (isCarRightLane){  //Bil 1 är på 1,0
        if(currentCar2XPos == 2 && currentCar2YPos == 1)
            return 1;
        else    
            return 0;
  }      

    if(!isCarRightLane) { //Bil 1 är på 1,1
        if(currentCar2XPos == 2 && currentCar2YPos == 0)
            return 1;
        else
            return 0;     
    }
}


//Funktion för att flytta bil 2
void moveCar2() {
    if ((millis() - car2recentMove) > (unsigned long)car2Speed) { // Cast car2Speed to unsigned long
        lcd.setCursor(currentCar2XPos, currentCar2YPos);
        lcd.print(" ");
        currentCar2XPos--;
        lcd.setCursor(currentCar2XPos, currentCar2YPos);
        lcd.write(car2Char);

        if (currentCar2XPos == 0) {
            lcd.setCursor(currentCar2XPos, currentCar2YPos);
            lcd.clear();
            if (isCarRightLane)
                putCarRightLane();
            else
                putCarLeftLane();
                isCar2Present = false;
                score++;
                car2recentMove = millis();
        }
        else
            car2recentMove += car2Speed; //adderar car2Speed till car2recentMove för att hålla koll på tiden
    }
}

//Funktion för att försöka spawna bil 2
void spawnCar2() {
    if(!isCar2Present) { // Om bilen inte redan är på skärmen
        int positionX = 15;
        int positionY = random(0, 2);
        lcd.setCursor(positionX, positionY);
        lcd.write(car2Char);
        isCar2Present = true;
        currentCar2XPos = positionX;
        currentCar2YPos = positionY;
        car2recentMove = millis();

    }   
}

void handleGameOver(); // Declare the handleGameOver function
void showCar(int position); // Declare the showCar function

//Funktion för att hantera spelet
void handleGame() {
  while(!gameOver){
    
    static bool spawnCar = true;
        if(spawnCar){
            putCarRightLane(); //Bilen börjar på höger sida
            spawnCar = false;
        }

    lcd.setCursor(13, 0);
    //lcd.print(score);

    if (digitalRead(buttonSwitchLeftLane) == HIGH && isCarRightLane) { 
        putCarLeftLane(); // Change to left lane
    }
    if (digitalRead(buttonSwitchRightLane) == HIGH && !isCarRightLane) {
        putCarRightLane(); // Change to right lane
    }

    if(isCar2Present == false){ // Om bilen inte redan är på skärmen
        //spawnar en bil
        spawnCar2();
        //spela ljud?
    }else{
        //flyttar bilen
        moveCar2();
        if(checkCollision()){ // Om bilen kolliderar
            handleGameOver(); // Hantera Game over
        }
    }

  }
}

void handleGameOver() {
    gameOver = true;
    lcd.clear();
    lcd.print("Game Over");
    
    lcd.setCursor(0,1);
    lcd.print("SCORE: ");
    lcd.print(score);
}
