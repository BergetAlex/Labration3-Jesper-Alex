#include <LiquidCrystal.h>

#define MaxCars 3 // Max amount of cars on the screen

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void handleGameOver(); // Declare the handleGameOver function
void displayLevel(); // Declare the displayLevel function
void introSequence(); // Declare the introSequence function
void startGame(); // Declare the startGame function
void handleGame(); // Declare the handleGame function
void moveCar2();
bool canSpawnCar2();
//TODO: Fixa så att score inte skriver över en 

byte car1[8] = {
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

byte spawn1[8] = {
  B11111,
  B00011,
  B00001,
  B00001,
  B00001,
  B00001,
  B00011,
  B11111
};

typedef struct car {
  int x;
  int y;
  unsigned long int lastMove;
  bool isPresent;
}Car;

Car car [MaxCars]; // Skapar en array med 3 bilar

//knapparna pinar
const int buttonSwitchRightLane = 6;
const int buttonSwitchLeftLane = 7;
//----------------------------------------------


//variabler för bilarna
int currentCar2XPos,currentCar2YPos, introCarXPos, introCarYPos; //sparar bilarnas position
unsigned long car2Speed = 500; //hur snabbt bil 2 ska röra sig
unsigned long int car2recentMove; //sparar bil 2's senaste rörelse

unsigned long int introCarSpeed; //hur snabbt intro bilarna ska röra sig
unsigned long int introCarRecentMove; //sparar intro bilarnas senaste rörelse
//----------------------------------------------

//variabler för lcdn
const int menuSize = 2; 
const int lcdColumn = 16;
//----------------------------------------------

//variabler för karaktärerna
const int carChar = 7;
const int car2Char = 6;
const int spawn1Char = 8;
//----------------------------------------------

//variabler för spelets funktionalitet
bool isCar2Present = false;
bool gameOver = false;
bool isPlaying = false;
bool isCarRightLane = true;
int score = 0;
int highScore = 0;
int level = 1;
int amountOfCars = 2;
int car2Counter = 0;
unsigned long int timeSinceCarSpawned = 0; //sparar tiden sedan bil framför spawnades
unsigned long int timeBetweenCars = 2200; //tiden mellan bilarna
int temp = level + score;
//----------------------------------------------

void setup() {
  lcd.begin(16, 2);
  lcd.createChar(carChar, car1); // Create a new character
  lcd.createChar(car2Char, car2);  // Create a new character
  lcd.createChar(spawn1Char, spawn1); // Create tunel1

  pinMode(buttonSwitchRightLane, INPUT);
  pinMode(buttonSwitchLeftLane, INPUT);
  randomSeed(analogRead(0));

  for(int i = 0; i < MaxCars; i++) {
    car[i].lastMove = 0;
    car[i].isPresent = false;
  }
}

void loop() {
  lcd.clear();
  menu();
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
    for(int i = 0; i < MaxCars; i++) {
        if(car[i].isPresent) {
            if(isCarRightLane){ //Bil 1 är på 1,0
                if(car[i].x == 1 && car[i].y == 1)
                    return 1;
                else    
                    return 0;
            }
            if(!isCarRightLane) { //Bil 1 är på 1,1
                if(car[i].x == 1 && car[i].y == 0)
                    return 1;
                else
                    return 0;
          }
        }
    }
} 


//Funktion för att flytta bil 2
void moveCar2() {

  for(int i = 0; i < MaxCars; i++) {
    if(car[i].isPresent) {
      
    if ((millis() - car[i].lastMove) > (unsigned long)car2Speed) { // check if it is time to move the car
        lcd.setCursor(car[i].x, car[i].y);
        lcd.print(" ");
        car[i].x--;
        lcd.setCursor(car[i].x, car[i].y);
        lcd.write(car2Char);

        if (currentCar2XPos == 0) { //när bilen når slutet av skärmen så tas den bort och score ökar
            score++;
            lcd.setCursor(currentCar2XPos, currentCar2YPos);
            lcd.clear();
            isCar2Present = false;
            car2recentMove = millis();
            if (isCarRightLane)
                putCarRightLane();
            else
                putCarLeftLane();
        }
        else
            car[i].lastMove += car2Speed; // Increment car lastMove by car2Speed
      }
    }
  }
}

bool canSpawnCar2() {
  if((millis() - timeSinceCarSpawned) > timeBetweenCars) {
    return true;
  }
  return false;
}
  

//Funktion för att försöka spawna bil 2
void spawnCar2() {
 if (canSpawnCar2() && car2Counter != level) {
    
    for(int i = 0; i < MaxCars; i++) {
      
      if(!car[i].isPresent) {
        car[i].x = 12;
        car[i].y = random(0, 2);
        lcd.setCursor(car[i].x, car[i].y);
        lcd.write(car2Char);
        car[i].isPresent = true;
        car[i].lastMove = millis();
        timeSinceCarSpawned = millis(); // Update the time of the last spawn
        car2Counter++;
        break;
      }
    }
 }
}

//Funktion för att hantera spelet
void handleGame() {
  while(!gameOver){
    
    displayLevel();

    if (digitalRead(buttonSwitchLeftLane) == HIGH && isCarRightLane) { 
        putCarLeftLane(); // Change to left lane
    }
    if (digitalRead(buttonSwitchRightLane) == HIGH && !isCarRightLane) {
        putCarRightLane(); // Change to right lane
    }

      spawnCar2();
      moveCar2();

      if(checkCollision()){ // Om bilen kolliderar
       handleGameOver(); // Hantera Game over
     }
    
    if(temp == score){ //kollar om det är dags för level up
        level++;
        initialiseLevel();
        displayLevel();    
    }
  }
  
  isPlaying = false;
  gameOver = false;
  menu();

}

void uppdateHighScore(){
    if(score > highScore){
        highScore = score;
    }
}
void handleGameOver() {
    gameOver = true;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Game Over");
    lcd.setCursor(0,1);
    lcd.print("Score: ");
    lcd.print(score);
    delay(3000);
    uppdateHighScore(); //uppdatera highscore om det behövs
    level = 1; //resetar level
    score = 0; //resetar score
}

void menu() {
    lcd.clear();
    while(!isPlaying){
      lcd.setCursor(0, 0);
      lcd.print("1. Start Game");
      lcd.setCursor(0, 1);
      lcd.print("2. High Score");
      lcd.setCursor(0, 2);

      if(digitalRead(buttonSwitchLeftLane) == HIGH){ //om knapp 1 trycks (starta spelet)

        introSequence(); //spelar intro sequencen           
        startGame();
      }
      
      if(digitalRead(buttonSwitchRightLane) == HIGH){ //om knapp 2 trycks (visa highscore)
        lcd.clear();
        lcd.setCursor(0, 0);
        uppdateHighScore();
        lcd.print("High Score: ");
        lcd.print(highScore);
        delay(3000);
        lcd.clear();
      }
  }
}

void initialiseLevel() { //initierar leveln och ökar svårighetsgraden
    
    for(int i = 0; i < MaxCars; i++) {
        car[i].x = 0; //sätter bilarna till startpositionen
        car[i].y = 0;
        car[i].lastMove = 0;
        car[i].isPresent = false;
    }
    
    timeBetweenCars = car2Speed * 4; //uppdaterar tiden mellan bilarna
    amountOfCars += level;
    car2Speed = 350 - (level * 25);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Level: ");
    lcd.print(level);
    delay(1000);
    lcd.clear();
    car2Counter = 0;
    temp = level + score;
}

void displayLevel(){ //printar alla statiska saker på skärmen, som level, score och tunnlarna (spawn1)
    if(score >= 10){ //fixar så vi printar score på rätt plats
        lcd.setCursor(14, 0);
        lcd.print(score);
    }else{
        lcd.setCursor(15, 0);
        lcd.print(score);
    }
    if(level >= 10){ //fixar så vi printar level på rätt plats
        lcd.setCursor(14, 1);
        lcd.print(level);
    }else{
        lcd.setCursor(15, 1);
        lcd.print(level);
    }

    lcd.setCursor(13,0);
    lcd.write(spawn1Char);
    lcd.setCursor(13,1);
    lcd.write(spawn1Char);
    if(isCarRightLane)
        putCarRightLane();
    else
        putCarLeftLane();
}

void spawnCar2Intro() { //funktion som spawnar 2 bilar för intro sequencen
    //spawna bilar i index 0 och 1
    for(int i = 0; i < 2; i++) {
        car[i].x = 18;
        car[i].y = i;
        lcd.setCursor(car[i].x, car[i].y);
        lcd.write(car2Char);
        car[i].isPresent = true;
        car[i].lastMove = millis();
    }
        introCarSpeed = 300; //sätt hastigheten för intro bilarna
}

//fixa så whilen kollar om X postionen istället för boolen, sparar en variabel
void moveCar2Intro() {
 while((car[0].x > 0 && car[1].x > 0)) {
    for(int i = 0; i < 2; i++) { // Only move the first two cars (intro cars)
      if(car[i].isPresent) {
        if ((millis() - car[i].lastMove) > introCarSpeed) {
          lcd.setCursor(car[i].x, car[i].y);
          lcd.print(" ");
          car[i].x--;
          lcd.setCursor(car[i].x, car[i].y);
          lcd.write(car2Char);

          if (car[i].x == 0) {
            lcd.setCursor(car[i].x, car[i].y);
            lcd.print(" ");
            car[i].isPresent = false;
            car[i].lastMove = 0;
          } else {
            car[i].lastMove += introCarSpeed;
          }
        }
      }
    }
 }
 // Reset cars after intro sequence
 for(int i = 0; i < 2; i++) {
    car[i].x = 0;
    car[i].y = 0;
    car[i].isPresent = false;
 }
}

void introSequence() {
    spawnCar2Intro();  
      //flyttar bilarna tills de når slutet av skärmen
    moveCar2Intro();
  
    for(int i = 3; i > 0; i--){
        lcd.clear();
        lcd.setCursor(8,1);
        lcd.print(i);
        delay(500);
        lcd.print(" ");
    }
    lcd.setCursor(8,1);
    lcd.print("GO!");
    delay(500);
    lcd.clear();
}