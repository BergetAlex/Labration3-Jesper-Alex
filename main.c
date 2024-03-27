#include <LiquidCrystal.h>

#define MaxCars 3 // Max antal bilar

LiquidCrystal lcd(12, 11, 5, 4, 3, 2); // Deklarera lcd displayen

void handleGameOver(); // Deklarera handleGameOver funktionen
void displayLevel(); // Deklarera displayLevel funktionen
void introSequence(); // Deklarera introSequence funktionen
void startGame(); // Deklarera startGame funktionen
void handleGame(); // Deklarera handleGame funktionen
void moveCar2(); // Deklarera moveCar2 funktionen
bool canSpawnCar2(); // Deklarera canSpawnCar2 funktionen


byte car1[8] = { // Skapar en byte array för bil1
  B00000,
  B00000,
  B00010,
  B11101,
  B11101,
  B00010,
  B00000,
  B00000
};

byte car2[8] = { // skapar en byte array för bil2
  B00000,
  B00000,
  B01000,
  B10111,
  B10111,
  B01000,
  B00000,
  B00000
};

byte spawn1[8] = { // skapar en byte array för tunnlar
  B11111, 
  B00011,
  B00001,
  B00001,
  B00001,
  B00001,
  B00011,
  B11111
};

typedef struct car { // Skapar en struct för bilarna
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
unsigned long car2Speed = 500; //hur snabbt bil 2 ska röra sig
unsigned long int car2recentMove; //sparar bil 2's senaste rörelse

unsigned long int introCarSpeed; //hur snabbt intro bilarna ska röra sig
//----------------------------------------------

//variabler för lcdn
const int menuSize = 2; // Storlek på menyn
const int lcdColumn = 16; // Antal kolumner på lcdn
//----------------------------------------------

//variabler för karaktärerna
const int carChar = 7; // Karaktär för bil 1
const int car2Char = 6; // Karaktär för bil 2
const int spawn1Char = 8; // Karaktär för tunnlar
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
  lcd.createChar(carChar, car1); // Skapar en ny karaktär för bil1
  lcd.createChar(car2Char, car2);  // Skapar en ny karaktär för bil2
  lcd.createChar(spawn1Char, spawn1); // Skapar en ny karaktär för tunnlar

  pinMode(buttonSwitchRightLane, INPUT); // Sätt höger knapp som input
  pinMode(buttonSwitchLeftLane, INPUT); // Sätt vänster knapp som input
  randomSeed(analogRead(0));

  for(int i = 0; i < MaxCars; i++) { // Sätter all bilar som inte är närvarande
    car[i].lastMove = 0;
    car[i].isPresent = false;
  }
}

void loop() { // Main loop
  lcd.clear();
  menu();
}


void startGame() { // Starta spelet
  isPlaying = true; 
  
  while (isPlaying) { // Medans spelet körs gå in i handleGame
    handleGame();
  }
}

void putCarRightLane() { // Flytta bilen till höger fil
  lcd.setCursor(1, 1);
  lcd.write(carChar);
  lcd.setCursor(1, 0);
  lcd.print(" ");
  
  isCarRightLane = true;
}

void putCarLeftLane() { // Flytta bilen till vänster fil
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
      
    if ((millis() - car[i].lastMove) > (unsigned long)car2Speed) { // Kolla om det är dags att flytta bilen
        lcd.setCursor(car[i].x, car[i].y);
        lcd.print(" ");
        car[i].x--;
        lcd.setCursor(car[i].x, car[i].y);
        lcd.write(car2Char);

        if (car[i].x == -1) { // När bilen når slutet av skärmen så tas den bort och score ökar
            score++;
            lcd.setCursor(car[i].x, car[i].y);
            lcd.print(" ");
            car[i].isPresent = false;
            car[i].lastMove = millis();
            
        }
        else
            car[i].lastMove += car2Speed; // Incrementera tiden för senaste rörelsen
      }
    }
  }
}

bool canSpawnCar2() { // Kolla om det är dags att spawna bil 2
  if((millis() - timeSinceCarSpawned) > timeBetweenCars) {
    return true;
  }
  return false;
}
  

//Funktion för att spawna bil 2
void spawnCar2() {
 if (canSpawnCar2() && car2Counter != level) { // Kollar om vi kan spawna bil 2
    
    for(int i = 0; i < MaxCars; i++) {
      
      if(!car[i].isPresent) { // Om bilen inte är närvarande så spawna den
        car[i].x = 12;
        car[i].y = random(0, 2);
        lcd.setCursor(car[i].x, car[i].y);
        lcd.write(car2Char);
        car[i].isPresent = true;
        car[i].lastMove = millis(); // Uppdatera tiden för senaste rörelsen
        timeSinceCarSpawned = millis(); // Uppdatera tiden sedan bilen spawnades
        car2Counter++; // Incrementera antalet bilar
        break;
      }
    }
 }
}

//Funktion för att hantera spelet
void handleGame() {
  while(!gameOver){ // Medans spelet inte är över
    
    displayLevel(); // Visa level och score

    if (digitalRead(buttonSwitchLeftLane) == HIGH && isCarRightLane) { 
        putCarLeftLane(); // Flytta till vänster fil
    }
    if (digitalRead(buttonSwitchRightLane) == HIGH && !isCarRightLane) {
        putCarRightLane(); // Flytta till höger fil
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

void uppdateHighScore(){ // Uppdaterar highscore om det behövs
    if(score > highScore){
        highScore = score;
    }
}
void handleGameOver() { // Hantera Game Over
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

void menu() { // Meny
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

void initialiseLevel() { // Initierar leveln och ökar svårighetsgraden
    
    for(int i = 0; i < MaxCars; i++) {
        car[i].x = 0; // Sätter bilarna till startpositionen
        car[i].y = 0;
        car[i].lastMove = 0;
        car[i].isPresent = false;
    }
    
    timeBetweenCars = car2Speed * 4; // Uppdaterar tiden mellan bilarna
    amountOfCars += level; // Ökar antalet bilar
    car2Speed = 350 - (level * 25); // Ökar hastigheten för bil 2
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Level: ");
    lcd.print(level);
    delay(1000);
    lcd.clear();
    car2Counter = 0;
    temp = level + score;
}

void displayLevel(){ // Printar alla statiska saker på skärmen, som level, score och tunnlarna (spawn1)
    if(score >= 10){ // Fixar så vi printar score på rätt plats
        lcd.setCursor(14, 0);
        lcd.print(score);
    }else{
        lcd.setCursor(15, 0);
        lcd.print(score);
    }
    if(level >= 10){ // Fixar så vi printar level på rätt plats
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
    if(isCarRightLane){
        lcd.setCursor(1, 1);
        lcd.write(carChar);
    }
    else{
        lcd.setCursor(1, 0);
        lcd.write(carChar);
    }
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
    for(int i = 0; i < 2; i++) { // Flyttar bara intro bilarna
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
 // När bilarna nått slutet av skärmen så sätt deras position till 0 och sätt isPresent till false
 for(int i = 0; i < 2; i++) {
    car[i].x = 0;
    car[i].y = 0;
    car[i].isPresent = false;
 }
}

void introSequence() {
    spawnCar2Intro();  
      // Flyttar bilarna tills de når slutet av skärmen
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