#include <Arduino.h>

class LCD
{
public:
    LCD(int rs, int enable, const int dataPins[8]);

    void begin();
    void begin(int cols, int rows);

    void clear();
    void print(const String &str);
    void setCursor(int col, int row);

    int _rs;
    int _enable;
    int _dataPins[8];

private:
    uint8_t _cols;
    uint8_t _rows;

    void sendCommand(uint8_t cmd);
    void writeChar(uint8_t value);
    void shortDelay();
};