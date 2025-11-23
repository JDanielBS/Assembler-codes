#include "LCD.h"

/**
 * Constructor de la clase LCD en modo 8-bit.
 */
LCD::LCD(int rs, int enable, const int dataPins[8])
    : _rs(rs), _enable(enable), _cols(0), _rows(0)
{
    for (int i = 0; i < 8; i++)
    {
        _dataPins[i] = dataPins[i];
    }
}

/**
 * Inicializa el LCD (por defecto 16x2).
 */
void LCD::begin()
{
    begin(16, 2);
}

/**
 * Inicializa los pines y envía la secuencia de inicio al LCD.
 */
void LCD::begin(int cols, int rows)
{
    _cols = cols;
    _rows = rows;

    pinMode(_rs, OUTPUT);
    pinMode(_enable, OUTPUT);
    for (int i = 0; i < 8; i++)
    {
        pinMode(_dataPins[i], OUTPUT);
    }

    shortDelay();

    if (_rows > 1)
        sendCommand(0x38);
    else
        sendCommand(0x30);

    sendCommand(0x0C);

    clear();

    sendCommand(0x06);
}

/**
 * Envía un comando al LCD (coloca bits en los pines D0..D7 y pulsa EN).
 */
void LCD::sendCommand(uint8_t cmd)
{
    digitalWrite(_rs, LOW);

    for (int i = 0; i < 8; i++)
    {
        bool bit = bitRead(cmd, i);
        digitalWrite(_dataPins[i], bit ? HIGH : LOW);
    }

    digitalWrite(_enable, HIGH);
    shortDelay();
    digitalWrite(_enable, LOW);

    delay(2);
}

/**
 * Limpia la pantalla LCD y posiciona el cursor en la posición inicial.
 */
void LCD::clear()
{
    sendCommand(0x01);
    delay(2);
}

/**
 * Posiciona el cursor en la columna y fila especificadas.
 * col y row comienzan en 0. Comprueba límites si begin no fue llamado.
 */
void LCD::setCursor(int col, int row)
{
    if (_cols == 0 || _rows == 0)
    {
    }

    uint8_t addr;
    switch (row)
    {
    case 0:
        addr = 0x80 + col;
        break;
    case 1:
        addr = 0xC0 + col;
        break;
    case 2:
        addr = 0x94 + col;
        break;
    case 3:
        addr = 0xD4 + col;
        break;
    default:
        return;
    }
    sendCommand(addr);
}

/**
 * Escribe un carácter en la pantalla LCD.
 */
void LCD::writeChar(uint8_t value)
{
    digitalWrite(_rs, HIGH);

    for (int i = 0; i < 8; i++)
    {
        bool bit = bitRead(value, i);
        digitalWrite(_dataPins[i], bit ? HIGH : LOW);
    }

    digitalWrite(_enable, HIGH);
    shortDelay();
    digitalWrite(_enable, LOW);

    delay(2);
}

/**
 * Imprime una cadena de texto en la pantalla LCD.
 */
void LCD::print(const String &str)
{
    for (size_t i = 0; i < str.length(); i++)
    {
        writeChar((uint8_t)str[i]);
    }
}

void LCD::shortDelay()
{
    delayMicroseconds(50);
}