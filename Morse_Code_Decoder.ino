// Author: T TECH
// Project: Morse Code Decoder
// Lisence: MIT License 
// Date Created: 13/08/2026
// Date Modified: 16/08/2026
/* 
 _______              _______      _______      _______      __   __
/\__  __\            /\__  __\    /\    __\    /\   ___\    /\ \ /\ \
\/_/\ \_/            \/_/\ \_/    \ \   \_/_   \ \  \__/    \ \ \\_\ \
   \ \ \                \ \ \      \ \    __\   \ \  \       \ \  ___ \
    \ \ \                \ \ \      \ \   \_/_   \ \  \____   \ \ \_/\ \
     \ \_\                \ \_\      \ \______\   \ \______\   \ \_\\ \_\
      \/_/                 \/_/       \/______/    \/______/    \/_/ \/_/
*/

#include <LiquidCrystal.h>

// Array of morse code corresponding to alphabet  
const char* morse_code[26] = 
{
  ".-00", "-...", "-.-.", "-..0", ".000", "..-.", "--.0", "....", "..00", ".---", "-.-0", ".-..", "--00", "-.00", "---0", ".--.", "--.-", ".-.0", "...0", "-000", "..-", "...-", ".--0", "-..-", "-.--", "--.."
};

// Array of alphabet
const char alphabet[26] = 
{
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'
};

// Keep track of the previous time and currernt time recorded
double previous_time = 0;
double current_time = 0;

// Array holding the current character being decoded
char current_character[5] = 
{
  '0', '0', '0', '0', '\0'
};

// Index for current_character array
int current_character_index = 0;

// Keep track of when the last space was printed
bool last_character_space = true;

// Initialize LCD Display
LiquidCrystal lcd(47, 43, 39, 35, 31, 27);

// Characters for displaying on LCD
char lcd_character [16][2] = 
{
  {' ', ' '},
  {' ', ' '},
  {' ', ' '},
  {' ', ' '},
  {' ', ' '},
  {' ', ' '},
  {' ', ' '},
  {' ', ' '},
  {' ', ' '},
  {' ', ' '},
  {' ', ' '},
  {' ', ' '},
  {' ', ' '},
  {' ', ' '},
  {' ', ' '},
  {' ', ' '}
};

// Index for lcd_character array
int lcd_character_index [2] = 
{
  0,0
};

// Clear a row of the LCD
void lcd_clear (int row)
{
  for (int i = 0; i < 16; i++)
  {
    lcd_character[i][row] = ' ';
  }

  lcd_character_index[row] = 0;

  lcd_update();

  return;
}

// Add a character to a row of the LCD
void lcd_add_character (int row, char character)
{
  // If the row is full, shift it 1 spot to the left
  if (lcd_character_index[row] == 16)
  {
    for (int i = 0; i < 15; i++)
    {
      lcd_character[i][row] = lcd_character[i + 1][row];
    }
  }
  else 
  {
    lcd_character_index[row]++;
  }

  lcd_character[lcd_character_index[row] - 1][row] = character;

  return;
}

// Update the LCD by display the text
void lcd_update ()
{
  for (int j = 0; j < 16; j++)
  {
    for (int i = 0; i < 2; i++)
    {
      if (i == 0)
      {
        lcd.setCursor(j + 12, i);
        lcd.print(lcd_character[j][i]);
      }
      else 
      {
        lcd.setCursor(j, i);
        lcd.print(lcd_character[j][i]);
      }
    }
  }

  lcd.setCursor(0, 0);
  lcd.print("Morse Code:");

  return;
}

void setup() 
{
  // Setup serial port
  Serial.begin(9600);

  // Setup pins for buttons and buzzer
  pinMode(12, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  pinMode(51, OUTPUT); 

  // Setup LCD
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);

  return;
}

void loop()
{
  // Check if the button is beingn pressed
  if (digitalRead(12) == LOW)
  {
    previous_time = current_time;

    // Play the buzzer while the button is being pressed
    while (digitalRead(12) == LOW)
    {

      for (int i = 0; i < 59; i++)
      {
        digitalWrite(51, HIGH);
        delayMicroseconds(852);

        digitalWrite(51, LOW);
        delayMicroseconds(852);
      }
    }

    current_time = millis();
    
    // After the button is lifted calculate the time passed and add a dot or dash
    if (current_time - previous_time < 250)
    {
      current_character [current_character_index] = '.';
      lcd_add_character(0, '.');
      lcd_update();

    }
    else 
    {
      current_character [current_character_index] = '-';
      lcd_add_character(0, '-');
      lcd_update();
    }

    current_character_index ++;

    previous_time = current_time;
  }

  current_time = millis();

  // If enough time has passed for a character determine the letter
  if ((current_time - previous_time > 750 and current_time - previous_time < 4000) or current_character [3] != '0')
  {
    for (int i = 0; i < 26; i++)
    {
      if (strcmp(current_character, morse_code[i]) == 0)
      {
        // Display the character to the LCD
        lcd_add_character(1, alphabet[i]);
        lcd_update();
        last_character_space = false;
        i = 26;
      }
    }

    // Clear the current character
    current_character[0] = '0';
    current_character[1] = '0';
    current_character[2] = '0';
    current_character[3] = '0';
    current_character[4] = '\0';
    current_character_index = 0;
    
    // Clear the top row of the LCD
    lcd_clear(0);
  }
  // If enough time has passed for a space display it
  else if (current_time - previous_time > 4000 and last_character_space == false)
  {
    lcd_add_character(1, '_');
    lcd_update();

    last_character_space = true;

    current_character[0] = '0';
    current_character[1] = '0';
    current_character[2] = '0';
    current_character[3] = '0';
    current_character[4] = '\0';
    current_character_index = 0;
  }

  // If the second button is pressed clear the LCD
  if (digitalRead(7) == LOW)
  {
    lcd_clear(0);
    lcd_clear(1);

    last_character_space == true;
  }

  return;
}
