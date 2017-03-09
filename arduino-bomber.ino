#include <Event.h>
#include <Timer.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
int y = 0;

int bx = 0;
int by = 0;

int state = 0;
int prevState = 0;

int score = 0;

int countfirst = 0;
int countsecond = 0;

int timerid = 0;

Timer t;

byte man[8] = {
  0b01110,
  0b01010,
  0b01110,
  0b00100,
  0b11111,
  0b00100,
  0b01010,
  0b10001
};

byte bomb[8] = {
  0b00010,
  0b00100,
  0b01110,
  0b11111,
  0b11111,
  0b11111,
  0b01110,
  0b00000
};

int clickLCD()
{
  int x = analogRead(0);

  if(x > 1000) return 0; //brak
  if(x > 700) return 1; //select
  if(x > 400) return 2; //left
  if(x > 250) return 3; //down
  if(x > 100) return 4; //up
  if(x >= 0) return 5; //right

  return 0;
}

void createBomb() {
  by = random(2);
  bx = 14;

  if(by == 0)
    countfirst++;
  else if(by == 1)
    countsecond++;

  if(countfirst > 1) {
    countfirst = 0;
    by = 1;
  }
  else if(countsecond > 1) {
    countsecond = 0;
    by = 0;
  }

  lcd.setCursor(bx, by);
  lcd.write(byte(1));
}

void moveBomb() {
  lcd.setCursor(bx, by);
  lcd.print(' ');
  bx--;
  lcd.setCursor(bx, by);
  lcd.write(byte(1));

  if(bx == 0 && by == y) {
    lcd.clear();
    lcd.setCursor(4, 0);
    lcd.print("You lose!");
    delay(5000);

    //RESET ARDUINO
    asm volatile (" jmp 0");
  }
}

void destroyBomb() {
  lcd.setCursor(0, by);
  lcd.print(' ');

  createBomb();
}

void printScore(int score) {
  if(score < 10)
  {
    lcd.setCursor(15, 0);
    lcd.print("0");
    lcd.setCursor(15, 1);
    lcd.print(score);   
  }

  else
  { 
    lcd.setCursor(15, 0);
    lcd.print(score);
    lcd.setCursor(15, 1);
    lcd.print(score % 10);  
  }
  
  lcd.setCursor(0, y);
}

void setup() {
  Serial.begin(9600);

  lcd.createChar(0, man);
  lcd.createChar(1, bomb);
  lcd.begin(16, 2);
  lcd.setCursor(4, 0);
  lcd.write(byte(1));
  lcd.print("BOMBER");
  lcd.write(byte(1));
  while(analogRead(0) > 1000) {}
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.write(byte(0));
  printScore(0);

  createBomb();

  timerid = t.every(350, moveBomb);
}

void loop() {
  t.update();
  
  state = clickLCD();

  if(bx < 0) {
    score++;

    if(score == 50) {
      lcd.clear();
      lcd.setCursor(4, 0);
      lcd.print("You win!");
      delay(5000);
  
      //RESET ARDUINO
      asm volatile (" jmp 0");
    }

    printScore(score);

    destroyBomb();

    t.stop(timerid);
    timerid = t.every(350 - (score * 6), moveBomb);
  }
  
  if(state != prevState)
  { 
    prevState = state;
    
    lcd.setCursor(0, y);
    lcd.print(' ');
  
    switch(state)
    {
      case 5:
        y++;

        if(y > 1)
          y = 1;
        break;
      case 2:
        y--;

        if(y < 0)
          y = 0;
        break;
      case 4:
        y--;
        
        if(y < 0)
          y = 0;
        break;
      case 3:
        y++;
        
        if(y > 1)
          y = 1;
        break;
    }
  
    lcd.setCursor(0, y);
    lcd.write(byte(0));
  }
}
