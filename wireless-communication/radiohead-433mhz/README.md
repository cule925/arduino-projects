# RADIOHEAD BIBLIOTEKA I 433 MHZ MODUL

[RF 433 MHz odašiljač i prijemnik modul](https://www.alldatasheet.com/datasheet-pdf/view/1452032/ETC/433MHZ.html) je jeftin par modula koji se koristi u jednostavnim Arduino projektima gdje je potrebna bežična komunikacija. Moduli koriste amplitudnu modulaciju kako bi odašiljali i primali informacije. Kako bi olakšali slanje i primanje paketa podataka, preporučuje se koristiti *RadioHead.h* biblioteku koja će podatke enkapsulirati i dekapsulirati u pakete i upravljati RF modulima. Funkcije koje se mogu koristiti se nalaze [ovdje](https://www.airspayce.com/mikem/arduino/RadioHead/classRH__ASK.html).

Gotovi programi se nalaze u direktoriju [*transmitter*](transmitter) za odašiljač i [*receiver*](receiver) za prijemnik. Odašiljač šalje dvije poruke naizmjenično u intervalu od 1 sekunde dok prijemnik prima poruke i ispisuje ih na serijsku vezu. Datoteku odašiljača je potrebno prevesti i prebaciti na Arduino NANO pločicu na kojoj je spojen odašiljač, a datoteku prijemnika potrebno je prevesti i prebaciti na Arduinu NANO pločicu na kojoj je spojen prijemnik.

**Preporuka je koristiti zalemljene antene (obična žica) duljine 34.6 cm za veći raspon komunikacije.**

## Shema spojeva za RF module

```
        +--------------+          +--------------+                +--------------+          +--------------+
        | ARDUINO   5V +----------+ 5V    TX     |                | ARDUINO   5V +----------+ 5V    RX     |
        | NANO         |          |       MODULE |                | NANO         |          |       MODULE |
        |          D12 +----------+              |                |          D11 +----------+              |
        |              |          |              |                |              |          |              |
        |          GND +----------+ GND          |                |          GND +----------+ GND          |
        +--------------+          +--------------+                +--------------+          +--------------+
```

