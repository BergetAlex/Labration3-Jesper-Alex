#include <LiquidCrystal.h>


LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void handleGameOver(); // Declare the handleGameOver function
void displayLevel(); // Declare the displayLevel function
void introSequence(); // Declare the introSequence function

//TODO: Fixa så att score inte skriver över en 

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

const int buttonSwitchRightLane = 6;
const int buttonSwitchLeftLane = 7;

int currentCar2XPos,currentCar2YPos, introCarXPos, introCarYPos; //sparar bilarnas position
unsigned long car2Speed = 500; //hur snabbt bil 2 ska röra sig
unsigned long int car2recentMove; //sparar bil 2's senaste rörelse

unsigned long int introCarSpeed; //hur snabbt intro bilarna ska röra sig
unsigned long int introCarRecentMove; //sparar intro bilarnas senaste rörelse

const int menuSize = 2;
const int lcdColumn = 16;
bool isCar2Present = false;
const int car2Char = 6;
const int carChar = 7;
const int spawn1Char = 8;

bool gameOver = false;
bool isPlaying = false;
bool isCarRightLane = true;
int score = 0;
int highScore = 0;
int level = 1;
int amountOfCars = 2;

void setup() {
  lcd.begin(16, 2);
  lcd.createChar(carChar, car); // Create a new character
  lcd.createChar(car2Char, car2);  // Create a new character
  lcd.createChar(spawn1Char, spawn1); // Create tunel1

  pinMode(buttonSwitchRightLane, INPUT_PULLUP);
  pinMode(buttonSwitchLeftLane, INPUT_PULLUP);
  randomSeed(analogRead(0));
}

void startGame(); // Declare the startGame function

void handleGame(); // Declare the handleGame function

void loop() {
  lcd.clear();
  menu();
}


void startGame() {
  isPlaying = true;
  isCar2Present = false; //Resetar denna variabel så att bil 2 kan spawna som vanligt vid ny omgång
  
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

    if ((millis() - car2recentMove) > (unsigned long)car2Speed) { // check if it is time to move the car
        lcd.setCursor(currentCar2XPos, currentCar2YPos);
        lcd.print(" ");
        currentCar2XPos--;
        lcd.setCursor(currentCar2XPos, currentCar2YPos);
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
            car2recentMove += car2Speed; // Increment car2recentMove by car2Speed
    }
}

//Funktion för att försöka spawna bil 2
void spawnCar2() {
    if(!isCar2Present) { // Om bilen inte redan är på skärmen
        int positionX = 12;
        int positionY = random(0, 2);
        lcd.setCursor(positionX, positionY);
        lcd.write(car2Char);
        isCar2Present = true;
        currentCar2XPos = positionX;
        currentCar2YPos = positionY;
        car2recentMove = millis();

    }   
}

//Funktion för att hantera spelet
void handleGame() {
  while(!gameOver){
    
    displayLevel();

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
    
    if(score == amountOfCars){ //kollar om det är dags för level up
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
    level = 1;
    uppdateHighScore(); //uppdatera highscore om det behövs
    score = 0;
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
    amountOfCars = level * 2;
    car2Speed = 500 - (level * 50);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Level: ");
    lcd.print(level);
    delay(1000);
    lcd.clear();
    if(level == 10){
        gameOver = true;
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("You Win!");
        lcd.setCursor(0,1);
        lcd.print("Score: ");
        lcd.print(score);
        delay(3000);
        menu();
    }
}

void displayLevel(){ //printar alla statiska saker på skärmen, som level, score och tunnlarna (spawn1)
    if(score >= 10){ //fixar så vi printar score på rätt plats
        lcd.setCursor(14, 0);
        lcd.print(score);
    }else{
        lcd.setCursor(15, 0);
        lcd.print(score);
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
    //spawnr bil 1 (intro car variablar)
    //spawnar bil 2 (current car variablar)
    introCarXPos = 18;
    introCarYPos = 0;

    currentCar2XPos = 18;
    currentCar2YPos = 1;

        lcd.setCursor(introCarXPos, introCarYPos); //spawnar bil 1
        lcd.write(car2Char);



        lcd.setCursor(currentCar2XPos, currentCar2YPos); //spawnar bil 2
        lcd.write(car2Char);
        introCarSpeed = 300;
        introCarRecentMove = millis();
}



//fixa så whilen kollar om X postionen istället för boolen, sparar en variabel
void moveCar2Intro() { //funktion som flyttar på bilarna för intro sequencen
  while(introCarXPos > 0){
    if ((millis() - introCarRecentMove) > introCarSpeed) { // check if it is time to move the car
        //flyttar bil 1
        lcd.setCursor(currentCar2XPos, currentCar2YPos);
        lcd.print(" ");
        currentCar2XPos--;
        lcd.setCursor(currentCar2XPos, currentCar2YPos);
        lcd.write(car2Char);

        //flyttar bil 2  
        lcd.setCursor(introCarXPos, introCarYPos);
        lcd.print(" ");
        introCarXPos--;
        lcd.setCursor(introCarXPos, introCarYPos);
        lcd.write(car2Char);        
        
        
        if(introCarXPos == 0) { //när bilarna når slutet av skärmen så tas den bort
            lcd.setCursor(introCarXPos, introCarYPos);
            lcd.print(" ");
            lcd.setCursor(currentCar2XPos, currentCar2YPos);
            lcd.print(" ");
        }
        else
            introCarRecentMove += introCarSpeed; // Increment car2recentMove by car2Speed
    }   
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