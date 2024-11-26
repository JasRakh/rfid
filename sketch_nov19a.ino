#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>

// Пины для подключения RFID
#define RST_PIN 9   // RST
#define SS_PIN 10   // SDA (SS)

// Создаем объекты для LCD и RFID
LiquidCrystal_I2C lcd(0x27, 16, 2); // Адрес 0x27, LCD размер 16x2
MFRC522 rfid(SS_PIN, RST_PIN);      // RFID модуль

// Структура для хранения данных пользователя
struct User {
  String uid;       // UID карты
  String name;      // Имя пользователя
  String universityId; // ID университета
};

// Массив с информацией о картах
User userMap[] = {
  {"59C2A5FF", "Akbar Juraboev", "220901"},
  {"FE396C6A", "Jasur Rakhmanov", "220257"},
  {"91D49BFF", "Islombek Pulatov", "220358"},
  {"D4E8A89F", "Alisher Erkinov", "210655"},
  {"041BA6FF", "Azizbek Boboqulov", "221198"},
  {"CC5CA7FF", "Ismoil Salohiddinov", "220626"},
  {"A975A6FF", "Abdurashid Djumabaev", "210004"},
  {"F075A6FF", "Komiljon Qosimov", "220493"},
  {"7522A99F", "Javohirbek Khoshimov", "220899"},
  {"0C71A7FF", "Islombek Abdurahmonov", "221236"},
  {"05DA3C5F", "Feruza Khudoyberdieva", "221328"},
};

void setup() {
  // Инициализация серийного порта
  Serial.begin(9600);
  while (!Serial);

  // Инициализация SPI и RFID
  SPI.begin();
  rfid.PCD_Init();

  // Инициализация LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Scan your card...");
}

void loop() {
  // Проверяем наличие новой карты
  if (!rfid.PICC_IsNewCardPresent()) {
    return;
  }

  // Пробуем прочитать карту
  if (!rfid.PICC_ReadCardSerial()) {
    return;
  }

  // Считываем UID карты
  String uid = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) {
      uid += "0"; // Добавляем ведущий ноль
    }
    uid += String(rfid.uid.uidByte[i], HEX); // Преобразуем байт в HEX
  }
  uid.toUpperCase(); // Делаем буквы заглавными

  // Поиск информации о карте
  bool userFound = false;
  for (int i = 0; i < sizeof(userMap) / sizeof(User); i++) {
    if (userMap[i].uid == uid) {
      // Карта найдена, отображаем информацию
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(userMap[i].name);
      lcd.setCursor(0, 1);
      lcd.print("ID: " + userMap[i].universityId);

      // Отправка данных
      Serial.println(uid + "," + userMap[i].name + "," + userMap[i].universityId);

      userFound = true;
      break;
    }
  }

  if (!userFound) {
    // Карта не найдена
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Unknown Card");
    Serial.println(uid + ",Unknown,Unknown");
  }

  // Ожидаем некоторое время перед следующим чтением
  delay(3000);

  // Снова просим приложить карту
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Scan your card...");
}
