int sensorPin = 0;
const int FSR_PIN = A1;
const float VCC = 4.98;
const float R_DIV = 3230.0;
float userWeight = 0;
bool isAuth = false;
int breakTime = 0;
int authTime = 0;
bool flag = true;
int sessionTime = 0;

void setup()
{
    Serial.begin(9600);
    pinMode(13, OUTPUT);
    pinMode(FSR_PIN, INPUT);
}

void loop()
{
    int reading = analogRead(sensorPin);
    float voltage = reading * 5.0;
    voltage /= 1024.0;
    float temperatureC = (voltage - 0.5) * 33;


    int fsrADC = analogRead(FSR_PIN);
    float fsrV = fsrADC * VCC / 1023.0;
    float fsrR = R_DIV * (VCC / fsrV - 1.0);
    float force;
    float fsrG = 1.0 / fsrR;
    if (fsrR <= 600)
        force = (fsrG - 0.00075) / 0.0000032639;
    else
        force = fsrG / 0.00000642857;

    //Serial.println(String(force) + " kg");

    if (temperatureC > 36.7 && flag == true)
    {
        Serial.println("User overheated.");
        flag = false;
    }

    if (temperatureC < 36.7 && flag == false)
    {
        flag = true;
        Serial.println("User temp normal.");
    }

    if (force > 50.0 && isAuth == false)
        authTime++;
    else
        authTime = 0;

    if (force > 50.0 && authTime == 7)
    {
        userWeight = force;
        isAuth = true;
        Serial.println("User is authorized. Weight fixed.");
    }

    if (isAuth)
        sessionTime++;

    if (sessionTime % 15 == 0 && isAuth == true)
        Serial.println("It's time to pause!");

    if ((force < userWeight - 5.0 || force > userWeight + 5.0) && isAuth == true)
        breakTime++;
    else if (force > userWeight - 3.0 && force < userWeight + 3.0)
        breakTime = 0;

    if (isAuth == true && breakTime == 10)
    {
        if (force < userWeight - 3.0 || force > userWeight + 3.0)
        {
            Serial.println("Block System");
            Serial.println("Last session time: " + String(sessionTime) + " seconds");
            sessionTime = 0;
            isAuth = false;
            breakTime = 0;
        }
    }

    delay(1000); // Wait for 1000 millisecond(s)

}
