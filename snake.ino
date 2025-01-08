#include <FastLED.h>

// Настройки матрицы WS2812
#define LED_PIN     6       // Пин подключения матрицы WS2812
#define NUM_LEDS    64      // 8x8 матрица = 64 светодиодов
#define BRIGHTNESS  50      // Яркость матрицы
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS]; // Массив для хранения состояния каждого светодиода на матрице

// Пины для кнопок управления
#define BUTTON_UP    2
#define BUTTON_DOWN  3
#define BUTTON_LEFT  4
#define BUTTON_RIGHT 5
#define BUTTON_RESTART 7   // Кнопка для перезапуска игры

// Структура для хранения координат змейки
struct SnakeSegment {
  int x, y;  // Координаты сегмента змейки на матрице
};

SnakeSegment snake[NUM_LEDS]; // Массив для хранения координат всех сегментов змейки
int snakeLength = 1;          // Начальная длина змейки
int appleX = -1, appleY = -1; // Координаты яблока

// Направление змейки
enum Direction { UP, DOWN, LEFT, RIGHT };
Direction dir = RIGHT;  // Начальное направление змейки — вправо

// Очки
int score = 0;

void setup() {
  // Инициализация последовательного порта для вывода информации о счете
  Serial.begin(9600);

  // Инициализация матрицы светодиодов
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);  // Установка яркости

  // Инициализация пинов кнопок
  pinMode(BUTTON_UP, INPUT_PULLUP);
  pinMode(BUTTON_DOWN, INPUT_PULLUP);
  pinMode(BUTTON_LEFT, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT, INPUT_PULLUP);
  pinMode(BUTTON_RESTART, INPUT_PULLUP);

  // Начальная позиция змейки
  snake[0].x = 1;
  snake[0].y = 1;
  applePosition();  // Генерация начальной позиции яблока
}

void loop() {
  updateDirection();  // Проверка нажатия кнопок и обновление направления
  moveSnake();        // Движение змейки
  displayGame();      // Отображение состояния игры на матрице
  delay(100);         // Задержка для контроля скорости игры
}

// Функция для перемещения змейки
void moveSnake() {
  // Сдвигаем каждый сегмент змейки в массиве на одно место
  for (int i = snakeLength - 1; i > 0; i--) {
    snake[i] = snake[i - 1];
  }

  // Перемещаем голову змейки в направлении, заданном переменной 'dir'
  switch (dir) {
    case UP:    snake[0].y--; break;
    case DOWN:  snake[0].y++; break;
    case LEFT:  snake[0].x--; break;
    case RIGHT: snake[0].x++; break;
  }

  // Проверка на столкновение с границами игрового поля
  if (snake[0].x < 0 || snake[0].x >= 16 && snake[0].y < 0 || snake[0].y >= 16) {
    gameOver();  // Игра завершена
  }

  // Проверка на столкновение с собственным хвостом
  for (int i = 1; i < snakeLength; i++) {
    if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
      gameOver();  // Игра завершена
    }
  }

  // Проверка на поедание яблока
  if (snake[0].x == appleX && snake[0].y == appleY) {
    score++;      // Увеличиваем очки
    snakeLength++; // Увеличиваем длину змейки
    applePosition(); // Генерация нового яблока
  }
}

// Функция для генерации случайных координат для яблока
void applePosition() {
  appleX = random(0, 16);  // Случайная координата X яблока
  appleY = random(0, 16);  // Случайная координата Y яблока
}

// Функция для отображения игры на матрице WS2812
void displayGame() {
  fill_solid(leds, NUM_LEDS, CRGB::Black); // Очистка экрана (выключаем все светодиоды)

  // Отображаем каждый сегмент змейки на экране
  for (int i = 0; i < snakeLength; i++) {
    leds[snake[i].y * 16 + snake[i].x] = CRGB::Green;  // Зеленый для змейки
  }

  // Отображаем яблоко на экране
  leds[appleY * 16 + appleX] = CRGB::Red; // Красный для яблока

  FastLED.show(); // Обновляем матрицу, чтобы отобразить изменения

  // Выводим текущий счет в последовательный порт
  Serial.print("Score: ");
  Serial.println(score);
}

// Функция для обновления направления движения змейки
void updateDirection() {
  // Проверка нажатия кнопок и изменение направления змейки
  if (digitalRead(BUTTON_UP) == LOW && dir != DOWN) {
    dir = UP;  // Если кнопка вверх нажата, змейка идет вверх
  }
  if (digitalRead(BUTTON_DOWN) == LOW && dir != UP) {
    dir = DOWN;  // Если кнопка вниз нажата, змейка идет вниз
  }
  if (digitalRead(BUTTON_LEFT) == LOW && dir != RIGHT) {
    dir = LEFT;  // Если кнопка влево нажата, змейка идет влево
  }
  if (digitalRead(BUTTON_RIGHT) == LOW && dir != LEFT) {
    dir = RIGHT;  // Если кнопка вправо нажата, змейка идет вправо
  }
}

// Функция для завершения игры и перезапуска
void gameOver() {
  Serial.print("Game Over! Score: ");
  Serial.println(score);
  delay(1000);  // Задержка перед перезапуском игры

  // Ожидаем нажатия кнопки для перезапуска игры
  while (digitalRead(BUTTON_RESTART) == HIGH) {
    // Ждем нажатие кнопки для перезапуска
  }
  restartGame();  // Перезапускаем игру
}

// Функция для перезапуска игры
void restartGame() {
  score = 0;         // Сбрасываем счет
  snakeLength = 1;   // Сбрасываем длину змейки
  snake[0].x = 0;    // Начальная позиция головы змейки
  snake[0].y = 0;
  applePosition();   // Генерируем новое яблоко
  dir = RIGHT;       // Направление змейки на старте — вправо
}
