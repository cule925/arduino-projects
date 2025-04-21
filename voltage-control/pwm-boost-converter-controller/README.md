# PWM BOOST UPRAVLJAČ

PWM [boost pretvarač](https://en.wikipedia.org/wiki/Boost_converter) se koristi za pretvaranje nižeg istosmjernog napona u viši istosmjerni napon. Osnovni dijelovi su mu:

- tranzistorska sklopka (najčešće MOSFET)
	- upravljana uz pomoć PWM signala mikroupravljača, propušta struju iz izvora napajanja
- zavojnica
	- skladišti energiju kada je tranzistorska sklopka uključena i ispušta je kada je tranzistorska sklopka isljučena
- dioda
	- pruža put za struju iz zavojnice kada je tranzistorska sklopka isključena
- kondezator za izravnavanje napona
	- stabilizira izlazni napon

Potrebno je ispravno odabrati vrijednosti komponenata prilikom izgradnje boost pretvarača.

![Arduino boost pretvarač implementacija.](boost-pwm.drawio.png)

![PWM signal kojim se upravlja tranzistorska sklopka.](pwm-signal.jpg)

