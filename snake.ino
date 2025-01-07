#include <FastLED.h>

// Настройки матрицы WS2812
#define LED_PIN     6       // Пин подключения матрицы WS2812
#define NUM_LEDS    64     // 16x16 матрица = 256 светодиодов
#define BRIGHTNESS  50      // Яркость матрицы
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS];

// Пины для кнопок управления
#define BUTTON_UP    2
#define BUTTON_DOWN  3
#define BUTTON_LEFT  4
#define BUTTON_RIGHT 5
#define BUTTON_RESTART 7   // Кнопка для перезапуска игры

// Структура для хранения координат змейки
struct SnakeSegment {
  int x, y;
};

SnakeSegment snake[NUM_LEDS]; // Массив для хранения координат змейки
int snakeLength = 1;          // Начальная длина змейки
int appleX = -1, appleY = -1; // Координаты яблока

// Направление змейки
enum Direction { UP, DOWN, LEFT, RIGHT };
Direction dir = RIGHT;

// Очки
int score = 0;

void setup() {
  // Инициализация последовательного порта
  Serial.begin(9600);

  // Инициализация матрицы
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);

  // Инициализация кнопок
  pinMode(BUTTON_UP, INPUT_PULLUP);
  pinMode(BUTTON_DOWN, INPUT_PULLUP);
  pinMode(BUTTON_LEFT, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT, INPUT_PULLUP);
  pinMode(BUTTON_RESTART, INPUT_PULLUP);

  // Инициализация змейки
  snake[0].x = 1; // Начальная позиция змейки
  snake[0].y = 1;
  applePosition();
}

void loop() {
  updateDirection();
  moveSnake();
  displayGame();
  delay(100);  // Задержка для управления скоростью игры
}

// Движение змейки
void moveSnake() {
  // Сдвигаем змейку по направлению
  for (int i = snakeLength - 1; i > 0; i--) {
    snake[i] = snake[i - 1];
  }

  // Перемещаем голову змейки в соответствующем направлении
  switch (dir) {
    case UP:
      snake[0].y--;
      break;
    case DOWN:
      snake[0].y++;
      break;
    case LEFT:
      snake[0].x--;
      break;
    case RIGHT:
      snake[0].x++;
      break;
  }

  // Проверка столкновений с границами
  if (snake[0].x < 0 || snake[0].x >= 16 || snake[0].y < 0 || snake[0].y >= 16) {
    gameOver();
  }
  

  // Проверка на столкновение с хвостом
  for (int i = 1; i < snakeLength; i++) {
    if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
      gameOver();
    }
  }

  // Проверка на поедание яблока
  if (snake[0].x == appleX && snake[0].y == appleY) {
    score++;
    snakeLength++;
    applePosition();
  }
}

// Генерация случайных координат для яблока
void applePosition() {
  appleX = random(0, 16);
  appleY = random(0, 16);
}

// Отображение игры на матрице WS2812
void displayGame() {
  // Очистка экрана
  fill_solid(leds, NUM_LEDS, CRGB::Black);

  // Отображаем змейку
  for (int i = 0; i < snakeLength; i++) {
    leds[snake[i].y * 16 + snake[i].x] = CRGB::Green;
  }

  // Отображаем яблоко
  leds[appleY * 16 + appleX] = CRGB::Red;

  // Обновляем матрицу
  FastLED.show();

  // Выводим информацию об очках в последовательный порт
  Serial.print("Score: ");
  Serial.println(score);
}

// Обновление направления движения змейки
void updateDirection() {
  if (digitalRead(BUTTON_UP) == LOW && dir != DOWN) {
    dir = UP;
  }
  if (digitalRead(BUTTON_DOWN) == LOW && dir != UP) {
    dir = DOWN;
  }
  if (digitalRead(BUTTON_LEFT) == LOW && dir != RIGHT) {
    dir = LEFT;
  }
  if (digitalRead(BUTTON_RIGHT) == LOW && dir != LEFT) {
    dir = RIGHT;
  }
}

// Завершение игры и перезапуск
void gameOver() {
  Serial.print("Game Over! Score: ");
  Serial.println(score);
  delay(1000);

  // Ожидаем нажатия кнопки для перезапуска
  while (digitalRead(BUTTON_RESTART) == HIGH) {
    // Ждем нажатие кнопки для перезапуска
  }
  restartGame();
}

// Функция для перезапуска игры
void restartGame() {
  score = 0;
  snakeLength = 1;
  snake[0].x = 0;
  snake[0].y = 0;
  applePosition();
  dir = RIGHT;
}
