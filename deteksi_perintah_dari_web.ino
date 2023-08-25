#include <Adafruit_Fingerprint.h>

SoftwareSerial mySerial(2, 3);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

void setup() {
  Serial.begin(9600);
  delay(100);
  finger.begin(57600);

  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!\n");
  } else {
    Serial.println("Did not find fingerprint sensor :(\n");
    while (1) { delay(1); }
  }

  finger.getTemplateCount();
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n'); // Baca pesan dari serial
    command.trim(); // Hapus karakter whitespace tambahan
    
    if (command.length() > 0 && !isWhitespace(command[0])) {
      // Pisahkan string menjadi token
      char commandArray[command.length() + 1];
      command.toCharArray(commandArray, sizeof(commandArray));

      char *token;
      token = strtok(commandArray, " \n");

      int tokenCount = 0;
      char *action = NULL;
      char *option1 = NULL;
      char *option2 = NULL;

      while (token != NULL) {
        tokenCount++;

        if (tokenCount == 1) {
          action = token;
        } else if (tokenCount == 2) {
          option1 = token;
        } else if (tokenCount == 3) {
          option2 = token;
        }

        token = strtok(NULL, " \n");
      }

      if (action != NULL) {
        if (strcmp(action, "register") == 0) {
          Serial.println("Tempelkan sidik jari\n");
          delay(50);
          
          int id = atoi(option1); 
          if (id > 0 && id <= 127) {
            while (!getFingerprintEnroll(id));
          } else {
            Serial.println("ID harus antara 1 dan 127\n");
          }

        } else if (strcmp(action, "read") == 0) {
          Serial.println("Tempelkan sidik jari\n");
          delay(50);
          
          uint8_t sf = getFingerprintID();
          while (!sf);

          Serial.println(sf);

        } else if (strcmp(action, "delete") == 0) {
          Serial.println("Tempelkan sidik jari\n");
          delay(1000);
          Serial.println("ID anda adalah xxx sudah terhapus\n");
          delay(1000);

        }
      }
    }
  }
}

uint8_t getFingerprintEnroll(int id) {
  int p = -1;
  Serial.println("Waiting for enroll\n"); 
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK: Serial.println("Image taken\n"); break;
    // case FINGERPRINT_NOFINGER: Serial.println(".\n"); break;
    case FINGERPRINT_PACKETRECIEVEERR: Serial.println("Communication error\n"); break;
    case FINGERPRINT_IMAGEFAIL: Serial.println("Imaging error\n"); break;
    // default: Serial.println("Unknown error\n"); break;
    }
  }
  // OK success!
  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK: Serial.println("Image converted\n"); break;
    case FINGERPRINT_IMAGEMESS: return p;
    case FINGERPRINT_PACKETRECIEVEERR: return p;
    case FINGERPRINT_FEATUREFAIL: return p;
    case FINGERPRINT_INVALIDIMAGE: return p;
    default: return p;
  }

  Serial.println("Remove finger and tap againt\n"); delay(2000);
  
  p = 0;
  while (p != FINGERPRINT_NOFINGER) p = finger.getImage();
  Serial.println("ID \n"); Serial.println(id);
  p = -1;
  Serial.println("Place same finger again\n");

  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK: Serial.println("Image taken\n"); break;
    // case FINGERPRINT_NOFINGER: Serial.print(".\n"); break;
    case FINGERPRINT_PACKETRECIEVEERR: Serial.println("Communication error\n"); break;
    case FINGERPRINT_IMAGEFAIL: Serial.println("Imaging error\n"); break;
    // default: Serial.println("Unknown error\n"); break;
    }
  }
  // OK success!
  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK: Serial.println("Image converted\n"); break;
    case FINGERPRINT_IMAGEMESS: return p;
    case FINGERPRINT_PACKETRECIEVEERR: return p;
    case FINGERPRINT_FEATUREFAIL: return p;
    case FINGERPRINT_INVALIDIMAGE: return p;
    default: return p;
  }
  // OK converted!
  Serial.println("Creating model for #\n");  Serial.println(id);
  p = finger.createModel();
  if (p == FINGERPRINT_OK) Serial.println(p);
  else return p;
  
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) Serial.println("Data save to sensor\n");
  else return p;
  return true;
}

uint8_t getFingerprintID() {
  int p = -1;
  Serial.println("Waiting for check\n");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK: Serial.println("Image taken\n"); break;
    // case FINGERPRINT_NOFINGER: Serial.println(".\n"); break;
    case FINGERPRINT_PACKETRECIEVEERR: Serial.println("Communication error\n"); break;
    case FINGERPRINT_IMAGEFAIL: Serial.println("Imaging error\n"); break;
    // default: Serial.println("Unknown error\n"); break;
    }
  }
  
  Serial.println("\n");
  // OK success!
  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK: Serial.println("Image converted\n"); break;
    case FINGERPRINT_IMAGEMESS: return p;
    case FINGERPRINT_PACKETRECIEVEERR: return p;
    case FINGERPRINT_FEATUREFAIL: return p;
    case FINGERPRINT_INVALIDIMAGE: return p;
    default: return p;
  }
  
  // OK converted!
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!\n");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error\n");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match\n");
    return p;
  } else {
    // Serial.println("Unknown error\n");
    return p;
  }

  return finger.fingerID;
}