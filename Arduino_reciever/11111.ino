const int trigPin1 = 9;
const int echoPin1 = 6;
const int trigPin2 = 5;
const int echoPin2 = 3;
const int for1 = 13;
const int rev1 = 7;
const int for2 = 11;
const int rev2 = 10;

void left() {
  digitalWrite(for1, HIGH);
  digitalWrite(rev2, HIGH);
  digitalWrite(for2, LOW);
  digitalWrite(rev1, LOW);
}

void right() {
  digitalWrite(for2, HIGH);
  digitalWrite(rev1, HIGH);
  digitalWrite(for1, LOW);
  digitalWrite(rev2, LOW);
}

void reverse() {
  digitalWrite(rev1, HIGH);
  digitalWrite(rev2, HIGH);
  digitalWrite(for1, LOW);
  digitalWrite(for2, LOW);
}

void straight() {
  digitalWrite(for1, HIGH);
  digitalWrite(for2, HIGH);
  digitalWrite(rev1, LOW);
  digitalWrite(rev2, LOW);
}

void stop() {
  digitalWrite(for1, LOW);
  digitalWrite(for2, LOW);
  digitalWrite(rev1, LOW);
  digitalWrite(rev2, LOW);
}

float Distance(int trigPin, int echoPin) {
  pinMode(trigPin, OUTPUT);
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  pinMode(echoPin, INPUT);

  float duration = pulseIn(echoPin, HIGH);
  float distance = duration * 0.034 / 2;
  return distance;
}

void setup() {
  pinMode(trigPin1, OUTPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(echoPin2, INPUT);
  pinMode(for1, OUTPUT);
  pinMode(for2, OUTPUT);
  pinMode(rev1, OUTPUT);
  pinMode(rev2, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  if (Serial.available() > 0) {
    char received = Serial.read();

    if (received == 'a') {
      // Move straight
      straight();
    } 
    else if (received == 'b') {
      while (received == 'b') {
        char newchar = Serial.read();
        // Activate the Sensor
        int x = Distance(trigPin2, echoPin2);
        if (x < 20) {
          // Turn left
          int delay1 = 0;
          for (int i = 0; i < 500; i++) {
            int x = Distance(trigPin2, echoPin2);
            if (x > 30) {
              break;
            }
            left();
            delay(i);
            delay1 += i;
          }
          // Move a little bit forward
          straight();
          delay(500);
          // Turn right with the same amount of delay
          right();
          delay(delay1);
          if (newchar != 'b') {
            break;
          }
        } 
        else {
          // Move straight
          straight();
          if (newchar != 'b') {
            break;
          }
        }

        // Check for other commands here
        if (Serial.available() > 0) {
          char additionalCommand = Serial.read();
          switch (additionalCommand) {
            case 'c':
              // Handle command 'c'
              stop();
              break;
            case 'd':
              // Handle command 'd'
              reverse();
              break;
            // Add more cases for additional commands if needed
            default:
              // Handle unknown commands or do nothing
              break;
          }
        }
      }
    }
    else {
      // Handle other commands or default behavior
      stop();
    }
  }
  delay(100); // Small delay to avoid overwhelming the serial buffer
}
