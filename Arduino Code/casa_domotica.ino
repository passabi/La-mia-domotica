#include <Servo.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>
Servo servo_pr;
DHT dht(44, DHT11); //pin usato, tipo di sensore
LiquidCrystal_I2C lcd(0x27, 16, 2);  //0x27 dipende dal modulo i2c

#define bt Serial1

//camera 1  (bagno)
#define led_grandi_camera1 2      //rosso
#define led_piccoli_1_camera1 22  //grigio
#define led_piccoli_2_camera1 23  //verde

//camera 2  (cucina)
#define led_grandi_camera2 3
#define led_piccoli_1_camera2 24
#define led_piccoli_2_camera2 25

//camera 3  (studio)
#define led_grandi_camera3 4
#define led_piccoli_1_camera3 26
#define led_piccoli_2_camera3 27

//camera 4  (letto)
#define led_grandi_camera4 5
#define led_piccoli_1_camera4 28
#define led_piccoli_2_camera4 29

//corridoio
#define rosso_rgb1 7 //pwm
#define verde_rgb1 8 //pwm
#define blu_rgb1   9 //pwm

#define rosso_rgb2 40
#define verde_rgb2 41
#define blu_rgb2   42

//**********FUSI CON LA SERIE 1****************/
#define rosso_rgb3 10 //pwm
#define verde_rgb3 11 //pwm
#define blu_rgb3   12 //pwm

#define bianchi_pin_piccoli 30
#define rossi_pin_piccoli   31
#define verdi_pin_piccoli   32
#define blu_pin_piccoli     33
#define gialli_pin_piccoli  34

#define bianchi_pin_grandi  35
#define rossi_pin_grandi    36
#define verdi_pin_grandi    37
#define blu_pin_grandi      38
#define gialli_pin_grandi   39

//illuminazione led bassi esterno
#define led_giardino 47

//fontanella
#define pompa_acqua 51
#define led_pompa_acqua 53

//allarme
#define buzzer1 A7
#define buzzer2 A6

//INPUT
//fotoresistenze
#define fotoresistenza_1 A0
#define fotoresistenza_2 A1
#define led_rosso_sensore_luminosita A5
#define led_verde_sensore_luminosita A4

//sensore gas
#define sensore_gas A2
#define led_rosso_sensore_gas A13
#define led_verde_sensore_gas A12

//sensore fiamma
#define sensore_fiamma A3
#define led_rosso_sensore_fiamma A11
#define led_verde_sensore_fiamma A10

//sensore temperatura e umidità
#define led_rosso_sensore_dht11 A15
#define led_verde_sensore_dht11 A14
#define ventola 6

//sensore distanza
#define led_rosso_sensore_distanza A9
#define led_verde_sensore_distanza A8
#define echo 43
#define trig 45

//sensore movimento
#define sensore_movimento 46
#define led_rosso_sensore_movimento 49
#define led_verde_sensore_movimento 48

//dichiarazione variabili
char cmd = ' ';
int val_fotoresistenza_1 = 0;
int val_fotoresistenza_2 = 0;
unsigned long dt_gas, t_gas;
bool stato_gas = false;
int val_gas = 0;
unsigned long dt_fiamma, t_fiamma;
bool stato_fiamma = false;
int val_fiamma = 0;
int lettura_fiamma = 0;
unsigned long dt_dht11, t_dht11;
bool stato_dht11 = false;
int temperatura = 0;
int umidita = 0;
unsigned long dt_distanza, t_distanza;
bool stato_distanza = false;
long val_tempo = 0;
float val_distanza = 0;
int val_movimento = 0;
unsigned long dt_movimento, t_movimento;
bool stato_movimento = false;
unsigned long dt_allarme, t_allarme;
bool stato_allarme = false;

//dichiarazione variabili da impostare (ALLA FINE)
int luminosita_tramonto = 300 ; //da impostare
int tempo_apertura_porta_principale = 12;  //lasciare così
int soglia_gas = 160; //impostare alla fine perchè potrebbe dipendere dal carico di tensione
int tempo_led_allarme_gas = 75; //lampeggia il "led_rosso_sensore_gas"
int deboucing_gas = 5;
int soglia_fiamma = 800; //impostare alla fine perchè potrebbe dipendere dal carico di tensione
int tempo_led_allarme_fiamma = 75; //lampeggia il "led_rosso_sensore_fiamma"
int deboucing_fiamma = 5;
int soglia_dht11 = 32.50; //impostare alla fine
int tempo_led_allarme_dht11 = 75;//lampeggia il "led_rosso_sensore_DHT11"
int deboucing_dht11 = 0; //il valore della temperatura cambia dopo tanto quindi non c'è il rischio di valori che sbalzano
int soglia_distanza = 27; //impostare alla fine (valore ipotetico: 35)
int tempo_led_allarme_distanza = 75;
int deboucing_distanza = 0.5;
int tempo_led_allarme_movimento = 75;
int tempo_led_allarme = 20;
int tempo_led_allarme_logout = 20;
int w = 400; //delay lcd
int w_sensor = 200; //delay lcd sensori
int w_sensor_movimento = 100; //delay lcd sensore movimento
int tempo_logout_spegnimento = 550; //delay spegnimento domotica in seguito al logout
int tempo_logout_buzzer_segnalazione_sonora = 87; //delay segnalazione sonora buzzer in seguito al logout

//Appunto: con il valore di"deboucing" se il valore è compreso proprio tra i 2 valori non esegue niente

void setup() {
  //inizializzazione seriale e bt
  Serial.begin(9600);
  bt.begin(9600);
  bt.flush();
  lcd.init();
  lcd.backlight(); //attiva la retroilluminazione

  //inizializzazione output
  //corridoio
  pinMode(rosso_rgb1, OUTPUT);
  pinMode(verde_rgb1, OUTPUT);
  pinMode(blu_rgb1, OUTPUT);

  pinMode(rosso_rgb2, OUTPUT);
  pinMode(verde_rgb2, OUTPUT);
  pinMode(blu_rgb2, OUTPUT);

  pinMode(rosso_rgb3, OUTPUT);
  pinMode(verde_rgb3, OUTPUT);
  pinMode(blu_rgb3, OUTPUT);

  pinMode(bianchi_pin_piccoli , OUTPUT);
  pinMode(rossi_pin_piccoli , OUTPUT);
  pinMode(verdi_pin_piccoli , OUTPUT);
  pinMode(blu_pin_piccoli , OUTPUT);
  pinMode(gialli_pin_piccoli , OUTPUT);

  pinMode(bianchi_pin_grandi , OUTPUT);
  pinMode(rossi_pin_grandi , OUTPUT);
  pinMode(verdi_pin_grandi , OUTPUT);
  pinMode(blu_pin_grandi , OUTPUT);
  pinMode(gialli_pin_grandi , OUTPUT);

  //camera 1
  pinMode(led_grandi_camera1, OUTPUT);
  pinMode(led_piccoli_1_camera1, OUTPUT);
  pinMode(led_piccoli_2_camera1, OUTPUT);

  //camera 2
  pinMode(led_grandi_camera2, OUTPUT);
  pinMode(led_piccoli_1_camera2, OUTPUT);
  pinMode(led_piccoli_2_camera2, OUTPUT);

  //camera 3
  pinMode(led_grandi_camera3, OUTPUT);
  pinMode(led_piccoli_1_camera3, OUTPUT);
  pinMode(led_piccoli_2_camera3, OUTPUT);

  //camera 4
  pinMode(led_grandi_camera4, OUTPUT);
  pinMode(led_piccoli_1_camera4, OUTPUT);
  pinMode(led_piccoli_2_camera4, OUTPUT);

  //illuminazione led bassi esterno
  pinMode(led_giardino, OUTPUT);

  //fontanella e led correlati
  pinMode(pompa_acqua, OUTPUT);
  pinMode(led_pompa_acqua, OUTPUT);

  //porta principale
  servo_pr.attach(52);

  //fotoresistenze
  pinMode(fotoresistenza_1, INPUT);
  pinMode(fotoresistenza_2, INPUT);
  pinMode(led_rosso_sensore_luminosita, OUTPUT);
  pinMode(led_verde_sensore_luminosita, OUTPUT);

  //sensore gas
  pinMode(sensore_gas, INPUT);
  pinMode(led_rosso_sensore_gas, OUTPUT);
  pinMode(led_verde_sensore_gas, OUTPUT);

  //sensore fiamma
  pinMode(sensore_fiamma, INPUT);
  pinMode(led_rosso_sensore_fiamma, OUTPUT);
  pinMode(led_verde_sensore_fiamma, OUTPUT);

  //sensore temperatura e umidita
  dht.begin();
  pinMode(led_rosso_sensore_dht11, OUTPUT);
  pinMode(led_verde_sensore_dht11, OUTPUT);
  pinMode(ventola, OUTPUT);

  //sensore distanza
  pinMode(echo, INPUT);  //ricevitore (riceve il segnale)
  pinMode(trig, OUTPUT); //emmettitore (emette il segnale)
  pinMode(led_rosso_sensore_distanza, OUTPUT);
  pinMode(led_verde_sensore_distanza, OUTPUT);

  //sensore movimento
  pinMode(sensore_movimento, INPUT);
  pinMode(led_rosso_sensore_movimento, OUTPUT);
  pinMode(led_verde_sensore_movimento, OUTPUT);

  //allarme
  pinMode(buzzer1, OUTPUT);
  pinMode(buzzer2, OUTPUT);


  //tutti gli output (led) sono spenti tranne i led piccoli
  //corridoio

  digitalWrite(rosso_rgb1, LOW);
  digitalWrite(verde_rgb1, LOW);
  digitalWrite(blu_rgb1, LOW);

  digitalWrite(rosso_rgb2, LOW);
  digitalWrite(verde_rgb2, LOW);
  digitalWrite(blu_rgb2, LOW);

  digitalWrite(rosso_rgb3, LOW);
  digitalWrite(verde_rgb3, LOW);
  digitalWrite(blu_rgb3, LOW);

  digitalWrite(bianchi_pin_piccoli, HIGH);
  digitalWrite(rossi_pin_piccoli, HIGH);
  digitalWrite(verdi_pin_piccoli, HIGH);
  digitalWrite(blu_pin_piccoli, HIGH);
  digitalWrite(gialli_pin_piccoli, HIGH);

  digitalWrite(bianchi_pin_grandi, LOW);
  digitalWrite(rossi_pin_grandi, LOW);
  digitalWrite(verdi_pin_grandi, LOW);
  digitalWrite(blu_pin_grandi, LOW);
  digitalWrite(gialli_pin_grandi, LOW);

  //led camera 1
  digitalWrite(led_grandi_camera1, LOW);
  digitalWrite(led_piccoli_1_camera1, HIGH);
  digitalWrite(led_piccoli_2_camera1, HIGH);

  //led camera 2
  digitalWrite(led_grandi_camera2, LOW);
  digitalWrite(led_piccoli_1_camera2, HIGH);
  digitalWrite(led_piccoli_2_camera2, HIGH);

  //led camera 3
  digitalWrite(led_grandi_camera3, LOW);
  digitalWrite(led_piccoli_1_camera3, HIGH);
  digitalWrite(led_piccoli_2_camera3, HIGH);

  //led camera 4
  digitalWrite(led_grandi_camera4, LOW);
  digitalWrite(led_piccoli_1_camera4, HIGH);
  digitalWrite(led_piccoli_2_camera4, HIGH);

  //illuminazione giardino
  digitalWrite(led_giardino, HIGH);
  digitalWrite(led_verde_sensore_luminosita, LOW);
  digitalWrite(led_rosso_sensore_luminosita, HIGH);

  //fontanella
  digitalWrite(pompa_acqua, HIGH); //il relay funziona al contrario HIGH --> LOW
  digitalWrite(led_pompa_acqua, LOW);

  //la porta è chiusa
  servo_pr.write(170); //(ho scelto il servomotore per l'apertura della porta e non un semplice motorino perchè se dovessi opporre resistenza mantiene la forza esercitata --> conferisce maggiore suicurezza)

  //sensore movimento
  digitalWrite(led_verde_sensore_movimento, LOW);
  digitalWrite(buzzer1, LOW);
  digitalWrite(buzzer2, LOW);
  digitalWrite(led_rosso_sensore_movimento, HIGH);

  //sensore gas
  digitalWrite(led_verde_sensore_gas, LOW);
  digitalWrite(buzzer1, LOW);
  digitalWrite(buzzer2, LOW);
  digitalWrite(led_rosso_sensore_gas, HIGH);   //inizialmente il rilevatore del gas è disabilitato

  //sensore fiamma
  digitalWrite(led_verde_sensore_fiamma, LOW);
  digitalWrite(buzzer1, LOW);
  digitalWrite(buzzer2, LOW);
  digitalWrite(led_rosso_sensore_fiamma, HIGH);  //inizialmente il rilevatore della fiamma è disabilitato

  //sensore temperatura e umidità
  digitalWrite(led_verde_sensore_dht11, LOW);
  digitalWrite(buzzer1, LOW);
  digitalWrite(buzzer2, LOW);
  digitalWrite(led_rosso_sensore_dht11, HIGH);
  digitalWrite(ventola, LOW);

  //sensore distanza
  digitalWrite(led_verde_sensore_distanza, LOW);
  digitalWrite(buzzer1, LOW);
  digitalWrite(buzzer2, LOW);
  digitalWrite(led_rosso_sensore_distanza, HIGH);

  lcd_testo_iniziale();
}

void loop() {

  if (bt.available()) {
    cmd = bt.read();
    Serial.write(cmd);
  }
  if (Serial.available()) {
    cmd = Serial.read();
    bt.write(cmd);
  }

  val_fotoresistenza_1 = analogRead(fotoresistenza_1);
  val_fotoresistenza_2 = analogRead(fotoresistenza_2 );
  val_gas = analogRead(sensore_gas);
  lettura_fiamma = analogRead(sensore_fiamma);
  val_fiamma = 1023 - lettura_fiamma;
  temperatura = dht.readTemperature();
  umidita = dht.readHumidity();
  val_movimento = digitalRead(sensore_movimento);
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10); //20
  digitalWrite(trig, LOW);
  val_tempo = pulseIn(echo, HIGH);
  val_distanza = 0.0340 * val_tempo / 2; //velocità del suono -- > 0, 0340 cm / microsecondo (s = v * t)

  ///////////////////////////////////////////
  /*inizio comandi "gestione impianto luci"*/
  ///////////////////////////////////////////

  //corridoio
  //tutti i led sono spenti (switch off)
  tutti_led_off_corridoio();

  //tutti i led sono accesi (switch on)
  tutti_led_on_corridoio();

  //led rossi spenti (switch off)
  led_rossi_off_corridoio();

  //led rossi accesi (switch on)
  led_rossi_on_corridoio();

  //led verdi spenti (switch off)
  led_verdi_off_corridoio();

  //led verdi accesi (switch on)
  led_verdi_on_corridoio();

  //led blu spenti (switch off)
  led_blu_off_corridoio();

  //led blu accesi (switch on)
  led_blu_on_corridoio();

  //led bianchi spenti (switch off)
  led_bianchi_off_corridoio();

  //led bianchi accesi (switch on)
  led_bianchi_on_corridoio();

  //led gialli spenti (switch off)
  led_gialli_off_corridoio();

  //led gialli accesi (switch on)
  led_gialli_on_corridoio();

  effetto_uno_corridoio(250);

  effetto_due_corridoio(100);

  effetto_tre_corridoio(350);

  effetto_quattro_corridoio(4);

  effetto_cinque_corridoio(175);

  effetto_sei_corridoio(7);


  //camera 1
  //tutti i led sono accesi
  tutti_led_on_camera1();

  //tutti i led sono spenti
  tutti_led_off_camera1();

  //solo i led grandi sono accesi
  led_grandi_on_camera1();

  //solo i led piccoli sono accesi
  led_piccoli_on_camera1();

  effetto_uno_camera1(150);

  effetto_due_camera1(250);

  effetto_tre_camera1(100);

  effetto_quattro_camera1(150);


  //camera 2
  //tutti i led sono accesi
  tutti_led_on_camera2();

  //tutti i led sono spenti
  tutti_led_off_camera2();

  //solo i led grandi sono accesi
  led_grandi_on_camera2();

  //solo i led piccoli sono accesi
  led_piccoli_on_camera2();

  effetto_uno_camera2(150);

  effetto_due_camera2(250);

  effetto_tre_camera2(100);

  effetto_quattro_camera2(150);


  //camera 3
  //tutti i led sono accesi
  tutti_led_on_camera3();

  //tutti i led sono spenti
  tutti_led_off_camera3();

  //solo i led grandi sono accesi
  led_grandi_on_camera3();

  //solo i led piccoli sono accesi
  led_piccoli_on_camera3();

  effetto_uno_camera3(150);

  effetto_due_camera3(250);

  effetto_tre_camera3(100);

  effetto_quattro_camera3(150);


  //camera 4
  //tutti i led sono accesi
  tutti_led_on_camera4();

  //tutti i led sono spenti
  tutti_led_off_camera4();

  //solo i led grandi sono accesi
  led_grandi_on_camera4();

  //solo i led piccoli sono accesi
  led_piccoli_on_camera4();

  effetto_uno_camera4(150);

  effetto_due_camera4(250);

  effetto_tre_camera4(100);

  effetto_quattro_camera4(150);



  ////////////////////////////
  /*inizio comandi "esterno"*/
  ////////////////////////////

  //fontanella
  //spegnimento fontanella
  spegnimento_fontanella();

  //accensione fontanella
  accensione_fontanella();

  //led fontanella spenti
  led_fontanella_spenti();

  //led fontanella accesi
  led_fontanella_accesi();

  //fotoresistenze
  //lettura_sensore_luminosita();

  //fotoresistenze attivate
  luminosita_on();

  //fotoresistenze disattivate
  luminosita_off();

  //illuminazione giardino on
  illuminazione_giardino_on();

  //illuminazione giardino off
  illuminazione_giardino_off();


  //porta principale

  //porta principale aperta
  porta_principale_aperta();

  //porta principale chiusa
  porta_principale_chiusa();


  //allarme

  //allarme attivato
  allarme_on();

  //allarme disattivato
  allarme_off();



  ////////////////////////////
  /*inizio comandi "sensori"*/
  ////////////////////////////

  //tutti i sensori
  tutti_sensori_on();

  //tutti i sensori disattivati
  tutti_sensori_off();


  //sensore gas
  //lettura_sensore_gas();

  //gas attivato
  gas_on();

  //gas disattivato
  gas_off();

  //sensore fiamma
  //lettura_sensore_fiamma();

  //fiamma attivata
  fiamma_on();

  //fiamma disattivata
  fiamma_off();

  //sensore temperatura e umidità
  //lettura_sensore_dht11();

  //temperatura e umidità attivati
  dht11_on();

  //temperatura e umidità disattivati
  dht11_off();

  //ventola attivata
  ventola_on();

  //ventola disattivata
  ventola_off();

  //sensore di distanza
  //lettura_senore_distanza();

  //distanza attivata
  distanza_on();

  //distanza disattivata
  distanza_off();

  //sensore movimento
  //lettura_sensore_movimento();

  //movimento attivato
  movimento_on();

  //movimento disattivato
  movimento_off();

  //al logout
  logout();


} //chiusura loop








//**********************************************************************************************************************************************************************************************************************
//**********************************************************************************************************************************************************************************************************************
//**********************************************************************************************************************************************************************************************************************
//**********************************************************************************************************************************************************************************************************************
//**********************************************************************************************************************************************************************************************************************
//**********************************************************************************************************************************************************************************************************************
//**********************************************************************************************************************************************************************************************************************
//**********************************************************************************************************************************************************************************************************************
//**********************************************************************************************************************************************************************************************************************
//**********************************************************************************************************************************************************************************************************************
//**********************************************************************************************************************************************************************************************************************
//**********************************************************************************************************************************************************************************************************************
//**********************************************************************************************************************************************************************************************************************
//**********************************************************************************************************************************************************************************************************************
//**********************************************************************************************************************************************************************************************************************












//funzioni

//gestione impianto luci

//camera 1
//tutti i led sono accesi
void tutti_led_on_camera1() {
  if (cmd == '1') {
    lcd_tutti_led_on_camera1();
    digitalWrite(led_grandi_camera1, HIGH);
    digitalWrite(led_piccoli_1_camera1, HIGH);
    digitalWrite(led_piccoli_2_camera1, HIGH);
  }
}

//tutti i led sono spenti
void tutti_led_off_camera1() {
  if (cmd == '2') {
    lcd_tutti_led_off_camera1();
    digitalWrite(led_grandi_camera1, LOW);
    digitalWrite(led_piccoli_1_camera1, LOW);
    digitalWrite(led_piccoli_2_camera1, LOW);
  }
}

//tutti i led grandi sono accesi
void led_grandi_on_camera1() {
  if (cmd == '3') {
    lcd_led_grandi_on_camera1();
    digitalWrite(led_grandi_camera1, HIGH);
    digitalWrite(led_piccoli_1_camera1, LOW);
    digitalWrite(led_piccoli_2_camera1, LOW);
  }
}

//tutti i led piccoli sono accesi
void led_piccoli_on_camera1() {
  if (cmd == '4') {
    lcd_led_piccoli_on_camera1();
    digitalWrite(led_grandi_camera1, LOW);
    digitalWrite(led_piccoli_1_camera1, HIGH);
    digitalWrite(led_piccoli_2_camera1, HIGH);
  }
}


//camera 2
//tutti i led sono accesi
void tutti_led_on_camera2() {
  if (cmd == 'S') {
    lcd_tutti_led_on_camera2();
    digitalWrite(led_grandi_camera2, HIGH);
    digitalWrite(led_piccoli_1_camera2, HIGH);
    digitalWrite(led_piccoli_2_camera2, HIGH);
  }
}

//tutti i led sono spenti
void tutti_led_off_camera2() {
  if (cmd == 'T') {
    lcd_tutti_led_off_camera2();
    digitalWrite(led_grandi_camera2, LOW);
    digitalWrite(led_piccoli_1_camera2, LOW);
    digitalWrite(led_piccoli_2_camera2, LOW);
  }
}

//tutti i led grandi sono accesi
void led_grandi_on_camera2() {
  if (cmd == 'U') {
    lcd_led_grandi_on_camera2();
    digitalWrite(led_grandi_camera2, HIGH);
    digitalWrite(led_piccoli_1_camera2, LOW);
    digitalWrite(led_piccoli_2_camera2, LOW);
  }
}

//tutti i led piccoli sono accesi
void led_piccoli_on_camera2() {
  if (cmd == 'V') {
    lcd_led_piccoli_on_camera2();
    digitalWrite(led_grandi_camera2, LOW);
    digitalWrite(led_piccoli_1_camera2, HIGH);
    digitalWrite(led_piccoli_2_camera2, HIGH);
  }
}


//camera 3
//tutti i led sono accesi
void tutti_led_on_camera3() {
  if (cmd == '+') {
    lcd_tutti_led_on_camera3();
    digitalWrite(led_grandi_camera3, HIGH);
    digitalWrite(led_piccoli_1_camera3, HIGH);
    digitalWrite(led_piccoli_2_camera3, HIGH);
  }
}

//tutti i led sono spenti
void tutti_led_off_camera3() {
  if (cmd == '-') {
    lcd_tutti_led_off_camera3();
    digitalWrite(led_grandi_camera3, LOW);
    digitalWrite(led_piccoli_1_camera3, LOW);
    digitalWrite(led_piccoli_2_camera3, LOW);
  }
}

//tutti i led grandi sono accesi
void led_grandi_on_camera3() {
  if (cmd == '(') {
    lcd_led_grandi_on_camera3();
    digitalWrite(led_grandi_camera3, HIGH);
    digitalWrite(led_piccoli_1_camera3, LOW);
    digitalWrite(led_piccoli_2_camera3, LOW);
  }
}

//tutti i led piccoli sono accesi
void led_piccoli_on_camera3() {
  if (cmd == ')') {
    lcd_led_piccoli_on_camera3();
    digitalWrite(led_grandi_camera3, LOW);
    digitalWrite(led_piccoli_1_camera3, HIGH);
    digitalWrite(led_piccoli_2_camera3, HIGH);
  }
}


//camera 4
//tutti i led sono accesi
void tutti_led_on_camera4() {
  if (cmd == '/') {
    lcd_tutti_led_on_camera4();
    digitalWrite(led_grandi_camera4, HIGH);
    digitalWrite(led_piccoli_1_camera4, HIGH);
    digitalWrite(led_piccoli_2_camera4, HIGH);
  }
}

//tutti i led sono spenti
void tutti_led_off_camera4() {
  if (cmd == '=') {
    lcd_tutti_led_off_camera4();
    digitalWrite(led_grandi_camera4, LOW);
    digitalWrite(led_piccoli_1_camera4, LOW);
    digitalWrite(led_piccoli_2_camera4, LOW);
  }
}

//tutti i led grandi sono accesi
void led_grandi_on_camera4() {
  if (cmd == '?') {
    lcd_led_grandi_on_camera4();
    digitalWrite(led_grandi_camera4, HIGH);
    digitalWrite(led_piccoli_1_camera4, LOW);
    digitalWrite(led_piccoli_2_camera4, LOW);
  }
}

//tutti i led piccoli sono accesi
void led_piccoli_on_camera4() {
  if (cmd == '^') {
    lcd_led_piccoli_on_camera4();
    digitalWrite(led_grandi_camera4, LOW);
    digitalWrite(led_piccoli_1_camera4, HIGH);
    digitalWrite(led_piccoli_2_camera4, HIGH);
  }
}


//corridoio
//tutti i led sono spenti
void tutti_led_off_corridoio() {
  if (cmd == 'q') {
    lcd_tutti_led_off_corridoio();

    digitalWrite(rosso_rgb1, LOW);
    digitalWrite(verde_rgb1, LOW);
    digitalWrite(blu_rgb1, LOW);

    digitalWrite(rosso_rgb2, LOW);
    digitalWrite(verde_rgb2, LOW);
    digitalWrite(blu_rgb2, LOW);

    digitalWrite(rosso_rgb3, LOW);
    digitalWrite(verde_rgb3, LOW);
    digitalWrite(blu_rgb3, LOW);

    digitalWrite(bianchi_pin_piccoli, LOW);
    digitalWrite(rossi_pin_piccoli, LOW);
    digitalWrite(verdi_pin_piccoli, LOW);
    digitalWrite(blu_pin_piccoli, LOW);
    digitalWrite(gialli_pin_piccoli, LOW);

    digitalWrite(bianchi_pin_grandi, LOW);
    digitalWrite(rossi_pin_grandi, LOW);
    digitalWrite(verdi_pin_grandi, LOW);
    digitalWrite(blu_pin_grandi, LOW);
    digitalWrite(gialli_pin_grandi, LOW);
  }
}

//tutti i led sono accesi
void tutti_led_on_corridoio() {
  if (cmd == 'w') {
    lcd_tutti_led_on_corridoio();

    digitalWrite(rosso_rgb1, HIGH);
    digitalWrite(verde_rgb1, HIGH);
    digitalWrite(blu_rgb1, HIGH);

    digitalWrite(rosso_rgb2, HIGH);
    digitalWrite(verde_rgb2, HIGH);
    digitalWrite(blu_rgb2, HIGH);

    digitalWrite(rosso_rgb3, HIGH);
    digitalWrite(verde_rgb3, HIGH);
    digitalWrite(blu_rgb3, HIGH);

    digitalWrite(bianchi_pin_piccoli, HIGH);
    digitalWrite(rossi_pin_piccoli, HIGH);
    digitalWrite(verdi_pin_piccoli, HIGH);
    digitalWrite(blu_pin_piccoli, HIGH);
    digitalWrite(gialli_pin_piccoli, HIGH);

    digitalWrite(bianchi_pin_grandi, HIGH);
    digitalWrite(rossi_pin_grandi, HIGH);
    digitalWrite(verdi_pin_grandi, HIGH);
    digitalWrite(blu_pin_grandi, HIGH);
    digitalWrite(gialli_pin_grandi, HIGH);
  }
}


//led rossi spenti
void led_rossi_off_corridoio() {
  if (cmd == 'e') {
    lcd_rossi_off_corridoio();
    digitalWrite(rosso_rgb1, LOW);
    digitalWrite(rosso_rgb2, LOW);
    digitalWrite(rosso_rgb3, LOW);
    digitalWrite(rossi_pin_piccoli, LOW);
    digitalWrite(rossi_pin_grandi, LOW);
  }
}

//led rossi accesi
void led_rossi_on_corridoio() {
  if (cmd == 'r') {
    lcd_led_rossi_on_corridoio();
    digitalWrite(rosso_rgb1, HIGH);
    digitalWrite(rosso_rgb2, HIGH);
    digitalWrite(rosso_rgb3, HIGH);
    digitalWrite(rossi_pin_piccoli, HIGH);
    digitalWrite(rossi_pin_grandi, HIGH);
  }
}

//led verdi spenti
void led_verdi_off_corridoio() {
  if (cmd == 't') {
    lcd_led_verdi_off_corridoio();
    digitalWrite(verde_rgb1, LOW);
    digitalWrite(verde_rgb2, LOW);
    digitalWrite(verde_rgb3, LOW);
    digitalWrite(verdi_pin_piccoli, LOW);
    digitalWrite(verdi_pin_grandi, LOW);
  }
}

//led verdi accesi
void led_verdi_on_corridoio() {
  if (cmd == 'y') {
    lcd_led_verdi_on_corridoio();
    digitalWrite(verde_rgb1, HIGH);
    digitalWrite(verde_rgb2, HIGH);
    digitalWrite(verde_rgb3, HIGH);
    digitalWrite(verdi_pin_piccoli, HIGH);
    digitalWrite(verdi_pin_grandi, HIGH);
  }
}

//led blu spenti
void led_blu_off_corridoio() {
  if (cmd == 'o') {
    lcd_led_blu_off_corridoio();
    digitalWrite(blu_rgb1, LOW);
    digitalWrite(blu_rgb2, LOW);
    digitalWrite(blu_rgb3, LOW);
    digitalWrite(blu_pin_piccoli, LOW);
    digitalWrite(blu_pin_grandi, LOW);
  }
}

//led blu accesi
void led_blu_on_corridoio() {
  if (cmd == 'p') {
    lcd_led_blu_on_corridoio();
    digitalWrite(blu_rgb1, HIGH);
    digitalWrite(blu_rgb2, HIGH);
    digitalWrite(blu_rgb3, HIGH);
    digitalWrite(blu_pin_piccoli, HIGH);
    digitalWrite(blu_pin_grandi, HIGH);
  }
}

//led bianchi spenti
void led_bianchi_off_corridoio() {
  if (cmd == 'u') {
    lcd_led_bianchi_off_corridoio();
    digitalWrite(bianchi_pin_piccoli, LOW);
    digitalWrite(bianchi_pin_grandi, LOW);
  }
}

//led bianchi accesi
void led_bianchi_on_corridoio() {
  if (cmd == 'i') {
    lcd_led_bianchi_on_corridoio();
    digitalWrite(bianchi_pin_piccoli, HIGH);
    digitalWrite(bianchi_pin_grandi, HIGH);
  }
}

//led gialli spenti
void led_gialli_off_corridoio() {
  if (cmd == 'a') {
    lcd_led_gialli_off_corridoio();
    digitalWrite(gialli_pin_piccoli, LOW);
    digitalWrite(gialli_pin_grandi, LOW);
  }
}

//led gialli accesi
void led_gialli_on_corridoio() {
  if (cmd == 's') {
    lcd_led_gialli_on_corridoio();
    digitalWrite(gialli_pin_piccoli, HIGH);
    digitalWrite(gialli_pin_grandi, HIGH);
  }
}














//gli effetti di luce sono universali per tutte le stanze della struttura
//CAMERA 1
void effetto_uno_camera1(int t) {
  if (cmd == '5') {
    lcd_effetto_uno_camera1();
    digitalWrite(led_grandi_camera1, LOW);
    digitalWrite(led_piccoli_1_camera1, LOW);
    digitalWrite(led_piccoli_2_camera1, LOW);
    delay(t);

    digitalWrite(led_grandi_camera1, LOW);
    digitalWrite(led_piccoli_1_camera1, HIGH);
    digitalWrite(led_piccoli_2_camera1, LOW);
    delay(t);

    digitalWrite(led_grandi_camera1, LOW);
    digitalWrite(led_piccoli_1_camera1, LOW);
    digitalWrite(led_piccoli_2_camera1, HIGH);
    delay(t);

    digitalWrite(led_grandi_camera1, LOW);
    digitalWrite(led_piccoli_1_camera1, HIGH);
    digitalWrite(led_piccoli_2_camera1, LOW);
    delay(t);

    digitalWrite(led_grandi_camera1, LOW);
    digitalWrite(led_piccoli_1_camera1, LOW);
    digitalWrite(led_piccoli_2_camera1, HIGH);
    delay(t);

    digitalWrite(led_grandi_camera1, HIGH);
    digitalWrite(led_piccoli_1_camera1, LOW);
    digitalWrite(led_piccoli_2_camera1, LOW);
    delay(t);

    digitalWrite(led_grandi_camera1, HIGH);
    digitalWrite(led_piccoli_1_camera1, HIGH);
    digitalWrite(led_piccoli_2_camera1, LOW);
    delay(t);

    digitalWrite(led_grandi_camera1, HIGH);
    digitalWrite(led_piccoli_1_camera1, HIGH);
    digitalWrite(led_piccoli_2_camera1, HIGH);
    delay(t);

    digitalWrite(led_grandi_camera1, LOW);
    digitalWrite(led_piccoli_1_camera1, HIGH);
    digitalWrite(led_piccoli_2_camera1, HIGH);
    delay(t);

    digitalWrite(led_grandi_camera1, LOW);
    digitalWrite(led_piccoli_1_camera1, LOW);
    digitalWrite(led_piccoli_2_camera1, HIGH);
    delay(t);
  }
}

void effetto_due_camera1(int t) {
  if (cmd == '6') {
    lcd_effetto_due_camera1();
    //solo grandi
    digitalWrite(led_grandi_camera1, HIGH);
    digitalWrite(led_piccoli_1_camera1, LOW);
    digitalWrite(led_piccoli_2_camera1, LOW);
    delay(t);

    //solo piccoli
    digitalWrite(led_grandi_camera1, LOW);
    digitalWrite(led_piccoli_1_camera1, HIGH);
    digitalWrite(led_piccoli_2_camera1, HIGH);
    delay(t);
  }
}

void effetto_tre_camera1(int t) {
  if (cmd == '7') {
    lcd_effetto_tre_camera1();
    digitalWrite(led_grandi_camera1, LOW);
    digitalWrite(led_piccoli_1_camera1, LOW);
    digitalWrite(led_piccoli_2_camera1, LOW);
    delay(t);

    digitalWrite(led_grandi_camera1, HIGH);
    digitalWrite(led_piccoli_1_camera1, HIGH);
    digitalWrite(led_piccoli_2_camera1, HIGH);
    delay(t);
  }
}


void effetto_quattro_camera1(int t) {
  if (cmd == '8') {
    lcd_effetto_quattro_camera1();
    for (int i = 0; i < 127; i++) {
      analogWrite(led_grandi_camera1, i);
      digitalWrite(led_piccoli_1_camera1, HIGH);
      digitalWrite(led_piccoli_2_camera1, LOW);
      delay(3);
    }

    for (int i = 127; i < 255; i++) {
      analogWrite(led_grandi_camera1, i);
      digitalWrite(led_piccoli_1_camera1, LOW);
      digitalWrite(led_piccoli_2_camera1, HIGH);
      delay(3);
    }

    for (int i = 255; i > 204; i--) {
      analogWrite(led_grandi_camera1, i);
      digitalWrite(led_piccoli_1_camera1, HIGH);
      digitalWrite(led_piccoli_2_camera1, LOW);
      delay(3);
    }

    for (int i = 204; i > 153; i--) {
      analogWrite(led_grandi_camera1, i);
      digitalWrite(led_piccoli_1_camera1, HIGH);
      digitalWrite(led_piccoli_2_camera1, LOW);
      delay(3);
    }

    for (int i = 153; i > 102; i--) {
      analogWrite(led_grandi_camera1, i);
      digitalWrite(led_piccoli_1_camera1, LOW);
      digitalWrite(led_piccoli_2_camera1, HIGH);
      delay(3);
    }

    for (int i = 102; i > 51; i--) {
      analogWrite(led_grandi_camera1, i);
      digitalWrite(led_piccoli_1_camera1, LOW);
      digitalWrite(led_piccoli_2_camera1, HIGH);
      delay(4);
    }

    for (int i = 51; i > 0; i--) {
      analogWrite(led_grandi_camera1, i);
      digitalWrite(led_piccoli_1_camera1, LOW);
      digitalWrite(led_piccoli_2_camera1, HIGH);
      delay(4);
    }
  }
}


//CAMERA 2
void effetto_uno_camera2(int t) {
  if (cmd == 'W') {
    lcd_effetto_uno_camera2();
    digitalWrite(led_grandi_camera2, LOW);
    digitalWrite(led_piccoli_1_camera2, LOW);
    digitalWrite(led_piccoli_2_camera2, LOW);
    delay(t);

    digitalWrite(led_grandi_camera2, LOW);
    digitalWrite(led_piccoli_1_camera2, HIGH);
    digitalWrite(led_piccoli_2_camera2, LOW);
    delay(t);

    digitalWrite(led_grandi_camera2, LOW);
    digitalWrite(led_piccoli_1_camera2, LOW);
    digitalWrite(led_piccoli_2_camera2, HIGH);
    delay(t);

    digitalWrite(led_grandi_camera2, LOW);
    digitalWrite(led_piccoli_1_camera2, HIGH);
    digitalWrite(led_piccoli_2_camera2, LOW);
    delay(t);

    digitalWrite(led_grandi_camera2, LOW);
    digitalWrite(led_piccoli_1_camera2, LOW);
    digitalWrite(led_piccoli_2_camera2, HIGH);
    delay(t);

    digitalWrite(led_grandi_camera2, HIGH);
    digitalWrite(led_piccoli_1_camera2, LOW);
    digitalWrite(led_piccoli_2_camera2, LOW);
    delay(t);

    digitalWrite(led_grandi_camera2, HIGH);
    digitalWrite(led_piccoli_1_camera2, HIGH);
    digitalWrite(led_piccoli_2_camera2, LOW);
    delay(t);

    digitalWrite(led_grandi_camera2, HIGH);
    digitalWrite(led_piccoli_1_camera2, HIGH);
    digitalWrite(led_piccoli_2_camera2, HIGH);
    delay(t);

    digitalWrite(led_grandi_camera2, LOW);
    digitalWrite(led_piccoli_1_camera2, HIGH);
    digitalWrite(led_piccoli_2_camera2, HIGH);
    delay(t);

    digitalWrite(led_grandi_camera2, LOW);
    digitalWrite(led_piccoli_1_camera2, LOW);
    digitalWrite(led_piccoli_2_camera2, HIGH);
    delay(t);
  }
}

void effetto_due_camera2(int t) {
  if (cmd == 'X') {
    lcd_effetto_due_camera2();
    //solo grandi
    digitalWrite(led_grandi_camera2, HIGH);
    digitalWrite(led_piccoli_1_camera2, LOW);
    digitalWrite(led_piccoli_2_camera2, LOW);
    delay(t);

    //solo piccoli
    digitalWrite(led_grandi_camera2, LOW);
    digitalWrite(led_piccoli_1_camera2, HIGH);
    digitalWrite(led_piccoli_2_camera2, HIGH);
    delay(t);
  }
}

void effetto_tre_camera2(int t) {
  if (cmd == 'Y') {
    lcd_effetto_tre_camera2();
    digitalWrite(led_grandi_camera2, LOW);
    digitalWrite(led_piccoli_1_camera2, LOW);
    digitalWrite(led_piccoli_2_camera2, LOW);
    delay(t);

    digitalWrite(led_grandi_camera2, HIGH);
    digitalWrite(led_piccoli_1_camera2, HIGH);
    digitalWrite(led_piccoli_2_camera2, HIGH);
    delay(t);
  }
}


void effetto_quattro_camera2(int t) {
  if (cmd == 'Z') {
    lcd_effetto_quattro_camera2();
    for (int i = 0; i < 127; i++) {
      analogWrite(led_grandi_camera2, i);
      digitalWrite(led_piccoli_1_camera2, HIGH);
      digitalWrite(led_piccoli_2_camera2, LOW);
      delay(3);
    }

    for (int i = 127; i < 255; i++) {
      analogWrite(led_grandi_camera2, i);
      digitalWrite(led_piccoli_1_camera2, LOW);
      digitalWrite(led_piccoli_2_camera2, HIGH);
      delay(3);
    }

    for (int i = 255; i > 204; i--) {
      analogWrite(led_grandi_camera2, i);
      digitalWrite(led_piccoli_1_camera2, HIGH);
      digitalWrite(led_piccoli_2_camera2, LOW);
      delay(3);
    }

    for (int i = 204; i > 153; i--) {
      analogWrite(led_grandi_camera2, i);
      digitalWrite(led_piccoli_1_camera2, HIGH);
      digitalWrite(led_piccoli_2_camera2, LOW);
      delay(3);
    }

    for (int i = 153; i > 102; i--) {
      analogWrite(led_grandi_camera2, i);
      digitalWrite(led_piccoli_1_camera2, LOW);
      digitalWrite(led_piccoli_2_camera2, HIGH);
      delay(3);
    }

    for (int i = 102; i > 51; i--) {
      analogWrite(led_grandi_camera2, i);
      digitalWrite(led_piccoli_1_camera2, LOW);
      digitalWrite(led_piccoli_2_camera2, HIGH);
      delay(4);
    }

    for (int i = 51; i > 0; i--) {
      analogWrite(led_grandi_camera2, i);
      digitalWrite(led_piccoli_1_camera2, LOW);
      digitalWrite(led_piccoli_2_camera2, HIGH);
      delay(4);
    }
  }
}


//CAMERA 3
void effetto_uno_camera3(int t) {
  if (cmd == '[') {
    lcd_effetto_uno_camera3();
    digitalWrite(led_grandi_camera3, LOW);
    digitalWrite(led_piccoli_1_camera3, LOW);
    digitalWrite(led_piccoli_2_camera3, LOW);
    delay(t);

    digitalWrite(led_grandi_camera3, LOW);
    digitalWrite(led_piccoli_1_camera3, HIGH);
    digitalWrite(led_piccoli_2_camera3, LOW);
    delay(t);

    digitalWrite(led_grandi_camera3, LOW);
    digitalWrite(led_piccoli_1_camera3, LOW);
    digitalWrite(led_piccoli_2_camera3, HIGH);
    delay(t);

    digitalWrite(led_grandi_camera3, LOW);
    digitalWrite(led_piccoli_1_camera3, HIGH);
    digitalWrite(led_piccoli_2_camera3, LOW);
    delay(t);

    digitalWrite(led_grandi_camera3, LOW);
    digitalWrite(led_piccoli_1_camera3, LOW);
    digitalWrite(led_piccoli_2_camera3, HIGH);
    delay(t);

    digitalWrite(led_grandi_camera3, HIGH);
    digitalWrite(led_piccoli_1_camera3, LOW);
    digitalWrite(led_piccoli_2_camera3, LOW);
    delay(t);

    digitalWrite(led_grandi_camera3, HIGH);
    digitalWrite(led_piccoli_1_camera3, HIGH);
    digitalWrite(led_piccoli_2_camera3, LOW);
    delay(t);

    digitalWrite(led_grandi_camera3, HIGH);
    digitalWrite(led_piccoli_1_camera3, HIGH);
    digitalWrite(led_piccoli_2_camera3, HIGH);
    delay(t);

    digitalWrite(led_grandi_camera3, LOW);
    digitalWrite(led_piccoli_1_camera3, HIGH);
    digitalWrite(led_piccoli_2_camera3, HIGH);
    delay(t);

    digitalWrite(led_grandi_camera3, LOW);
    digitalWrite(led_piccoli_1_camera3, LOW);
    digitalWrite(led_piccoli_2_camera3, HIGH);
    delay(t);
  }
}

void effetto_due_camera3(int t) {
  if (cmd == ']') {
    lcd_effetto_due_camera3();
    //solo grandi
    digitalWrite(led_grandi_camera3, HIGH);
    digitalWrite(led_piccoli_1_camera3, LOW);
    digitalWrite(led_piccoli_2_camera3, LOW);
    delay(t);

    //solo piccoli
    digitalWrite(led_grandi_camera3, LOW);
    digitalWrite(led_piccoli_1_camera3, HIGH);
    digitalWrite(led_piccoli_2_camera3, HIGH);
    delay(t);
  }
}

void effetto_tre_camera3(int t) {
  if (cmd == '!') {
    lcd_effetto_tre_camera3();
    digitalWrite(led_grandi_camera3, LOW);
    digitalWrite(led_piccoli_1_camera3, LOW);
    digitalWrite(led_piccoli_2_camera3, LOW);
    delay(t);

    digitalWrite(led_grandi_camera3, HIGH);
    digitalWrite(led_piccoli_1_camera3, HIGH);
    digitalWrite(led_piccoli_2_camera3, HIGH);
    delay(t);
  }
}


void effetto_quattro_camera3(int t) {
  if (cmd == '"') {
    lcd_effetto_quattro_camera3();
    for (int i = 0; i < 127; i++) {
      analogWrite(led_grandi_camera3, i);
      digitalWrite(led_piccoli_1_camera3, HIGH);
      digitalWrite(led_piccoli_2_camera3, LOW);
      delay(3);
    }

    for (int i = 127; i < 255; i++) {
      analogWrite(led_grandi_camera3, i);
      digitalWrite(led_piccoli_1_camera3, LOW);
      digitalWrite(led_piccoli_2_camera3, HIGH);
      delay(3);
    }

    for (int i = 255; i > 204; i--) {
      analogWrite(led_grandi_camera3, i);
      digitalWrite(led_piccoli_1_camera3, HIGH);
      digitalWrite(led_piccoli_2_camera3, LOW);
      delay(3);
    }

    for (int i = 204; i > 153; i--) {
      analogWrite(led_grandi_camera3, i);
      digitalWrite(led_piccoli_1_camera3, HIGH);
      digitalWrite(led_piccoli_2_camera3, LOW);
      delay(3);
    }

    for (int i = 153; i > 102; i--) {
      analogWrite(led_grandi_camera3, i);
      digitalWrite(led_piccoli_1_camera3, LOW);
      digitalWrite(led_piccoli_2_camera3, HIGH);
      delay(3);
    }

    for (int i = 102; i > 51; i--) {
      analogWrite(led_grandi_camera3, i);
      digitalWrite(led_piccoli_1_camera3, LOW);
      digitalWrite(led_piccoli_2_camera3, HIGH);
      delay(4);
    }

    for (int i = 51; i > 0; i--) {
      analogWrite(led_grandi_camera3, i);
      digitalWrite(led_piccoli_1_camera3, LOW);
      digitalWrite(led_piccoli_2_camera3, HIGH);
      delay(4);
    }
  }
}


//CAMERA 4
void effetto_uno_camera4(int t) {
  if (cmd == '@') {
    lcd_effetto_uno_camera4();
    digitalWrite(led_grandi_camera4, LOW);
    digitalWrite(led_piccoli_1_camera4, LOW);
    digitalWrite(led_piccoli_2_camera4, LOW);
    delay(t);

    digitalWrite(led_grandi_camera4, LOW);
    digitalWrite(led_piccoli_1_camera4, HIGH);
    digitalWrite(led_piccoli_2_camera4, LOW);
    delay(t);

    digitalWrite(led_grandi_camera4, LOW);
    digitalWrite(led_piccoli_1_camera4, LOW);
    digitalWrite(led_piccoli_2_camera4, HIGH);
    delay(t);

    digitalWrite(led_grandi_camera4, LOW);
    digitalWrite(led_piccoli_1_camera4, HIGH);
    digitalWrite(led_piccoli_2_camera4, LOW);
    delay(t);

    digitalWrite(led_grandi_camera4, LOW);
    digitalWrite(led_piccoli_1_camera4, LOW);
    digitalWrite(led_piccoli_2_camera4, HIGH);
    delay(t);

    digitalWrite(led_grandi_camera4, HIGH);
    digitalWrite(led_piccoli_1_camera4, LOW);
    digitalWrite(led_piccoli_2_camera4, LOW);
    delay(t);

    digitalWrite(led_grandi_camera4, HIGH);
    digitalWrite(led_piccoli_1_camera4, HIGH);
    digitalWrite(led_piccoli_2_camera4, LOW);
    delay(t);

    digitalWrite(led_grandi_camera4, HIGH);
    digitalWrite(led_piccoli_1_camera4, HIGH);
    digitalWrite(led_piccoli_2_camera4, HIGH);
    delay(t);

    digitalWrite(led_grandi_camera4, LOW);
    digitalWrite(led_piccoli_1_camera4, HIGH);
    digitalWrite(led_piccoli_2_camera4, HIGH);
    delay(t);

    digitalWrite(led_grandi_camera4, LOW);
    digitalWrite(led_piccoli_1_camera4, LOW);
    digitalWrite(led_piccoli_2_camera4, HIGH);
    delay(t);
  }
}

void effetto_due_camera4(int t) {
  if (cmd == '#') {
    lcd_effetto_due_camera4();
    //solo grandi
    digitalWrite(led_grandi_camera4, HIGH);
    digitalWrite(led_piccoli_1_camera4, LOW);
    digitalWrite(led_piccoli_2_camera4, LOW);
    delay(t);

    //solo piccoli
    digitalWrite(led_grandi_camera4, LOW);
    digitalWrite(led_piccoli_1_camera4, HIGH);
    digitalWrite(led_piccoli_2_camera4, HIGH);
    delay(t);
  }
}

void effetto_tre_camera4(int t) {
  if (cmd == '9') {
    lcd_effetto_tre_camera4();
    digitalWrite(led_grandi_camera4, LOW);
    digitalWrite(led_piccoli_1_camera4, LOW);
    digitalWrite(led_piccoli_2_camera4, LOW);
    delay(t);

    digitalWrite(led_grandi_camera4, HIGH);
    digitalWrite(led_piccoli_1_camera4, HIGH);
    digitalWrite(led_piccoli_2_camera4, HIGH);
    delay(t);
  }
}


void effetto_quattro_camera4(int t) {
  if (cmd == '0') {
    lcd_effetto_quattro_camera4();
    for (int i = 0; i < 127; i++) {
      analogWrite(led_grandi_camera4, i);
      digitalWrite(led_piccoli_1_camera4, HIGH);
      digitalWrite(led_piccoli_2_camera4, LOW);
      delay(3);
    }

    for (int i = 127; i < 255; i++) {
      analogWrite(led_grandi_camera4, i);
      digitalWrite(led_piccoli_1_camera4, LOW);
      digitalWrite(led_piccoli_2_camera4, HIGH);
      delay(3);
    }

    for (int i = 255; i > 204; i--) {
      analogWrite(led_grandi_camera4, i);
      digitalWrite(led_piccoli_1_camera4, HIGH);
      digitalWrite(led_piccoli_2_camera4, LOW);
      delay(3);
    }

    for (int i = 204; i > 153; i--) {
      analogWrite(led_grandi_camera4, i);
      digitalWrite(led_piccoli_1_camera4, HIGH);
      digitalWrite(led_piccoli_2_camera4, LOW);
      delay(3);
    }

    for (int i = 153; i > 102; i--) {
      analogWrite(led_grandi_camera4, i);
      digitalWrite(led_piccoli_1_camera4, LOW);
      digitalWrite(led_piccoli_2_camera4, HIGH);
      delay(3);
    }

    for (int i = 102; i > 51; i--) {
      analogWrite(led_grandi_camera4, i);
      digitalWrite(led_piccoli_1_camera4, LOW);
      digitalWrite(led_piccoli_2_camera4, HIGH);
      delay(4);
    }

    for (int i = 51; i > 0; i--) {
      analogWrite(led_grandi_camera4, i);
      digitalWrite(led_piccoli_1_camera4, LOW);
      digitalWrite(led_piccoli_2_camera4, HIGH);
      delay(4);
    }
  }
}

//CORRIDOIO
void effetto_uno_corridoio(int t) {
  if (cmd == 'd') {
    lcd_effetto_uno_corridoio();
    digitalWrite(rosso_rgb1, HIGH);
    digitalWrite(verde_rgb1, HIGH);
    digitalWrite(blu_rgb1, HIGH);

    digitalWrite(rosso_rgb2, HIGH);
    digitalWrite(verde_rgb2, HIGH);
    digitalWrite(blu_rgb2, HIGH);

    digitalWrite(rosso_rgb3, HIGH);
    digitalWrite(verde_rgb3, HIGH);
    digitalWrite(blu_rgb3, HIGH);

    digitalWrite(bianchi_pin_piccoli, HIGH);
    digitalWrite(rossi_pin_piccoli, LOW);
    digitalWrite(verdi_pin_piccoli, LOW);
    digitalWrite(blu_pin_piccoli, LOW);
    digitalWrite(gialli_pin_piccoli, LOW);

    digitalWrite(bianchi_pin_grandi, HIGH);
    digitalWrite(rossi_pin_grandi, LOW);
    digitalWrite(verdi_pin_grandi, LOW);
    digitalWrite(blu_pin_grandi, LOW);
    digitalWrite(gialli_pin_grandi, LOW);
    delay(t);
    digitalWrite(rosso_rgb1, HIGH);
    digitalWrite(verde_rgb1, LOW);
    digitalWrite(blu_rgb1, LOW);

    digitalWrite(rosso_rgb2, HIGH);
    digitalWrite(verde_rgb2, LOW);
    digitalWrite(blu_rgb2, LOW);

    digitalWrite(rosso_rgb3, HIGH);
    digitalWrite(verde_rgb3, LOW);
    digitalWrite(blu_rgb3, LOW);

    digitalWrite(bianchi_pin_piccoli, LOW);
    digitalWrite(rossi_pin_piccoli, HIGH);
    digitalWrite(verdi_pin_piccoli, LOW);
    digitalWrite(blu_pin_piccoli, LOW);
    digitalWrite(gialli_pin_piccoli, LOW);

    digitalWrite(bianchi_pin_grandi, LOW);
    digitalWrite(rossi_pin_grandi, HIGH);
    digitalWrite(verdi_pin_grandi, LOW);
    digitalWrite(blu_pin_grandi, LOW);
    digitalWrite(gialli_pin_grandi, LOW);
    delay(t);
    digitalWrite(rosso_rgb1, LOW);
    digitalWrite(verde_rgb1, HIGH);
    digitalWrite(blu_rgb1, LOW);

    digitalWrite(rosso_rgb2, LOW);
    digitalWrite(verde_rgb2, HIGH);
    digitalWrite(blu_rgb2, LOW);

    digitalWrite(rosso_rgb3, LOW);
    digitalWrite(verde_rgb3, HIGH);
    digitalWrite(blu_rgb3, LOW);

    digitalWrite(bianchi_pin_piccoli, LOW);
    digitalWrite(rossi_pin_piccoli, LOW);
    digitalWrite(verdi_pin_piccoli, HIGH);
    digitalWrite(blu_pin_piccoli, LOW);
    digitalWrite(gialli_pin_piccoli, LOW);

    digitalWrite(bianchi_pin_grandi, LOW);
    digitalWrite(rossi_pin_grandi, LOW);
    digitalWrite(verdi_pin_grandi, HIGH);
    digitalWrite(blu_pin_grandi, LOW);
    digitalWrite(gialli_pin_grandi, LOW);
    delay(t);
    digitalWrite(rosso_rgb1, LOW);
    digitalWrite(verde_rgb1, LOW);
    digitalWrite(blu_rgb1, HIGH);

    digitalWrite(rosso_rgb2, LOW);
    digitalWrite(verde_rgb2, LOW);
    digitalWrite(blu_rgb2, HIGH);

    digitalWrite(rosso_rgb3, LOW);
    digitalWrite(verde_rgb3, LOW);
    digitalWrite(blu_rgb3, HIGH);

    digitalWrite(bianchi_pin_piccoli, LOW);
    digitalWrite(rossi_pin_piccoli, LOW);
    digitalWrite(verdi_pin_piccoli, LOW);
    digitalWrite(blu_pin_piccoli, HIGH);
    digitalWrite(gialli_pin_piccoli, LOW);

    digitalWrite(bianchi_pin_grandi, LOW);
    digitalWrite(rossi_pin_grandi, LOW);
    digitalWrite(verdi_pin_grandi, LOW);
    digitalWrite(blu_pin_grandi, HIGH);
    digitalWrite(gialli_pin_grandi, LOW);
    delay(t);
    digitalWrite(rosso_rgb1, HIGH);
    digitalWrite(verde_rgb1, HIGH);
    digitalWrite(blu_rgb1, LOW);

    digitalWrite(rosso_rgb2, HIGH);
    digitalWrite(verde_rgb2, HIGH);
    digitalWrite(blu_rgb2, LOW);

    digitalWrite(rosso_rgb3, HIGH);
    digitalWrite(verde_rgb3, HIGH);
    digitalWrite(blu_rgb3, LOW);

    digitalWrite(bianchi_pin_piccoli, LOW);
    digitalWrite(rossi_pin_piccoli, LOW);
    digitalWrite(verdi_pin_piccoli, LOW);
    digitalWrite(blu_pin_piccoli, LOW);
    digitalWrite(gialli_pin_piccoli, HIGH);

    digitalWrite(bianchi_pin_grandi, LOW);
    digitalWrite(rossi_pin_grandi, LOW);
    digitalWrite(verdi_pin_grandi, LOW);
    digitalWrite(blu_pin_grandi, LOW);
    digitalWrite(gialli_pin_grandi, HIGH);
    delay(t);
  }
}

void effetto_due_corridoio(int t) {
  if (cmd == 'f') {
    lcd_effetto_due_corridoio();
    digitalWrite(rosso_rgb1, HIGH);
    digitalWrite(verde_rgb1, LOW);
    digitalWrite(blu_rgb1, HIGH);

    digitalWrite(rosso_rgb2, LOW);
    digitalWrite(verde_rgb2, LOW);
    digitalWrite(blu_rgb2, LOW);

    digitalWrite(rosso_rgb3, HIGH);
    digitalWrite(verde_rgb3, LOW);
    digitalWrite(blu_rgb3, HIGH);

    digitalWrite(bianchi_pin_piccoli, LOW);
    digitalWrite(rossi_pin_piccoli, LOW);
    digitalWrite(verdi_pin_piccoli, LOW);
    digitalWrite(blu_pin_piccoli, LOW);
    digitalWrite(gialli_pin_piccoli, LOW);

    digitalWrite(bianchi_pin_grandi, LOW);
    digitalWrite(rossi_pin_grandi, LOW);
    digitalWrite(verdi_pin_grandi, LOW);
    digitalWrite(blu_pin_grandi, LOW);
    digitalWrite(gialli_pin_grandi, LOW);
    delay(t);
    digitalWrite(rosso_rgb1, LOW);
    digitalWrite(verde_rgb1, LOW);
    digitalWrite(blu_rgb1, LOW);

    digitalWrite(rosso_rgb2, LOW);
    digitalWrite(verde_rgb2, LOW);
    digitalWrite(blu_rgb2, LOW);

    digitalWrite(rosso_rgb3, LOW);
    digitalWrite(verde_rgb3, LOW);
    digitalWrite(blu_rgb3, LOW);

    digitalWrite(bianchi_pin_piccoli, LOW);
    digitalWrite(rossi_pin_piccoli, HIGH);
    digitalWrite(verdi_pin_piccoli, LOW);
    digitalWrite(blu_pin_piccoli, LOW);
    digitalWrite(gialli_pin_piccoli, LOW);

    digitalWrite(bianchi_pin_grandi, LOW);
    digitalWrite(rossi_pin_grandi, LOW);
    digitalWrite(verdi_pin_grandi, LOW);
    digitalWrite(blu_pin_grandi, HIGH);
    digitalWrite(gialli_pin_grandi, LOW);
    delay(t);
    digitalWrite(rosso_rgb1, LOW);
    digitalWrite(verde_rgb1, LOW);
    digitalWrite(blu_rgb1, LOW);

    digitalWrite(rosso_rgb2, LOW);
    digitalWrite(verde_rgb2, LOW);
    digitalWrite(blu_rgb2, LOW);

    digitalWrite(rosso_rgb3, LOW);
    digitalWrite(verde_rgb3, LOW);
    digitalWrite(blu_rgb3, LOW);

    digitalWrite(bianchi_pin_piccoli, LOW);
    digitalWrite(rossi_pin_piccoli, LOW);
    digitalWrite(verdi_pin_piccoli, LOW);
    digitalWrite(blu_pin_piccoli, HIGH);
    digitalWrite(gialli_pin_piccoli, LOW);

    digitalWrite(bianchi_pin_grandi, LOW);
    digitalWrite(rossi_pin_grandi, HIGH);
    digitalWrite(verdi_pin_grandi, LOW);
    digitalWrite(blu_pin_grandi, LOW);
    digitalWrite(gialli_pin_grandi, LOW);

    delay(t);
    digitalWrite(rosso_rgb1, LOW);
    digitalWrite(verde_rgb1, LOW);
    digitalWrite(blu_rgb1, LOW);

    digitalWrite(rosso_rgb2, LOW);
    digitalWrite(verde_rgb2, HIGH);
    digitalWrite(blu_rgb2, LOW);

    digitalWrite(rosso_rgb3, LOW);
    digitalWrite(verde_rgb3, LOW);
    digitalWrite(blu_rgb3, LOW);

    digitalWrite(bianchi_pin_piccoli, LOW);
    digitalWrite(rossi_pin_piccoli, LOW);
    digitalWrite(verdi_pin_piccoli, LOW);
    digitalWrite(blu_pin_piccoli, LOW);
    digitalWrite(gialli_pin_piccoli, LOW);

    digitalWrite(bianchi_pin_grandi, LOW);
    digitalWrite(rossi_pin_grandi, LOW);
    digitalWrite(verdi_pin_grandi, LOW);
    digitalWrite(blu_pin_grandi, LOW);
    digitalWrite(gialli_pin_grandi, LOW);
    delay(t);
    digitalWrite(rosso_rgb1, LOW);
    digitalWrite(verde_rgb1, LOW);
    digitalWrite(blu_rgb1, LOW);

    digitalWrite(rosso_rgb2, LOW);
    digitalWrite(verde_rgb2, LOW);
    digitalWrite(blu_rgb2, LOW);

    digitalWrite(rosso_rgb3, LOW);
    digitalWrite(verde_rgb3, LOW);
    digitalWrite(blu_rgb3, LOW);

    digitalWrite(bianchi_pin_piccoli, LOW);
    digitalWrite(rossi_pin_piccoli, LOW);
    digitalWrite(verdi_pin_piccoli, LOW);
    digitalWrite(blu_pin_piccoli, LOW);
    digitalWrite(gialli_pin_piccoli, HIGH);

    digitalWrite(bianchi_pin_grandi, HIGH);
    digitalWrite(rossi_pin_grandi, LOW);
    digitalWrite(verdi_pin_grandi, LOW);
    digitalWrite(blu_pin_grandi, LOW);
    digitalWrite(gialli_pin_grandi, LOW);
    delay(t);
    digitalWrite(rosso_rgb1, LOW);
    digitalWrite(verde_rgb1, LOW);
    digitalWrite(blu_rgb1, LOW);

    digitalWrite(rosso_rgb2, LOW);
    digitalWrite(verde_rgb2, LOW);
    digitalWrite(blu_rgb2, LOW);

    digitalWrite(rosso_rgb3, LOW);
    digitalWrite(verde_rgb3, LOW);
    digitalWrite(blu_rgb3, LOW);

    digitalWrite(bianchi_pin_piccoli, LOW);
    digitalWrite(rossi_pin_piccoli, LOW);
    digitalWrite(verdi_pin_piccoli, HIGH);
    digitalWrite(blu_pin_piccoli, LOW);
    digitalWrite(gialli_pin_piccoli, LOW);

    digitalWrite(bianchi_pin_grandi, LOW);
    digitalWrite(rossi_pin_grandi, LOW);
    digitalWrite(verdi_pin_grandi, HIGH);
    digitalWrite(blu_pin_grandi, LOW);
    digitalWrite(gialli_pin_grandi, LOW);
    delay(t);
    digitalWrite(rosso_rgb1, LOW);
    digitalWrite(verde_rgb1, LOW);
    digitalWrite(blu_rgb1, LOW);

    digitalWrite(rosso_rgb2, LOW);
    digitalWrite(verde_rgb2, LOW);
    digitalWrite(blu_rgb2, LOW);

    digitalWrite(rosso_rgb3, LOW);
    digitalWrite(verde_rgb3, LOW);
    digitalWrite(blu_rgb3, LOW);

    digitalWrite(bianchi_pin_piccoli, HIGH);
    digitalWrite(rossi_pin_piccoli, LOW);
    digitalWrite(verdi_pin_piccoli, LOW);
    digitalWrite(blu_pin_piccoli, LOW);
    digitalWrite(gialli_pin_piccoli, LOW);

    digitalWrite(bianchi_pin_grandi, LOW);
    digitalWrite(rossi_pin_grandi, LOW);
    digitalWrite(verdi_pin_grandi, LOW);
    digitalWrite(blu_pin_grandi, LOW);
    digitalWrite(gialli_pin_grandi, HIGH);
  }
}

void effetto_tre_corridoio(int t) {
  if (cmd == 'g') {
    lcd_effetto_tre_corridoio();
    digitalWrite(rosso_rgb1, HIGH);
    digitalWrite(verde_rgb1, LOW);
    digitalWrite(blu_rgb1, LOW);

    digitalWrite(rosso_rgb2, LOW);
    digitalWrite(verde_rgb2, HIGH);
    digitalWrite(blu_rgb2, LOW);

    digitalWrite(rosso_rgb3, HIGH);
    digitalWrite(verde_rgb3, LOW);
    digitalWrite(blu_rgb3, LOW);

    digitalWrite(bianchi_pin_piccoli, LOW);
    digitalWrite(rossi_pin_piccoli, HIGH);
    digitalWrite(verdi_pin_piccoli, LOW);
    digitalWrite(blu_pin_piccoli, HIGH);
    digitalWrite(gialli_pin_piccoli, LOW);

    digitalWrite(bianchi_pin_grandi, HIGH);
    digitalWrite(rossi_pin_grandi, LOW);
    digitalWrite(verdi_pin_grandi, HIGH);
    digitalWrite(blu_pin_grandi, LOW);
    digitalWrite(gialli_pin_grandi, HIGH);
    delay(t);
    digitalWrite(rosso_rgb1, LOW);
    digitalWrite(verde_rgb1, HIGH);
    digitalWrite(blu_rgb1, LOW);

    digitalWrite(rosso_rgb2, LOW);
    digitalWrite(verde_rgb2, LOW);
    digitalWrite(blu_rgb2, HIGH);

    digitalWrite(rosso_rgb3, LOW);
    digitalWrite(verde_rgb3, HIGH);
    digitalWrite(blu_rgb3, LOW);

    digitalWrite(bianchi_pin_piccoli, HIGH);
    digitalWrite(rossi_pin_piccoli, LOW);
    digitalWrite(verdi_pin_piccoli, HIGH);
    digitalWrite(blu_pin_piccoli, LOW);
    digitalWrite(gialli_pin_piccoli, HIGH);

    digitalWrite(bianchi_pin_grandi, LOW);
    digitalWrite(rossi_pin_grandi, HIGH);
    digitalWrite(verdi_pin_grandi, LOW);
    digitalWrite(blu_pin_grandi, HIGH);
    digitalWrite(gialli_pin_grandi, LOW);
    delay(t);

    digitalWrite(rosso_rgb1, HIGH);
    digitalWrite(verde_rgb1, HIGH);
    digitalWrite(blu_rgb1, HIGH);

    digitalWrite(rosso_rgb2, HIGH);
    digitalWrite(verde_rgb2, HIGH);
    digitalWrite(blu_rgb2, HIGH);

    digitalWrite(rosso_rgb3, HIGH);
    digitalWrite(verde_rgb3, HIGH);
    digitalWrite(blu_rgb3, HIGH);

    digitalWrite(bianchi_pin_piccoli, LOW);
    digitalWrite(rossi_pin_piccoli, HIGH);
    digitalWrite(verdi_pin_piccoli, LOW);
    digitalWrite(blu_pin_piccoli, HIGH);
    digitalWrite(gialli_pin_piccoli, LOW);

    digitalWrite(bianchi_pin_grandi, HIGH);
    digitalWrite(rossi_pin_grandi, LOW);
    digitalWrite(verdi_pin_grandi, HIGH);
    digitalWrite(blu_pin_grandi, LOW);
    digitalWrite(gialli_pin_grandi, HIGH);
    delay(t);
  }
}

void effetto_quattro_corridoio(int t) {
  if (cmd == 'h') {
    lcd_effetto_quattro_corridoio();
    for (int i = 0; i <= 255; i++) {
      analogWrite(rosso_rgb1, i);
      digitalWrite(verde_rgb1, LOW);
      digitalWrite(blu_rgb1, LOW);

      digitalWrite(rosso_rgb2, LOW);
      digitalWrite(verde_rgb2, HIGH);
      digitalWrite(blu_rgb2, LOW);

      digitalWrite(rosso_rgb3, LOW);
      digitalWrite(verde_rgb3, LOW);
      analogWrite(blu_rgb3, i);

      digitalWrite(bianchi_pin_piccoli, LOW);
      digitalWrite(rossi_pin_piccoli, LOW);
      digitalWrite(verdi_pin_piccoli, HIGH);
      digitalWrite(blu_pin_piccoli, LOW);
      digitalWrite(gialli_pin_piccoli, LOW);

      digitalWrite(bianchi_pin_grandi, LOW);
      digitalWrite(rossi_pin_grandi, LOW);
      digitalWrite(verdi_pin_grandi, HIGH);
      digitalWrite(blu_pin_grandi, LOW);
      digitalWrite(gialli_pin_grandi, LOW);
      delay(t);
    }

    for (int i = 0; i <= 255; i++) {
      digitalWrite(rosso_rgb1, LOW);
      analogWrite(verde_rgb1, i);
      digitalWrite(blu_rgb1, LOW);

      digitalWrite(rosso_rgb2, LOW);
      digitalWrite(verde_rgb2, LOW);
      digitalWrite(blu_rgb2, HIGH);

      analogWrite(rosso_rgb3, i);
      digitalWrite(verde_rgb3, LOW);
      digitalWrite(blu_rgb3, LOW);

      digitalWrite(bianchi_pin_piccoli, LOW);
      digitalWrite(rossi_pin_piccoli, HIGH);
      digitalWrite(verdi_pin_piccoli, LOW);
      digitalWrite(blu_pin_piccoli, LOW);
      digitalWrite(gialli_pin_piccoli, LOW);

      digitalWrite(bianchi_pin_grandi, LOW);
      digitalWrite(rossi_pin_grandi, HIGH);
      digitalWrite(verdi_pin_grandi, LOW);
      digitalWrite(blu_pin_grandi, LOW);
      digitalWrite(gialli_pin_grandi, LOW);
      delay(t);
    }

    for (int i = 0; i <= 255; i++) {
      digitalWrite(rosso_rgb1, LOW);
      digitalWrite(verde_rgb1, LOW);
      analogWrite(blu_rgb1, i);

      digitalWrite(rosso_rgb2, HIGH);
      digitalWrite(verde_rgb2, LOW);
      digitalWrite(blu_rgb2, LOW);

      digitalWrite(rosso_rgb3, LOW);
      analogWrite(verde_rgb3, i);
      digitalWrite(blu_rgb3, LOW);

      digitalWrite(bianchi_pin_piccoli, LOW);
      digitalWrite(rossi_pin_piccoli, LOW);
      digitalWrite(verdi_pin_piccoli, LOW);
      digitalWrite(blu_pin_piccoli, LOW);
      digitalWrite(gialli_pin_piccoli, HIGH);

      digitalWrite(bianchi_pin_grandi, LOW);
      digitalWrite(rossi_pin_grandi, LOW);
      digitalWrite(verdi_pin_grandi, LOW);
      digitalWrite(blu_pin_grandi, LOW);
      digitalWrite(gialli_pin_grandi, HIGH);
      delay(t);
    }

    for (int i = 0; i <= 255; i++) {
      analogWrite(rosso_rgb1, i);
      analogWrite(verde_rgb1, i);
      digitalWrite(blu_rgb1, LOW);

      digitalWrite(rosso_rgb2, LOW);
      digitalWrite(verde_rgb2, HIGH);
      digitalWrite(blu_rgb2, HIGH);

      analogWrite(rosso_rgb3, i);
      digitalWrite(verde_rgb3, LOW);
      analogWrite(blu_rgb3, i);

      digitalWrite(bianchi_pin_piccoli, HIGH);
      digitalWrite(rossi_pin_piccoli, LOW);
      digitalWrite(verdi_pin_piccoli, LOW);
      digitalWrite(blu_pin_piccoli, LOW);
      digitalWrite(gialli_pin_piccoli, LOW);

      digitalWrite(bianchi_pin_grandi, HIGH);
      digitalWrite(rossi_pin_grandi, LOW);
      digitalWrite(verdi_pin_grandi, LOW);
      digitalWrite(blu_pin_grandi, LOW);
      digitalWrite(gialli_pin_grandi, LOW);
      delay(t);
    }

    for (int i = 0; i <= 255; i++) {
      analogWrite(rosso_rgb1, i);
      analogWrite(verde_rgb1, i);
      analogWrite(blu_rgb1, i);

      digitalWrite(rosso_rgb2, HIGH);
      digitalWrite(verde_rgb2, HIGH);
      digitalWrite(blu_rgb2, HIGH);

      analogWrite(rosso_rgb3, i);
      analogWrite(verde_rgb3, i);
      analogWrite(blu_rgb3, i);

      digitalWrite(bianchi_pin_piccoli, LOW);
      digitalWrite(rossi_pin_piccoli, LOW);
      digitalWrite(verdi_pin_piccoli, LOW);
      digitalWrite(blu_pin_piccoli, HIGH);
      digitalWrite(gialli_pin_piccoli, LOW);

      digitalWrite(bianchi_pin_grandi, LOW);
      digitalWrite(rossi_pin_grandi, LOW);
      digitalWrite(verdi_pin_grandi, LOW);
      digitalWrite(blu_pin_grandi, HIGH);
      digitalWrite(gialli_pin_grandi, LOW);
      delay(t);
    }
  }
}

void effetto_cinque_corridoio(int t) {
  if (cmd == 'j') {
    lcd_effetto_cinque_corridoio();
    digitalWrite(rossi_pin_piccoli, HIGH);
    digitalWrite(gialli_pin_grandi, HIGH);

    digitalWrite(rosso_rgb1, HIGH);
    digitalWrite(verde_rgb1, LOW);
    digitalWrite(blu_rgb1, LOW);

    digitalWrite(rosso_rgb2, LOW);
    digitalWrite(verde_rgb2, HIGH);
    digitalWrite(blu_rgb2, LOW);

    digitalWrite(rosso_rgb3, LOW);
    digitalWrite(verde_rgb3, LOW);
    digitalWrite(blu_rgb3, HIGH);
    delay(t);
    digitalWrite(blu_pin_piccoli, HIGH);
    digitalWrite(verdi_pin_grandi, HIGH);
    digitalWrite(rosso_rgb1, LOW);
    digitalWrite(verde_rgb1, HIGH);
    digitalWrite(blu_rgb1, LOW);

    digitalWrite(rosso_rgb2, LOW);
    digitalWrite(verde_rgb2, LOW);
    digitalWrite(blu_rgb2, HIGH);

    digitalWrite(rosso_rgb3, HIGH);
    digitalWrite(verde_rgb3, LOW);
    digitalWrite(blu_rgb3, LOW);
    delay(t);
    digitalWrite(blu_pin_grandi, HIGH);
    digitalWrite(bianchi_pin_piccoli, HIGH);
    digitalWrite(rosso_rgb1, LOW);
    digitalWrite(verde_rgb1, LOW);
    digitalWrite(blu_rgb1, HIGH);

    digitalWrite(rosso_rgb2, HIGH);
    digitalWrite(verde_rgb2, LOW);
    digitalWrite(blu_rgb2, LOW);

    digitalWrite(rosso_rgb3, LOW);
    digitalWrite(verde_rgb3, HIGH);
    digitalWrite(blu_rgb3, LOW);
    delay(t);
    digitalWrite(rossi_pin_grandi, HIGH);
    digitalWrite(verdi_pin_piccoli, HIGH);
    digitalWrite(rosso_rgb1, HIGH);
    digitalWrite(verde_rgb1, LOW);
    digitalWrite(blu_rgb1, HIGH);

    digitalWrite(rosso_rgb2, HIGH);
    digitalWrite(verde_rgb2, LOW);
    digitalWrite(blu_rgb2, HIGH);

    digitalWrite(rosso_rgb3, HIGH);
    digitalWrite(verde_rgb3, LOW);
    digitalWrite(blu_rgb3, HIGH);
    delay(t);
    digitalWrite(gialli_pin_piccoli, HIGH);
    digitalWrite(bianchi_pin_grandi, HIGH);
    digitalWrite(rosso_rgb1, HIGH);
    digitalWrite(verde_rgb1, HIGH);
    digitalWrite(blu_rgb1, HIGH);

    digitalWrite(rosso_rgb2, HIGH);
    digitalWrite(verde_rgb2, HIGH);
    digitalWrite(blu_rgb2, HIGH);

    digitalWrite(rosso_rgb3, HIGH);
    digitalWrite(verde_rgb3, HIGH);
    digitalWrite(blu_rgb3, HIGH);
    delay(t);

    digitalWrite(rossi_pin_piccoli, LOW);
    digitalWrite(gialli_pin_grandi, LOW);
    digitalWrite(rosso_rgb1, LOW);
    digitalWrite(verde_rgb1, LOW);
    digitalWrite(blu_rgb1, LOW);

    digitalWrite(rosso_rgb2, LOW);
    digitalWrite(verde_rgb2, LOW);
    digitalWrite(blu_rgb2, LOW);

    digitalWrite(rosso_rgb3, LOW);
    digitalWrite(verde_rgb3, LOW);
    digitalWrite(blu_rgb3, LOW);
    delay(t);
    digitalWrite(blu_pin_piccoli, LOW);
    digitalWrite(verdi_pin_grandi, LOW);
    digitalWrite(rosso_rgb1, LOW);
    digitalWrite(verde_rgb1, LOW);
    digitalWrite(blu_rgb1, LOW);

    digitalWrite(rosso_rgb2, LOW);
    digitalWrite(verde_rgb2, LOW);
    digitalWrite(blu_rgb2, LOW);

    digitalWrite(rosso_rgb3, LOW);
    digitalWrite(verde_rgb3, LOW);
    digitalWrite(blu_rgb3, LOW);
    delay(t);
    digitalWrite(blu_pin_grandi, LOW);
    digitalWrite(bianchi_pin_piccoli, LOW);
    digitalWrite(rosso_rgb1, LOW);
    digitalWrite(verde_rgb1, LOW);
    digitalWrite(blu_rgb1, LOW);

    digitalWrite(rosso_rgb2, LOW);
    digitalWrite(verde_rgb2, LOW);
    digitalWrite(blu_rgb2, LOW);

    digitalWrite(rosso_rgb3, LOW);
    digitalWrite(verde_rgb3, LOW);
    digitalWrite(blu_rgb3, LOW);
    delay(t);
    digitalWrite(rossi_pin_grandi, LOW);
    digitalWrite(verdi_pin_piccoli, LOW);
    digitalWrite(rosso_rgb1, LOW);
    digitalWrite(verde_rgb1, LOW);
    digitalWrite(blu_rgb1, LOW);

    digitalWrite(rosso_rgb2, LOW);
    digitalWrite(verde_rgb2, LOW);
    digitalWrite(blu_rgb2, LOW);

    digitalWrite(rosso_rgb3, LOW);
    digitalWrite(verde_rgb3, LOW);
    digitalWrite(blu_rgb3, LOW);
    delay(t);
    digitalWrite(gialli_pin_piccoli, LOW);
    digitalWrite(bianchi_pin_grandi, LOW);
    digitalWrite(rosso_rgb1, LOW);
    digitalWrite(verde_rgb1, LOW);
    digitalWrite(blu_rgb1, LOW);

    digitalWrite(rosso_rgb2, LOW);
    digitalWrite(verde_rgb2, LOW);
    digitalWrite(blu_rgb2, LOW);

    digitalWrite(rosso_rgb3, LOW);
    digitalWrite(verde_rgb3, LOW);
    digitalWrite(blu_rgb3, LOW);
    delay(t);
  }
}

void effetto_sei_corridoio(int t) {
  if (cmd == 'k') {
    lcd_effetto_sei_corridoio();
    for (int i = 0; i <= 255 - i; i++) {
      analogWrite(rosso_rgb1, 0);
      analogWrite(verde_rgb1, 255 - i);
      analogWrite(blu_rgb1, i);

      digitalWrite(rosso_rgb2, LOW);
      digitalWrite(verde_rgb2, LOW);
      digitalWrite(blu_rgb2, HIGH);

      analogWrite(rosso_rgb3, i);
      analogWrite(verde_rgb3, 0);
      analogWrite(blu_rgb3, 255 - i);

      digitalWrite(bianchi_pin_piccoli, HIGH);
      digitalWrite(rossi_pin_piccoli, HIGH);
      digitalWrite(verdi_pin_piccoli, HIGH);
      digitalWrite(blu_pin_piccoli, HIGH);
      digitalWrite(gialli_pin_piccoli, HIGH);

      digitalWrite(bianchi_pin_grandi, LOW);
      digitalWrite(rossi_pin_grandi, LOW);
      digitalWrite(verdi_pin_grandi, LOW);
      digitalWrite(blu_pin_grandi, LOW);
      digitalWrite(gialli_pin_grandi, LOW);
      delay(t);
    }

    for (int i = 255; i >= 0 - i; i--) {
      analogWrite(rosso_rgb1, i - 255);
      analogWrite(verde_rgb1, i);
      analogWrite(blu_rgb1, 0);

      digitalWrite(rosso_rgb2, LOW);
      digitalWrite(verde_rgb2, HIGH);
      digitalWrite(blu_rgb2, LOW);

      analogWrite(rosso_rgb3, 0);
      analogWrite(verde_rgb3, i - 255);
      analogWrite(blu_rgb3, i);

      digitalWrite(bianchi_pin_piccoli, LOW);
      digitalWrite(rossi_pin_piccoli, LOW);
      digitalWrite(verdi_pin_piccoli, LOW);
      digitalWrite(blu_pin_piccoli, LOW);
      digitalWrite(gialli_pin_piccoli, LOW);

      digitalWrite(bianchi_pin_grandi, HIGH);
      digitalWrite(rossi_pin_grandi, HIGH);
      digitalWrite(verdi_pin_grandi, HIGH);
      digitalWrite(blu_pin_grandi, HIGH);
      digitalWrite(gialli_pin_grandi, HIGH);
      delay(t);
    }

    for (int i = 0; i <= 255 - i; i++) {
      analogWrite(rosso_rgb1, i);
      analogWrite(verde_rgb1, i);
      analogWrite(blu_rgb1, i);

      digitalWrite(rosso_rgb2, HIGH);
      digitalWrite(verde_rgb2, LOW);
      digitalWrite(blu_rgb2, LOW);

      analogWrite(rosso_rgb3, i);
      analogWrite(verde_rgb3, i);
      analogWrite(blu_rgb3, i);

      digitalWrite(bianchi_pin_piccoli, HIGH);
      digitalWrite(rossi_pin_piccoli, HIGH);
      digitalWrite(verdi_pin_piccoli, HIGH);
      digitalWrite(blu_pin_piccoli, HIGH);
      digitalWrite(gialli_pin_piccoli, HIGH);

      digitalWrite(bianchi_pin_grandi, HIGH);
      digitalWrite(rossi_pin_grandi, HIGH);
      digitalWrite(verdi_pin_grandi, HIGH);
      digitalWrite(blu_pin_grandi, HIGH);
      digitalWrite(gialli_pin_grandi, HIGH);
      delay(t);
    }
  }
}






//esterno
//porta principale aperta
void porta_principale_aperta() {
  if (cmd == 'l') {
    lcd_porta_principale_aperta();
    for (int i = 170; i >= 55; i--) {
      servo_pr.write(i);
      delay(tempo_apertura_porta_principale);
      cmd = ' ';
    }
  }
}

//porta principale chiusa
void porta_principale_chiusa() {
  if (cmd == 'z') {
    lcd_porta_principale_chiusa();
    for (int i = 55; i <= 170; i++) {
      servo_pr.write(i);
      delay(tempo_apertura_porta_principale);
      cmd = ' ';
    }
  }
}

//attivazione allarme
void allarme_on() {
  if (cmd == 'M') {
    lcd_allarme_on();
    if ((val_distanza < soglia_distanza) || (val_movimento == HIGH)) {
      dt_allarme = millis() - t_allarme;
      if (dt_allarme >= tempo_led_allarme) {
        stato_allarme = ! stato_allarme;
        digitalWrite(led_rosso_sensore_movimento, stato_allarme);
        digitalWrite(led_rosso_sensore_gas, stato_allarme);
        digitalWrite(led_rosso_sensore_fiamma, stato_allarme);
        digitalWrite(led_rosso_sensore_dht11, stato_allarme);
        digitalWrite(led_rosso_sensore_luminosita, stato_allarme);
        digitalWrite(led_rosso_sensore_distanza, stato_allarme);
        digitalWrite(buzzer1, stato_allarme);
        digitalWrite(buzzer2, !stato_allarme);

        digitalWrite(led_verde_sensore_movimento, LOW);
        digitalWrite(led_verde_sensore_gas, LOW);
        digitalWrite(led_verde_sensore_fiamma, LOW);
        digitalWrite(led_verde_sensore_dht11, LOW);
        digitalWrite(led_verde_sensore_luminosita, LOW);
        digitalWrite(led_verde_sensore_distanza, LOW);
        t_allarme = millis();
      }
    }
    else if ((val_distanza >= soglia_distanza + deboucing_distanza) || (val_movimento == LOW)) {
      digitalWrite(buzzer1, LOW);
      digitalWrite(buzzer2, LOW);
      digitalWrite(led_rosso_sensore_movimento, HIGH);
      digitalWrite(led_rosso_sensore_gas, HIGH);
      digitalWrite(led_rosso_sensore_fiamma, HIGH);
      digitalWrite(led_rosso_sensore_dht11, HIGH);
      digitalWrite(led_rosso_sensore_luminosita, HIGH);
      digitalWrite(led_rosso_sensore_distanza, HIGH);

      digitalWrite(led_verde_sensore_movimento, LOW);
      digitalWrite(led_verde_sensore_gas, LOW);
      digitalWrite(led_verde_sensore_fiamma, LOW);
      digitalWrite(led_verde_sensore_dht11, LOW);
      digitalWrite(led_verde_sensore_luminosita, LOW);
      digitalWrite(led_verde_sensore_distanza, LOW);
    }
  }
}

//disattivazione allarme
void allarme_off() {
  if (cmd == 'N') {
    lcd_allarme_off();
    digitalWrite(buzzer1, LOW);
    digitalWrite(buzzer2, LOW);

    digitalWrite(led_verde_sensore_movimento, HIGH);
    digitalWrite(led_verde_sensore_gas, HIGH);
    digitalWrite(led_verde_sensore_fiamma, HIGH);
    digitalWrite(led_verde_sensore_dht11, HIGH);
    digitalWrite(led_verde_sensore_luminosita, HIGH);
    digitalWrite(led_verde_sensore_distanza, HIGH);

    digitalWrite(led_rosso_sensore_movimento, LOW);
    digitalWrite(led_rosso_sensore_gas, LOW);
    digitalWrite(led_rosso_sensore_fiamma, LOW);
    digitalWrite(led_rosso_sensore_dht11, LOW);
    digitalWrite(led_rosso_sensore_luminosita, LOW);
    digitalWrite(led_rosso_sensore_distanza, LOW);

    delay(500);

    digitalWrite(led_verde_sensore_movimento, LOW);
    digitalWrite(led_verde_sensore_gas, LOW);
    digitalWrite(led_verde_sensore_fiamma, LOW);
    digitalWrite(led_verde_sensore_dht11, LOW);
    digitalWrite(led_verde_sensore_luminosita, LOW);
    digitalWrite(led_verde_sensore_distanza, LOW);

    digitalWrite(led_rosso_sensore_movimento, HIGH);
    digitalWrite(led_rosso_sensore_gas, HIGH);
    digitalWrite(led_rosso_sensore_fiamma, HIGH);
    digitalWrite(led_rosso_sensore_dht11, HIGH);
    digitalWrite(led_rosso_sensore_luminosita, HIGH);
    digitalWrite(led_rosso_sensore_distanza, HIGH);

    cmd = ' ';
  }
}

//led fontanella accesi
void led_fontanella_accesi() {
  if (cmd == 'Q') {
    lcd_led_fontanella_accesi();
    digitalWrite(led_pompa_acqua, HIGH);
  }
}

//led fontanella spenti
void led_fontanella_spenti() {
  if (cmd == 'R') {
    lcd_led_fontanella_spenti();
    digitalWrite(led_pompa_acqua, LOW);  //6 in totale (3 effetto veloce e 3 effetto lento)
  }
}

//accensione fontanella
void accensione_fontanella() {
  if (cmd == 'O') {
    lcd_accensione_fontanella();
    digitalWrite(pompa_acqua, LOW);
  }
}

//spegnimento fontanella
void spegnimento_fontanella() {
  if (cmd == 'P') {
    lcd_spegnimento_fontanella();
    digitalWrite(pompa_acqua, HIGH); //spengo la pompa dell'acqua
    digitalWrite(led_pompa_acqua, LOW); //spengo i led
  }
}











////////////////////////////sensori (compresi i sensori all'esterno della struttura)////////////////////////////

//tutti i sensori attivati --> i buzzer sono disattivati perchè se un sensore è attivo spengo il buzzer ma se un altro è attivo lo devo attivare pertanto risulta impossibile procedere perchè il buzzer risulterebbe attivo e spento allo stesso momento
void tutti_sensori_on() {
  if (cmd == 'K') {
    lcd_tutti_sensori_on();
    digitalWrite(led_verde_sensore_gas, HIGH);
    digitalWrite(led_verde_sensore_movimento, HIGH);
    digitalWrite(led_verde_sensore_fiamma, HIGH);
    digitalWrite(led_verde_sensore_dht11, HIGH);
    digitalWrite(led_verde_sensore_distanza, HIGH);
    digitalWrite(led_verde_sensore_luminosita, HIGH);

    digitalWrite(led_rosso_sensore_gas, LOW);
    digitalWrite(led_rosso_sensore_movimento, LOW);
    digitalWrite(led_rosso_sensore_fiamma, LOW);
    digitalWrite(led_rosso_sensore_dht11, LOW);
    digitalWrite(led_rosso_sensore_distanza, LOW);
    digitalWrite(led_rosso_sensore_luminosita, LOW);

    //gas
    if (val_gas >= soglia_gas + deboucing_gas) {
      dt_gas = millis() - t_gas;
      if (dt_gas >= tempo_led_allarme_gas ) {
        stato_gas = ! stato_gas;
        digitalWrite(led_rosso_sensore_gas, stato_gas);
        digitalWrite(led_verde_sensore_gas, LOW);
        t_gas = millis();
      }
    } else if (val_gas < soglia_gas) {
      digitalWrite(led_rosso_sensore_gas, LOW);
      digitalWrite(led_verde_sensore_gas, HIGH);
    }

    //fiamma
    if (val_fiamma >= soglia_fiamma + deboucing_fiamma) {
      dt_fiamma = millis() - t_fiamma;
      if (dt_fiamma >= tempo_led_allarme_fiamma ) {
        stato_fiamma = ! stato_fiamma;
        digitalWrite(led_rosso_sensore_fiamma, stato_fiamma);
        digitalWrite(led_verde_sensore_fiamma, LOW);
        t_fiamma = millis();
      }
    } else if (val_fiamma < soglia_fiamma) {
      digitalWrite(led_rosso_sensore_fiamma, LOW);
      digitalWrite(led_verde_sensore_fiamma, HIGH);
    }

    //temperatura e umidità
    if (temperatura >= soglia_dht11 + deboucing_dht11) {
      dt_dht11 = millis() - t_dht11;
      if (dt_dht11 >= tempo_led_allarme_dht11 ) {
        stato_dht11 = ! stato_dht11;
        digitalWrite(led_rosso_sensore_dht11, stato_dht11);
        digitalWrite(led_verde_sensore_dht11, LOW);
        digitalWrite(ventola, HIGH);
        t_dht11 = millis();
      }
    } else if (temperatura < soglia_dht11) {
      digitalWrite(led_rosso_sensore_dht11, LOW);
      digitalWrite(led_verde_sensore_dht11, HIGH);
      digitalWrite(ventola, LOW);
    }

    //distanza
    if (val_distanza < soglia_distanza) {
      dt_distanza = millis() - t_distanza;
      if (dt_distanza >= tempo_led_allarme_distanza ) {
        stato_distanza = ! stato_distanza;
        digitalWrite(led_rosso_sensore_distanza, stato_distanza);
        digitalWrite(led_verde_sensore_distanza, LOW);
        t_distanza = millis();
      }
    } else if (val_distanza >= soglia_distanza + deboucing_distanza) {
      digitalWrite(led_rosso_sensore_distanza, LOW);
      digitalWrite(led_verde_sensore_distanza, HIGH);
    }

    //movimento
    if (val_movimento == HIGH) {
      dt_movimento = millis() - t_movimento;
      if (dt_movimento >= tempo_led_allarme_movimento) {
        stato_movimento = ! stato_movimento;
        digitalWrite(led_rosso_sensore_movimento, stato_movimento );
        digitalWrite(led_verde_sensore_movimento, LOW);
        t_movimento = millis();
      }
    } else if (val_movimento == LOW) {
      digitalWrite(led_rosso_sensore_movimento, LOW);
      digitalWrite(led_verde_sensore_movimento, HIGH);
    }

    //luminosita
    if ((val_fotoresistenza_1 <= luminosita_tramonto) && (val_fotoresistenza_2 <= luminosita_tramonto)) {
      //accendi luci giardino
      digitalWrite(led_giardino, HIGH);

    } else if ((val_fotoresistenza_1 >= luminosita_tramonto + 10) && (val_fotoresistenza_2 >= luminosita_tramonto + 10)) {
      //spegni luci giardino
      digitalWrite(led_giardino, LOW);
    }
  }
}

//tutti i sensori disattivati
void tutti_sensori_off() {
  if (cmd == 'L') {
    lcd_tutti_sensori_off();
    digitalWrite(buzzer1, LOW);
    digitalWrite(buzzer2, LOW);

    digitalWrite(led_verde_sensore_gas, LOW);
    digitalWrite(led_verde_sensore_movimento, LOW);
    digitalWrite(led_verde_sensore_fiamma, LOW);
    digitalWrite(led_verde_sensore_dht11, LOW);
    digitalWrite(led_verde_sensore_distanza, LOW);
    digitalWrite(led_verde_sensore_luminosita, LOW);
    digitalWrite(ventola, LOW);

    digitalWrite(led_rosso_sensore_gas, HIGH);
    digitalWrite(led_rosso_sensore_movimento, HIGH);
    digitalWrite(led_rosso_sensore_fiamma, HIGH);
    digitalWrite(led_rosso_sensore_dht11, HIGH);
    digitalWrite(led_rosso_sensore_distanza, HIGH);
    digitalWrite(led_rosso_sensore_luminosita, HIGH);
  }
}

//lettura sensore luminosita
void lettura_sensore_luminosita() {
  Serial.print("resistenza_1:  ");
  Serial.print(val_fotoresistenza_1);
  Serial.print('\t');
  Serial.print("       resistenza_2:  ");
  Serial.println(val_fotoresistenza_2);
  delay(150);
}

//fororesistenza attivata
void luminosita_on() {
  if (cmd == 'x') {
    lcd_luminosita_on();
    digitalWrite(led_verde_sensore_movimento, LOW);
    digitalWrite(led_verde_sensore_gas, LOW);
    digitalWrite(led_verde_sensore_fiamma, LOW);
    digitalWrite(led_verde_sensore_dht11, LOW);
    digitalWrite(led_verde_sensore_distanza, LOW);

    digitalWrite(led_rosso_sensore_movimento, HIGH);
    digitalWrite(led_rosso_sensore_gas, HIGH);
    digitalWrite(led_rosso_sensore_fiamma, HIGH);
    digitalWrite(led_rosso_sensore_dht11, HIGH);
    digitalWrite(led_rosso_sensore_distanza, HIGH);

    digitalWrite(led_verde_sensore_luminosita, HIGH);
    digitalWrite(led_rosso_sensore_luminosita, LOW);
    if ((val_fotoresistenza_1 <= luminosita_tramonto) && (val_fotoresistenza_2 <= luminosita_tramonto)) {
      //accendi luci giardino
      digitalWrite(led_giardino, HIGH);

    } else if ((val_fotoresistenza_1 >= luminosita_tramonto + 10) && (val_fotoresistenza_2 >= luminosita_tramonto + 10)) {
      //spegni luci giardino
      digitalWrite(led_giardino, LOW);
    }
  }
}

//fotoresistenza disattivata
void luminosita_off() {
  if (cmd == 'c') {
    lcd_luminosita_off();
    digitalWrite(led_giardino, LOW);
    digitalWrite(led_rosso_sensore_luminosita, HIGH);
    digitalWrite(led_verde_sensore_luminosita, LOW);
  }
}

//illuminazione giardino on
void illuminazione_giardino_on() {
  if (cmd == 'v') {
    lcd_illuminazione_giardino_on();
    digitalWrite(led_giardino, HIGH);
  }
}

//illuminazione giardino off
void illuminazione_giardino_off() {
  if (cmd == 'b') {
    lcd_illuminazione_giardino_off();
    digitalWrite(led_giardino, LOW);
  }
}

//lettura sensore gas
void lettura_sensore_gas() {
  Serial.println(val_gas);
  delay(100);
}

//gas attivato
void gas_on() {
  if (cmd == 'n') {
    lcd_gas_on();
    digitalWrite(led_verde_sensore_movimento, LOW);
    digitalWrite(led_verde_sensore_fiamma, LOW);
    digitalWrite(led_verde_sensore_dht11, LOW);
    digitalWrite(led_verde_sensore_distanza, LOW);
    digitalWrite(led_verde_sensore_luminosita, LOW);

    digitalWrite(led_rosso_sensore_movimento, HIGH);
    digitalWrite(led_rosso_sensore_fiamma, HIGH);
    digitalWrite(led_rosso_sensore_dht11, HIGH);
    digitalWrite(led_rosso_sensore_distanza, HIGH);
    digitalWrite(led_rosso_sensore_luminosita, HIGH);

    if (val_gas >= soglia_gas + deboucing_gas) {
      dt_gas = millis() - t_gas;
      if (dt_gas >= tempo_led_allarme_gas ) {
        stato_gas = ! stato_gas;
        digitalWrite(led_rosso_sensore_gas, stato_gas);
        digitalWrite(buzzer1, stato_gas);
        digitalWrite(buzzer2, !stato_gas); //!stato_gas per il suono alternato
        digitalWrite(led_verde_sensore_gas, LOW);
        t_gas = millis();
      }
    } else if (val_gas < soglia_gas) {
      digitalWrite(buzzer1, LOW);
      digitalWrite(buzzer2, LOW);
      digitalWrite(led_rosso_sensore_gas, LOW);
      digitalWrite(led_verde_sensore_gas, HIGH);
    }
  }
}

//gas disattivato
void gas_off() {
  if (cmd == 'm') {
    lcd_gas_off();
    digitalWrite(buzzer1, LOW);
    digitalWrite(buzzer2, LOW);
    digitalWrite(led_rosso_sensore_gas, HIGH);
    digitalWrite(led_verde_sensore_gas, LOW);
    //accensione led rosso di controllo disabilitazione
    //spegnimento led verde di controllo abilitazione
    //spegnimento dei buzzer
  }
}

//lettura sensore fiamma
void lettura_sensore_fiamma() {
  Serial.println(val_fiamma);
  delay(100);
}

//fiamma attivata
void fiamma_on() {
  if (cmd == 'A') {
    lcd_fiamma_on();
    digitalWrite(led_verde_sensore_movimento, LOW);
    digitalWrite(led_verde_sensore_gas, LOW);
    digitalWrite(led_verde_sensore_dht11, LOW);
    digitalWrite(led_verde_sensore_distanza, LOW);
    digitalWrite(led_verde_sensore_luminosita, LOW);

    digitalWrite(led_rosso_sensore_movimento, HIGH);
    digitalWrite(led_rosso_sensore_gas, HIGH);
    digitalWrite(led_rosso_sensore_dht11, HIGH);
    digitalWrite(led_rosso_sensore_distanza, HIGH);
    digitalWrite(led_rosso_sensore_luminosita, HIGH);

    if (val_fiamma >= soglia_fiamma + deboucing_fiamma) {
      dt_fiamma = millis() - t_fiamma;
      if (dt_fiamma >= tempo_led_allarme_fiamma ) {
        stato_fiamma = ! stato_fiamma;
        digitalWrite(led_rosso_sensore_fiamma, stato_fiamma);
        digitalWrite(buzzer1, stato_fiamma);
        digitalWrite(buzzer2, !stato_fiamma);
        digitalWrite(led_verde_sensore_fiamma, LOW);
        t_fiamma = millis();
      }
    } else if (val_fiamma < soglia_fiamma) {
      digitalWrite(buzzer1, LOW);
      digitalWrite(buzzer2, LOW);
      digitalWrite(led_rosso_sensore_fiamma, LOW);
      digitalWrite(led_verde_sensore_fiamma, HIGH);
    }
  }
}

//fiamma disattivata
void fiamma_off() {
  if (cmd == 'B') {
    lcd_fiamma_off();
    digitalWrite(buzzer1, LOW);
    digitalWrite(buzzer2, LOW);
    digitalWrite(led_rosso_sensore_fiamma, HIGH);
    digitalWrite(led_verde_sensore_fiamma, LOW);
    //accensione led rosso di controllo disabilitazione
    //spegnimento led verde di controllo abilitazione
    //spegnimento dei buzzer
  }
}

//lettura sensore temperatura e umidità
void lettura_senore_dht11() {
  Serial.print ("Temp.:");
  Serial.print ((int) dht.readTemperature());  //int per ottenere un valore intero (float per un numero con virgola)
  Serial.print ("°C");   //nei lcd il simbolo "°" viene rappresentato con "char (0xDF)"
  Serial.print ('\t');
  Serial.print ("umid.: " );
  Serial.print((int) dht.readHumidity());     //int per ottenere un valore intero (float per un numero con virgola)
  Serial.println("%");
  delay(500);
}

//temperatura e umidità attivata
void dht11_on() {
  if (cmd == 'C') {
    lcd_dht11_on();
    digitalWrite(led_verde_sensore_movimento, LOW);
    digitalWrite(led_verde_sensore_gas, LOW);
    digitalWrite(led_verde_sensore_fiamma, LOW);
    digitalWrite(led_verde_sensore_distanza, LOW);
    digitalWrite(led_verde_sensore_luminosita, LOW);

    digitalWrite(led_rosso_sensore_movimento, HIGH);
    digitalWrite(led_rosso_sensore_gas, HIGH);
    digitalWrite(led_rosso_sensore_fiamma, HIGH);
    digitalWrite(led_rosso_sensore_distanza, HIGH);
    digitalWrite(led_rosso_sensore_luminosita, HIGH);

    if (temperatura >= soglia_dht11 + deboucing_dht11) {
      dt_dht11 = millis() - t_dht11;
      if (dt_dht11 >= tempo_led_allarme_dht11 ) {
        stato_dht11 = ! stato_dht11;
        digitalWrite(led_rosso_sensore_dht11, stato_dht11);
        digitalWrite(led_verde_sensore_dht11, LOW);
        digitalWrite(ventola, HIGH);
        t_dht11 = millis();
      }
    } else if (temperatura < soglia_dht11) {
      digitalWrite(led_rosso_sensore_dht11, LOW);
      digitalWrite(led_verde_sensore_dht11, HIGH);
      digitalWrite(ventola, LOW);
    }
  }
}

//temperatura e umidità disattivata
void dht11_off() {
  if (cmd == 'D') {
    lcd_dht11_off();
    digitalWrite(led_rosso_sensore_dht11, HIGH);
    digitalWrite(led_verde_sensore_dht11, LOW);
    digitalWrite(ventola, LOW);
  }
}

//ventola on
void ventola_on() {
  if (cmd == '%') {
    lcd_ventola_on();
    digitalWrite(ventola, HIGH);
  }
}

//ventola off
void ventola_off() {
  if (cmd == '&') {
    lcd_ventola_off();
    digitalWrite(ventola, LOW);
  }
}

//lettura sensore distanza
void lettura_senore_distanza() {
  Serial.print(val_distanza);
  Serial.println(" cm");
  delay(500);
}

//distanza attivata
void distanza_on() {
  if (cmd == 'E') {
    lcd_distanza_on();
    digitalWrite(led_verde_sensore_movimento, LOW);
    digitalWrite(led_verde_sensore_gas, LOW);
    digitalWrite(led_verde_sensore_fiamma, LOW);
    digitalWrite(led_verde_sensore_dht11, LOW);
    digitalWrite(led_verde_sensore_luminosita, LOW);

    digitalWrite(led_rosso_sensore_movimento, HIGH);
    digitalWrite(led_rosso_sensore_gas, HIGH);
    digitalWrite(led_rosso_sensore_fiamma, HIGH);
    digitalWrite(led_rosso_sensore_dht11, HIGH);
    digitalWrite(led_rosso_sensore_luminosita, HIGH);

    if (val_distanza < soglia_distanza) {
      dt_distanza = millis() - t_distanza;
      if (dt_distanza >= tempo_led_allarme_distanza ) {
        stato_distanza = ! stato_distanza;
        digitalWrite(led_rosso_sensore_distanza, stato_distanza);
        digitalWrite(buzzer1, stato_distanza);
        digitalWrite(buzzer2, !stato_distanza);
        digitalWrite(led_verde_sensore_distanza, LOW);
        t_distanza = millis();
      }
    } else if (val_distanza >= soglia_distanza + deboucing_distanza) {
      digitalWrite(buzzer1, LOW);
      digitalWrite(buzzer2, LOW);
      digitalWrite(led_rosso_sensore_distanza, LOW);
      digitalWrite(led_verde_sensore_distanza, HIGH);
    }
  }
}

//distanza disattivata
void distanza_off() {
  if (cmd == 'F') {
    lcd_distanza_off();
    digitalWrite(buzzer1, LOW);
    digitalWrite(buzzer2, LOW);
    digitalWrite(led_rosso_sensore_distanza, HIGH);
    digitalWrite(led_verde_sensore_distanza, LOW);
  }
}

//lettura sensore movimento
void lettura_sensore_movimento() {
  Serial.println(val_movimento);
  delay(100);
}

//movimento attivato
void movimento_on() {
  if (cmd == 'I') {
    lcd_movimento_on();
    digitalWrite(led_verde_sensore_gas, LOW);
    digitalWrite(led_verde_sensore_fiamma, LOW);
    digitalWrite(led_verde_sensore_dht11, LOW);
    digitalWrite(led_verde_sensore_distanza, LOW);
    digitalWrite(led_verde_sensore_luminosita, LOW);

    digitalWrite(led_rosso_sensore_gas, HIGH);
    digitalWrite(led_rosso_sensore_fiamma, HIGH);
    digitalWrite(led_rosso_sensore_dht11, HIGH);
    digitalWrite(led_rosso_sensore_distanza, HIGH);
    digitalWrite(led_rosso_sensore_luminosita, HIGH);

    if (val_movimento == HIGH) {
      dt_movimento = millis() - t_movimento;
      if (dt_movimento >= tempo_led_allarme_movimento) {
        stato_movimento = ! stato_movimento;
        digitalWrite(led_rosso_sensore_movimento, stato_movimento );
        digitalWrite(buzzer1, stato_movimento);
        digitalWrite(buzzer2, !stato_movimento);
        digitalWrite(led_verde_sensore_movimento, LOW);
        t_movimento = millis();
      }
    } else if (val_movimento == LOW) {
      digitalWrite(buzzer1, LOW);
      digitalWrite(buzzer2, LOW);
      digitalWrite(led_rosso_sensore_movimento, LOW);
      digitalWrite(led_verde_sensore_movimento, HIGH);
    }
  }
}

//movimento disattivato
void movimento_off() {
  if (cmd == 'J') {
    lcd_movimento_off();
    digitalWrite(buzzer1, LOW);
    digitalWrite(buzzer2, LOW);
    digitalWrite(led_rosso_sensore_movimento, HIGH);
    digitalWrite(led_verde_sensore_movimento, LOW);
  }
}


//LCD

//lcd "ciao Andrea! Connessione BT"
void lcd_testo_iniziale() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" ");
  lcd.setCursor(1, 0);
  lcd.print(" ");
  lcd.setCursor(2, 0);
  lcd.print("C");
  lcd.setCursor(3, 0);
  lcd.print("i");
  lcd.setCursor(4, 0);
  lcd.print("a");
  lcd.setCursor(5, 0);
  lcd.print("o");
  lcd.setCursor(6, 0);
  lcd.print(" ");
  lcd.setCursor(7, 0);
  lcd.print("A");
  lcd.setCursor(8, 0);
  lcd.print("n");
  lcd.setCursor(9, 0);
  lcd.print("d");
  lcd.setCursor(10, 0);
  lcd.print("r");
  lcd.setCursor(11, 0);
  lcd.print("e");
  lcd.setCursor(12, 0);
  lcd.print("a");
  lcd.setCursor(13, 0);
  lcd.print("!");
  lcd.setCursor(14, 0);
  lcd.print(" ");
  lcd.setCursor(15, 0);
  lcd.print(" ");

  lcd.setCursor(0, 1);
  lcd.print(" ");
  lcd.setCursor(1, 1);
  lcd.print("C");
  lcd.setCursor(2, 1);
  lcd.print("o");
  lcd.setCursor(3, 1);
  lcd.print("n");
  lcd.setCursor(4, 1);
  lcd.print("n");
  lcd.setCursor(5, 1);
  lcd.print("e");
  lcd.setCursor(6, 1);
  lcd.print("s");
  lcd.setCursor(7, 1);
  lcd.print("s");
  lcd.setCursor(8, 1);
  lcd.print("i");
  lcd.setCursor(9, 1);
  lcd.print("o");
  lcd.setCursor(10, 1);
  lcd.print("n");
  lcd.setCursor(11, 1);
  lcd.print("e");
  lcd.setCursor(12, 1);
  lcd.print(" ");
  lcd.setCursor(13, 1);
  lcd.print("B");
  lcd.setCursor(14, 1);
  lcd.print("T");
  lcd.setCursor(15, 1);
  lcd.print(" ");
}

//lcd "Corridoio tutti i led OFF"
void lcd_tutti_led_off_corridoio() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("C");
  lcd.setCursor(1, 0);
  lcd.print("o");
  lcd.setCursor(2, 0);
  lcd.print("r");
  lcd.setCursor(3, 0);
  lcd.print("r");
  lcd.setCursor(4, 0);
  lcd.print("i");
  lcd.setCursor(5, 0);
  lcd.print("d");
  lcd.setCursor(6, 0);
  lcd.print("o");
  lcd.setCursor(7, 0);
  lcd.print("i");
  lcd.setCursor(8, 0);
  lcd.print("o");
  lcd.setCursor(9, 0);
  lcd.print("");
  lcd.setCursor(10, 0);
  lcd.print("");
  lcd.setCursor(11, 0);
  lcd.print("");
  lcd.setCursor(12, 0);
  lcd.print("");
  lcd.setCursor(13, 0);
  lcd.print("");
  lcd.setCursor(14, 0);
  lcd.print("");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("t");
  lcd.setCursor(1, 1);
  lcd.print("u");
  lcd.setCursor(2, 1);
  lcd.print("t");
  lcd.setCursor(3, 1);
  lcd.print("t");
  lcd.setCursor(4, 1);
  lcd.print("i");
  lcd.setCursor(5, 1);
  lcd.print("");
  lcd.setCursor(6, 1);
  lcd.print("i");
  lcd.setCursor(7, 1);
  lcd.print("");
  lcd.setCursor(8, 1);
  lcd.print("L");
  lcd.setCursor(9, 1);
  lcd.print("E");
  lcd.setCursor(10, 1);
  lcd.print("D");
  lcd.setCursor(11, 1);
  lcd.print("");
  lcd.setCursor(12, 1);
  lcd.print("O");
  lcd.setCursor(13, 1);
  lcd.print("F");
  lcd.setCursor(14, 1);
  lcd.print("F");
  lcd.setCursor(15, 1);
  lcd.print("");
  delay(w);
}

//lcd "Corridoio tutti i led ON"
void lcd_tutti_led_on_corridoio() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("C");
  lcd.setCursor(1, 0);
  lcd.print("o");
  lcd.setCursor(2, 0);
  lcd.print("r");
  lcd.setCursor(3, 0);
  lcd.print("r");
  lcd.setCursor(4, 0);
  lcd.print("i");
  lcd.setCursor(5, 0);
  lcd.print("d");
  lcd.setCursor(6, 0);
  lcd.print("o");
  lcd.setCursor(7, 0);
  lcd.print("i");
  lcd.setCursor(8, 0);
  lcd.print("o");
  lcd.setCursor(9, 0);
  lcd.print("");
  lcd.setCursor(10, 0);
  lcd.print("");
  lcd.setCursor(11, 0);
  lcd.print("");
  lcd.setCursor(12, 0);
  lcd.print("");
  lcd.setCursor(13, 0);
  lcd.print("");
  lcd.setCursor(14, 0);
  lcd.print("");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("t");
  lcd.setCursor(1, 1);
  lcd.print("u");
  lcd.setCursor(2, 1);
  lcd.print("t");
  lcd.setCursor(3, 1);
  lcd.print("t");
  lcd.setCursor(4, 1);
  lcd.print("i");
  lcd.setCursor(5, 1);
  lcd.print("");
  lcd.setCursor(6, 1);
  lcd.print("i");
  lcd.setCursor(7, 1);
  lcd.print("");
  lcd.setCursor(8, 1);
  lcd.print("L");
  lcd.setCursor(9, 1);
  lcd.print("E");
  lcd.setCursor(10, 1);
  lcd.print("D");
  lcd.setCursor(11, 1);
  lcd.print("");
  lcd.setCursor(12, 1);
  lcd.print("O");
  lcd.setCursor(13, 1);
  lcd.print("N");
  lcd.setCursor(14, 1);
  lcd.print("");
  lcd.setCursor(15, 1);
  lcd.print("");
  delay(w);
}

//lcd "Corridoio LED rossi OFF"
void lcd_rossi_off_corridoio() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("C");
  lcd.setCursor(1, 0);
  lcd.print("o");
  lcd.setCursor(2, 0);
  lcd.print("r");
  lcd.setCursor(3, 0);
  lcd.print("r");
  lcd.setCursor(4, 0);
  lcd.print("i");
  lcd.setCursor(5, 0);
  lcd.print("d");
  lcd.setCursor(6, 0);
  lcd.print("o");
  lcd.setCursor(7, 0);
  lcd.print("i");
  lcd.setCursor(8, 0);
  lcd.print("o");
  lcd.setCursor(9, 0);
  lcd.print("");
  lcd.setCursor(10, 0);
  lcd.print("");
  lcd.setCursor(11, 0);
  lcd.print("");
  lcd.setCursor(12, 0);
  lcd.print("");
  lcd.setCursor(13, 0);
  lcd.print("");
  lcd.setCursor(14, 0);
  lcd.print("");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("L");
  lcd.setCursor(1, 1);
  lcd.print("E");
  lcd.setCursor(2, 1);
  lcd.print("D");
  lcd.setCursor(3, 1);
  lcd.print(" ");
  lcd.setCursor(4, 1);
  lcd.print("r");
  lcd.setCursor(5, 1);
  lcd.print("o");
  lcd.setCursor(6, 1);
  lcd.print("s");
  lcd.setCursor(7, 1);
  lcd.print("s");
  lcd.setCursor(8, 1);
  lcd.print("i");
  lcd.setCursor(9, 1);
  lcd.print(" ");
  lcd.setCursor(10, 1);
  lcd.print("O");
  lcd.setCursor(11, 1);
  lcd.print("F");
  lcd.setCursor(12, 1);
  lcd.print("F");
  lcd.setCursor(13, 1);
  lcd.print("");
  lcd.setCursor(14, 1);
  lcd.print("");
  lcd.setCursor(15, 1);
  lcd.print("");
  delay(w);
}

//lcd "Corridoio LED rossi ON"
void lcd_led_rossi_on_corridoio() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("C");
  lcd.setCursor(1, 0);
  lcd.print("o");
  lcd.setCursor(2, 0);
  lcd.print("r");
  lcd.setCursor(3, 0);
  lcd.print("r");
  lcd.setCursor(4, 0);
  lcd.print("i");
  lcd.setCursor(5, 0);
  lcd.print("d");
  lcd.setCursor(6, 0);
  lcd.print("o");
  lcd.setCursor(7, 0);
  lcd.print("i");
  lcd.setCursor(8, 0);
  lcd.print("o");
  lcd.setCursor(9, 0);
  lcd.print("");
  lcd.setCursor(10, 0);
  lcd.print("");
  lcd.setCursor(11, 0);
  lcd.print("");
  lcd.setCursor(12, 0);
  lcd.print("");
  lcd.setCursor(13, 0);
  lcd.print("");
  lcd.setCursor(14, 0);
  lcd.print("");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("L");
  lcd.setCursor(1, 1);
  lcd.print("E");
  lcd.setCursor(2, 1);
  lcd.print("D");
  lcd.setCursor(3, 1);
  lcd.print(" ");
  lcd.setCursor(4, 1);
  lcd.print("r");
  lcd.setCursor(5, 1);
  lcd.print("o");
  lcd.setCursor(6, 1);
  lcd.print("s");
  lcd.setCursor(7, 1);
  lcd.print("s");
  lcd.setCursor(8, 1);
  lcd.print("i");
  lcd.setCursor(9, 1);
  lcd.print(" ");
  lcd.setCursor(10, 1);
  lcd.print("O");
  lcd.setCursor(11, 1);
  lcd.print("N");
  lcd.setCursor(12, 1);
  lcd.print("");
  lcd.setCursor(13, 1);
  lcd.print("");
  lcd.setCursor(14, 1);
  lcd.print("");
  lcd.setCursor(15, 1);
  lcd.print("");
  delay(w);
}

//lcd "Corridoio LED verdi OFF"
void lcd_led_verdi_off_corridoio() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("C");
  lcd.setCursor(1, 0);
  lcd.print("o");
  lcd.setCursor(2, 0);
  lcd.print("r");
  lcd.setCursor(3, 0);
  lcd.print("r");
  lcd.setCursor(4, 0);
  lcd.print("i");
  lcd.setCursor(5, 0);
  lcd.print("d");
  lcd.setCursor(6, 0);
  lcd.print("o");
  lcd.setCursor(7, 0);
  lcd.print("i");
  lcd.setCursor(8, 0);
  lcd.print("o");
  lcd.setCursor(9, 0);
  lcd.print("");
  lcd.setCursor(10, 0);
  lcd.print("");
  lcd.setCursor(11, 0);
  lcd.print("");
  lcd.setCursor(12, 0);
  lcd.print("");
  lcd.setCursor(13, 0);
  lcd.print("");
  lcd.setCursor(14, 0);
  lcd.print("");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("L");
  lcd.setCursor(1, 1);
  lcd.print("E");
  lcd.setCursor(2, 1);
  lcd.print("D");
  lcd.setCursor(3, 1);
  lcd.print(" ");
  lcd.setCursor(4, 1);
  lcd.print("v");
  lcd.setCursor(5, 1);
  lcd.print("e");
  lcd.setCursor(6, 1);
  lcd.print("r");
  lcd.setCursor(7, 1);
  lcd.print("d");
  lcd.setCursor(8, 1);
  lcd.print("i");
  lcd.setCursor(9, 1);
  lcd.print(" ");
  lcd.setCursor(10, 1);
  lcd.print("O");
  lcd.setCursor(11, 1);
  lcd.print("F");
  lcd.setCursor(12, 1);
  lcd.print("F");
  lcd.setCursor(13, 1);
  lcd.print("");
  lcd.setCursor(14, 1);
  lcd.print("");
  lcd.setCursor(15, 1);
  lcd.print("");
  delay(w);
}

//lcd "Corridoio LED verdi ON"
void lcd_led_verdi_on_corridoio() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("C");
  lcd.setCursor(1, 0);
  lcd.print("o");
  lcd.setCursor(2, 0);
  lcd.print("r");
  lcd.setCursor(3, 0);
  lcd.print("r");
  lcd.setCursor(4, 0);
  lcd.print("i");
  lcd.setCursor(5, 0);
  lcd.print("d");
  lcd.setCursor(6, 0);
  lcd.print("o");
  lcd.setCursor(7, 0);
  lcd.print("i");
  lcd.setCursor(8, 0);
  lcd.print("o");
  lcd.setCursor(9, 0);
  lcd.print("");
  lcd.setCursor(10, 0);
  lcd.print("");
  lcd.setCursor(11, 0);
  lcd.print("");
  lcd.setCursor(12, 0);
  lcd.print("");
  lcd.setCursor(13, 0);
  lcd.print("");
  lcd.setCursor(14, 0);
  lcd.print("");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("L");
  lcd.setCursor(1, 1);
  lcd.print("E");
  lcd.setCursor(2, 1);
  lcd.print("D");
  lcd.setCursor(3, 1);
  lcd.print(" ");
  lcd.setCursor(4, 1);
  lcd.print("v");
  lcd.setCursor(5, 1);
  lcd.print("e");
  lcd.setCursor(6, 1);
  lcd.print("r");
  lcd.setCursor(7, 1);
  lcd.print("d");
  lcd.setCursor(8, 1);
  lcd.print("i");
  lcd.setCursor(9, 1);
  lcd.print(" ");
  lcd.setCursor(10, 1);
  lcd.print("O");
  lcd.setCursor(11, 1);
  lcd.print("N");
  lcd.setCursor(12, 1);
  lcd.print("");
  lcd.setCursor(13, 1);
  lcd.print("");
  lcd.setCursor(14, 1);
  lcd.print("");
  lcd.setCursor(15, 1);
  lcd.print("");
  delay(w);
}

//lcd "Corridoio LED blu OFF"
void lcd_led_blu_off_corridoio() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("C");
  lcd.setCursor(1, 0);
  lcd.print("o");
  lcd.setCursor(2, 0);
  lcd.print("r");
  lcd.setCursor(3, 0);
  lcd.print("r");
  lcd.setCursor(4, 0);
  lcd.print("i");
  lcd.setCursor(5, 0);
  lcd.print("d");
  lcd.setCursor(6, 0);
  lcd.print("o");
  lcd.setCursor(7, 0);
  lcd.print("i");
  lcd.setCursor(8, 0);
  lcd.print("o");
  lcd.setCursor(9, 0);
  lcd.print("");
  lcd.setCursor(10, 0);
  lcd.print("");
  lcd.setCursor(11, 0);
  lcd.print("");
  lcd.setCursor(12, 0);
  lcd.print("");
  lcd.setCursor(13, 0);
  lcd.print("");
  lcd.setCursor(14, 0);
  lcd.print("");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("L");
  lcd.setCursor(1, 1);
  lcd.print("E");
  lcd.setCursor(2, 1);
  lcd.print("D");
  lcd.setCursor(3, 1);
  lcd.print(" ");
  lcd.setCursor(4, 1);
  lcd.print("b");
  lcd.setCursor(5, 1);
  lcd.print("l");
  lcd.setCursor(6, 1);
  lcd.print("u");
  lcd.setCursor(7, 1);
  lcd.print("");
  lcd.setCursor(8, 1);
  lcd.print("O");
  lcd.setCursor(9, 1);
  lcd.print("F");
  lcd.setCursor(10, 1);
  lcd.print("F");
  lcd.setCursor(11, 1);
  lcd.print("");
  lcd.setCursor(12, 1);
  lcd.print("");
  lcd.setCursor(13, 1);
  lcd.print("");
  lcd.setCursor(14, 1);
  lcd.print("");
  lcd.setCursor(15, 1);
  lcd.print("");
  delay(w);
}

//lcd "Corridoio LED blu ON"
void lcd_led_blu_on_corridoio() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("C");
  lcd.setCursor(1, 0);
  lcd.print("o");
  lcd.setCursor(2, 0);
  lcd.print("r");
  lcd.setCursor(3, 0);
  lcd.print("r");
  lcd.setCursor(4, 0);
  lcd.print("i");
  lcd.setCursor(5, 0);
  lcd.print("d");
  lcd.setCursor(6, 0);
  lcd.print("o");
  lcd.setCursor(7, 0);
  lcd.print("i");
  lcd.setCursor(8, 0);
  lcd.print("o");
  lcd.setCursor(9, 0);
  lcd.print("");
  lcd.setCursor(10, 0);
  lcd.print("");
  lcd.setCursor(11, 0);
  lcd.print("");
  lcd.setCursor(12, 0);
  lcd.print("");
  lcd.setCursor(13, 0);
  lcd.print("");
  lcd.setCursor(14, 0);
  lcd.print("");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("L");
  lcd.setCursor(1, 1);
  lcd.print("E");
  lcd.setCursor(2, 1);
  lcd.print("D");
  lcd.setCursor(3, 1);
  lcd.print(" ");
  lcd.setCursor(4, 1);
  lcd.print("b");
  lcd.setCursor(5, 1);
  lcd.print("l");
  lcd.setCursor(6, 1);
  lcd.print("u");
  lcd.setCursor(7, 1);
  lcd.print("");
  lcd.setCursor(8, 1);
  lcd.print("O");
  lcd.setCursor(9, 1);
  lcd.print("N");
  lcd.setCursor(10, 1);
  lcd.print("");
  lcd.setCursor(11, 1);
  lcd.print("");
  lcd.setCursor(12, 1);
  lcd.print("");
  lcd.setCursor(13, 1);
  lcd.print("");
  lcd.setCursor(14, 1);
  lcd.print("");
  lcd.setCursor(15, 1);
  lcd.print("");
  delay(w);
}

//lcd "Corridoio LED bianchi OFF"
void lcd_led_bianchi_off_corridoio() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("C");
  lcd.setCursor(1, 0);
  lcd.print("o");
  lcd.setCursor(2, 0);
  lcd.print("r");
  lcd.setCursor(3, 0);
  lcd.print("r");
  lcd.setCursor(4, 0);
  lcd.print("i");
  lcd.setCursor(5, 0);
  lcd.print("d");
  lcd.setCursor(6, 0);
  lcd.print("o");
  lcd.setCursor(7, 0);
  lcd.print("i");
  lcd.setCursor(8, 0);
  lcd.print("o");
  lcd.setCursor(9, 0);
  lcd.print("");
  lcd.setCursor(10, 0);
  lcd.print("");
  lcd.setCursor(11, 0);
  lcd.print("");
  lcd.setCursor(12, 0);
  lcd.print("");
  lcd.setCursor(13, 0);
  lcd.print("");
  lcd.setCursor(14, 0);
  lcd.print("");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("L");
  lcd.setCursor(1, 1);
  lcd.print("E");
  lcd.setCursor(2, 1);
  lcd.print("D");
  lcd.setCursor(3, 1);
  lcd.print("");
  lcd.setCursor(4, 1);
  lcd.print("b");
  lcd.setCursor(5, 1);
  lcd.print("i");
  lcd.setCursor(6, 1);
  lcd.print("a");
  lcd.setCursor(7, 1);
  lcd.print("n");
  lcd.setCursor(8, 1);
  lcd.print("c");
  lcd.setCursor(9, 1);
  lcd.print("h");
  lcd.setCursor(10, 1);
  lcd.print("i");
  lcd.setCursor(11, 1);
  lcd.print("");
  lcd.setCursor(12, 1);
  lcd.print("O");
  lcd.setCursor(13, 1);
  lcd.print("F");
  lcd.setCursor(14, 1);
  lcd.print("F");
  lcd.setCursor(15, 1);
  lcd.print("");
  delay(w);
}

//lcd "Corridoio LED bianchi ON"
void lcd_led_bianchi_on_corridoio() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("C");
  lcd.setCursor(1, 0);
  lcd.print("o");
  lcd.setCursor(2, 0);
  lcd.print("r");
  lcd.setCursor(3, 0);
  lcd.print("r");
  lcd.setCursor(4, 0);
  lcd.print("i");
  lcd.setCursor(5, 0);
  lcd.print("d");
  lcd.setCursor(6, 0);
  lcd.print("o");
  lcd.setCursor(7, 0);
  lcd.print("i");
  lcd.setCursor(8, 0);
  lcd.print("o");
  lcd.setCursor(9, 0);
  lcd.print("");
  lcd.setCursor(10, 0);
  lcd.print("");
  lcd.setCursor(11, 0);
  lcd.print("");
  lcd.setCursor(12, 0);
  lcd.print("");
  lcd.setCursor(13, 0);
  lcd.print("");
  lcd.setCursor(14, 0);
  lcd.print("");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("L");
  lcd.setCursor(1, 1);
  lcd.print("E");
  lcd.setCursor(2, 1);
  lcd.print("D");
  lcd.setCursor(3, 1);
  lcd.print("");
  lcd.setCursor(4, 1);
  lcd.print("b");
  lcd.setCursor(5, 1);
  lcd.print("i");
  lcd.setCursor(6, 1);
  lcd.print("a");
  lcd.setCursor(7, 1);
  lcd.print("n");
  lcd.setCursor(8, 1);
  lcd.print("c");
  lcd.setCursor(9, 1);
  lcd.print("h");
  lcd.setCursor(10, 1);
  lcd.print("i");
  lcd.setCursor(11, 1);
  lcd.print("");
  lcd.setCursor(12, 1);
  lcd.print("O");
  lcd.setCursor(13, 1);
  lcd.print("N");
  lcd.setCursor(14, 1);
  lcd.print("");
  lcd.setCursor(15, 1);
  lcd.print("");
  delay(w);
}

//lcd "Corridoio LED gialli OFF"
void lcd_led_gialli_off_corridoio() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("C");
  lcd.setCursor(1, 0);
  lcd.print("o");
  lcd.setCursor(2, 0);
  lcd.print("r");
  lcd.setCursor(3, 0);
  lcd.print("r");
  lcd.setCursor(4, 0);
  lcd.print("i");
  lcd.setCursor(5, 0);
  lcd.print("d");
  lcd.setCursor(6, 0);
  lcd.print("o");
  lcd.setCursor(7, 0);
  lcd.print("i");
  lcd.setCursor(8, 0);
  lcd.print("o");
  lcd.setCursor(9, 0);
  lcd.print("");
  lcd.setCursor(10, 0);
  lcd.print("");
  lcd.setCursor(11, 0);
  lcd.print("");
  lcd.setCursor(12, 0);
  lcd.print("");
  lcd.setCursor(13, 0);
  lcd.print("");
  lcd.setCursor(14, 0);
  lcd.print("");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("L");
  lcd.setCursor(1, 1);
  lcd.print("E");
  lcd.setCursor(2, 1);
  lcd.print("D");
  lcd.setCursor(3, 1);
  lcd.print("");
  lcd.setCursor(4, 1);
  lcd.print("g");
  lcd.setCursor(5, 1);
  lcd.print("i");
  lcd.setCursor(6, 1);
  lcd.print("a");
  lcd.setCursor(7, 1);
  lcd.print("l");
  lcd.setCursor(8, 1);
  lcd.print("l");
  lcd.setCursor(9, 1);
  lcd.print("i");
  lcd.setCursor(10, 1);
  lcd.print("");
  lcd.setCursor(11, 1);
  lcd.print("O");
  lcd.setCursor(12, 1);
  lcd.print("F");
  lcd.setCursor(13, 1);
  lcd.print("F");
  lcd.setCursor(14, 1);
  lcd.print("");
  lcd.setCursor(15, 1);
  lcd.print("");
  delay(w);
}

//lcd "Corridoio LED gialli ON"
void lcd_led_gialli_on_corridoio() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("C");
  lcd.setCursor(1, 0);
  lcd.print("o");
  lcd.setCursor(2, 0);
  lcd.print("r");
  lcd.setCursor(3, 0);
  lcd.print("r");
  lcd.setCursor(4, 0);
  lcd.print("i");
  lcd.setCursor(5, 0);
  lcd.print("d");
  lcd.setCursor(6, 0);
  lcd.print("o");
  lcd.setCursor(7, 0);
  lcd.print("i");
  lcd.setCursor(8, 0);
  lcd.print("o");
  lcd.setCursor(9, 0);
  lcd.print("");
  lcd.setCursor(10, 0);
  lcd.print("");
  lcd.setCursor(11, 0);
  lcd.print("");
  lcd.setCursor(12, 0);
  lcd.print("");
  lcd.setCursor(13, 0);
  lcd.print("");
  lcd.setCursor(14, 0);
  lcd.print("");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("L");
  lcd.setCursor(1, 1);
  lcd.print("E");
  lcd.setCursor(2, 1);
  lcd.print("D");
  lcd.setCursor(3, 1);
  lcd.print("");
  lcd.setCursor(4, 1);
  lcd.print("g");
  lcd.setCursor(5, 1);
  lcd.print("i");
  lcd.setCursor(6, 1);
  lcd.print("a");
  lcd.setCursor(7, 1);
  lcd.print("l");
  lcd.setCursor(8, 1);
  lcd.print("l");
  lcd.setCursor(9, 1);
  lcd.print("i");
  lcd.setCursor(10, 1);
  lcd.print("");
  lcd.setCursor(11, 1);
  lcd.print("O");
  lcd.setCursor(12, 1);
  lcd.print("N");
  lcd.setCursor(13, 1);
  lcd.print("");
  lcd.setCursor(14, 1);
  lcd.print("");
  lcd.setCursor(15, 1);
  lcd.print("");
  delay(w);
}

//lcd "Corridoio effetto 1 ATTIVO"
void lcd_effetto_uno_corridoio() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("C");
  lcd.setCursor(1, 0);
  lcd.print("o");
  lcd.setCursor(2, 0);
  lcd.print("r");
  lcd.setCursor(3, 0);
  lcd.print("r");
  lcd.setCursor(4, 0);
  lcd.print("i");
  lcd.setCursor(5, 0);
  lcd.print("d");
  lcd.setCursor(6, 0);
  lcd.print("o");
  lcd.setCursor(7, 0);
  lcd.print("i");
  lcd.setCursor(8, 0);
  lcd.print("o");
  lcd.setCursor(9, 0);
  lcd.print("");
  lcd.setCursor(10, 0);
  lcd.print("");
  lcd.setCursor(11, 0);
  lcd.print("");
  lcd.setCursor(12, 0);
  lcd.print("");
  lcd.setCursor(13, 0);
  lcd.print("");
  lcd.setCursor(14, 0);
  lcd.print("");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("e");
  lcd.setCursor(1, 1);
  lcd.print("f");
  lcd.setCursor(2, 1);
  lcd.print("f");
  lcd.setCursor(3, 1);
  lcd.print("e");
  lcd.setCursor(4, 1);
  lcd.print("t");
  lcd.setCursor(5, 1);
  lcd.print("t");
  lcd.setCursor(6, 1);
  lcd.print("o");
  lcd.setCursor(7, 1);
  lcd.print("");
  lcd.setCursor(8, 1);
  lcd.print("1");
  lcd.setCursor(9, 1);
  lcd.print("");
  lcd.setCursor(10, 1);
  lcd.print("A");
  lcd.setCursor(11, 1);
  lcd.print("T");
  lcd.setCursor(12, 1);
  lcd.print("T");
  lcd.setCursor(13, 1);
  lcd.print("I");
  lcd.setCursor(14, 1);
  lcd.print("V");
  lcd.setCursor(15, 1);
  lcd.print("O");
}

//lcd "Corridoio effetto 2 ATTIVO"
void lcd_effetto_due_corridoio() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("C");
  lcd.setCursor(1, 0);
  lcd.print("o");
  lcd.setCursor(2, 0);
  lcd.print("r");
  lcd.setCursor(3, 0);
  lcd.print("r");
  lcd.setCursor(4, 0);
  lcd.print("i");
  lcd.setCursor(5, 0);
  lcd.print("d");
  lcd.setCursor(6, 0);
  lcd.print("o");
  lcd.setCursor(7, 0);
  lcd.print("i");
  lcd.setCursor(8, 0);
  lcd.print("o");
  lcd.setCursor(9, 0);
  lcd.print("");
  lcd.setCursor(10, 0);
  lcd.print("");
  lcd.setCursor(11, 0);
  lcd.print("");
  lcd.setCursor(12, 0);
  lcd.print("");
  lcd.setCursor(13, 0);
  lcd.print("");
  lcd.setCursor(14, 0);
  lcd.print("");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("e");
  lcd.setCursor(1, 1);
  lcd.print("f");
  lcd.setCursor(2, 1);
  lcd.print("f");
  lcd.setCursor(3, 1);
  lcd.print("e");
  lcd.setCursor(4, 1);
  lcd.print("t");
  lcd.setCursor(5, 1);
  lcd.print("t");
  lcd.setCursor(6, 1);
  lcd.print("o");
  lcd.setCursor(7, 1);
  lcd.print("");
  lcd.setCursor(8, 1);
  lcd.print("2");
  lcd.setCursor(9, 1);
  lcd.print("");
  lcd.setCursor(10, 1);
  lcd.print("A");
  lcd.setCursor(11, 1);
  lcd.print("T");
  lcd.setCursor(12, 1);
  lcd.print("T");
  lcd.setCursor(13, 1);
  lcd.print("I");
  lcd.setCursor(14, 1);
  lcd.print("V");
  lcd.setCursor(15, 1);
  lcd.print("O");
}

//lcd "Corridoio effetto 3 ATTIVO"
void lcd_effetto_tre_corridoio() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("C");
  lcd.setCursor(1, 0);
  lcd.print("o");
  lcd.setCursor(2, 0);
  lcd.print("r");
  lcd.setCursor(3, 0);
  lcd.print("r");
  lcd.setCursor(4, 0);
  lcd.print("i");
  lcd.setCursor(5, 0);
  lcd.print("d");
  lcd.setCursor(6, 0);
  lcd.print("o");
  lcd.setCursor(7, 0);
  lcd.print("i");
  lcd.setCursor(8, 0);
  lcd.print("o");
  lcd.setCursor(9, 0);
  lcd.print("");
  lcd.setCursor(10, 0);
  lcd.print("");
  lcd.setCursor(11, 0);
  lcd.print("");
  lcd.setCursor(12, 0);
  lcd.print("");
  lcd.setCursor(13, 0);
  lcd.print("");
  lcd.setCursor(14, 0);
  lcd.print("");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("e");
  lcd.setCursor(1, 1);
  lcd.print("f");
  lcd.setCursor(2, 1);
  lcd.print("f");
  lcd.setCursor(3, 1);
  lcd.print("e");
  lcd.setCursor(4, 1);
  lcd.print("t");
  lcd.setCursor(5, 1);
  lcd.print("t");
  lcd.setCursor(6, 1);
  lcd.print("o");
  lcd.setCursor(7, 1);
  lcd.print("");
  lcd.setCursor(8, 1);
  lcd.print("3");
  lcd.setCursor(9, 1);
  lcd.print("");
  lcd.setCursor(10, 1);
  lcd.print("A");
  lcd.setCursor(11, 1);
  lcd.print("T");
  lcd.setCursor(12, 1);
  lcd.print("T");
  lcd.setCursor(13, 1);
  lcd.print("I");
  lcd.setCursor(14, 1);
  lcd.print("V");
  lcd.setCursor(15, 1);
  lcd.print("O");
}

//lcd "Corridoio effetto 4 ATTIVO"
void lcd_effetto_quattro_corridoio() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("C");
  lcd.setCursor(1, 0);
  lcd.print("o");
  lcd.setCursor(2, 0);
  lcd.print("r");
  lcd.setCursor(3, 0);
  lcd.print("r");
  lcd.setCursor(4, 0);
  lcd.print("i");
  lcd.setCursor(5, 0);
  lcd.print("d");
  lcd.setCursor(6, 0);
  lcd.print("o");
  lcd.setCursor(7, 0);
  lcd.print("i");
  lcd.setCursor(8, 0);
  lcd.print("o");
  lcd.setCursor(9, 0);
  lcd.print("");
  lcd.setCursor(10, 0);
  lcd.print("");
  lcd.setCursor(11, 0);
  lcd.print("");
  lcd.setCursor(12, 0);
  lcd.print("");
  lcd.setCursor(13, 0);
  lcd.print("");
  lcd.setCursor(14, 0);
  lcd.print("");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("e");
  lcd.setCursor(1, 1);
  lcd.print("f");
  lcd.setCursor(2, 1);
  lcd.print("f");
  lcd.setCursor(3, 1);
  lcd.print("e");
  lcd.setCursor(4, 1);
  lcd.print("t");
  lcd.setCursor(5, 1);
  lcd.print("t");
  lcd.setCursor(6, 1);
  lcd.print("o");
  lcd.setCursor(7, 1);
  lcd.print("");
  lcd.setCursor(8, 1);
  lcd.print("4");
  lcd.setCursor(9, 1);
  lcd.print("");
  lcd.setCursor(10, 1);
  lcd.print("A");
  lcd.setCursor(11, 1);
  lcd.print("T");
  lcd.setCursor(12, 1);
  lcd.print("T");
  lcd.setCursor(13, 1);
  lcd.print("I");
  lcd.setCursor(14, 1);
  lcd.print("V");
  lcd.setCursor(15, 1);
  lcd.print("O");
}

//lcd "Corridoio effetto 5 ATTIVO"
void lcd_effetto_cinque_corridoio() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("C");
  lcd.setCursor(1, 0);
  lcd.print("o");
  lcd.setCursor(2, 0);
  lcd.print("r");
  lcd.setCursor(3, 0);
  lcd.print("r");
  lcd.setCursor(4, 0);
  lcd.print("i");
  lcd.setCursor(5, 0);
  lcd.print("d");
  lcd.setCursor(6, 0);
  lcd.print("o");
  lcd.setCursor(7, 0);
  lcd.print("i");
  lcd.setCursor(8, 0);
  lcd.print("o");
  lcd.setCursor(9, 0);
  lcd.print("");
  lcd.setCursor(10, 0);
  lcd.print("");
  lcd.setCursor(11, 0);
  lcd.print("");
  lcd.setCursor(12, 0);
  lcd.print("");
  lcd.setCursor(13, 0);
  lcd.print("");
  lcd.setCursor(14, 0);
  lcd.print("");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("e");
  lcd.setCursor(1, 1);
  lcd.print("f");
  lcd.setCursor(2, 1);
  lcd.print("f");
  lcd.setCursor(3, 1);
  lcd.print("e");
  lcd.setCursor(4, 1);
  lcd.print("t");
  lcd.setCursor(5, 1);
  lcd.print("t");
  lcd.setCursor(6, 1);
  lcd.print("o");
  lcd.setCursor(7, 1);
  lcd.print("");
  lcd.setCursor(8, 1);
  lcd.print("5");
  lcd.setCursor(9, 1);
  lcd.print("");
  lcd.setCursor(10, 1);
  lcd.print("A");
  lcd.setCursor(11, 1);
  lcd.print("T");
  lcd.setCursor(12, 1);
  lcd.print("T");
  lcd.setCursor(13, 1);
  lcd.print("I");
  lcd.setCursor(14, 1);
  lcd.print("V");
  lcd.setCursor(15, 1);
  lcd.print("O");
}

//lcd "Corridoio effetto 6 ATTIVO"
void lcd_effetto_sei_corridoio() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("C");
  lcd.setCursor(1, 0);
  lcd.print("o");
  lcd.setCursor(2, 0);
  lcd.print("r");
  lcd.setCursor(3, 0);
  lcd.print("r");
  lcd.setCursor(4, 0);
  lcd.print("i");
  lcd.setCursor(5, 0);
  lcd.print("d");
  lcd.setCursor(6, 0);
  lcd.print("o");
  lcd.setCursor(7, 0);
  lcd.print("i");
  lcd.setCursor(8, 0);
  lcd.print("o");
  lcd.setCursor(9, 0);
  lcd.print("");
  lcd.setCursor(10, 0);
  lcd.print("");
  lcd.setCursor(11, 0);
  lcd.print("");
  lcd.setCursor(12, 0);
  lcd.print("");
  lcd.setCursor(13, 0);
  lcd.print("");
  lcd.setCursor(14, 0);
  lcd.print("");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("e");
  lcd.setCursor(1, 1);
  lcd.print("f");
  lcd.setCursor(2, 1);
  lcd.print("f");
  lcd.setCursor(3, 1);
  lcd.print("e");
  lcd.setCursor(4, 1);
  lcd.print("t");
  lcd.setCursor(5, 1);
  lcd.print("t");
  lcd.setCursor(6, 1);
  lcd.print("o");
  lcd.setCursor(7, 1);
  lcd.print("");
  lcd.setCursor(8, 1);
  lcd.print("6");
  lcd.setCursor(9, 1);
  lcd.print("");
  lcd.setCursor(10, 1);
  lcd.print("A");
  lcd.setCursor(11, 1);
  lcd.print("T");
  lcd.setCursor(12, 1);
  lcd.print("T");
  lcd.setCursor(13, 1);
  lcd.print("I");
  lcd.setCursor(14, 1);
  lcd.print("V");
  lcd.setCursor(15, 1);
  lcd.print("O");
}

//lcd "CAMERA 1 tutti i led ON"
void lcd_tutti_led_on_camera1() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("C");
  lcd.setCursor(1, 0);
  lcd.print("a");
  lcd.setCursor(2, 0);
  lcd.print("m");
  lcd.setCursor(3, 0);
  lcd.print("e");
  lcd.setCursor(4, 0);
  lcd.print("r");
  lcd.setCursor(5, 0);
  lcd.print("a");
  lcd.setCursor(6, 0);
  lcd.print("");
  lcd.setCursor(7, 0);
  lcd.print("1");
  lcd.setCursor(8, 0);
  lcd.print("");
  lcd.setCursor(9, 0);
  lcd.print("");
  lcd.setCursor(10, 0);
  lcd.print("");
  lcd.setCursor(11, 0);
  lcd.print("");
  lcd.setCursor(12, 0);
  lcd.print("");
  lcd.setCursor(13, 0);
  lcd.print("");
  lcd.setCursor(14, 0);
  lcd.print("");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("t");
  lcd.setCursor(1, 1);
  lcd.print("u");
  lcd.setCursor(2, 1);
  lcd.print("t");
  lcd.setCursor(3, 1);
  lcd.print("t");
  lcd.setCursor(4, 1);
  lcd.print("i");
  lcd.setCursor(5, 1);
  lcd.print("");
  lcd.setCursor(6, 1);
  lcd.print("i");
  lcd.setCursor(7, 1);
  lcd.print("");
  lcd.setCursor(8, 1);
  lcd.print("L");
  lcd.setCursor(9, 1);
  lcd.print("E");
  lcd.setCursor(10, 1);
  lcd.print("D");
  lcd.setCursor(11, 1);
  lcd.print("");
  lcd.setCursor(12, 1);
  lcd.print("O");
  lcd.setCursor(13, 1);
  lcd.print("N");
  lcd.setCursor(14, 1);
  lcd.print("");
  lcd.setCursor(15, 1);
  lcd.print("");
  delay(w);
}

//lcd "CAMERA 1 tutti i led OFF"
void lcd_tutti_led_off_camera1() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("C");
  lcd.setCursor(1, 0);
  lcd.print("a");
  lcd.setCursor(2, 0);
  lcd.print("m");
  lcd.setCursor(3, 0);
  lcd.print("e");
  lcd.setCursor(4, 0);
  lcd.print("r");
  lcd.setCursor(5, 0);
  lcd.print("a");
  lcd.setCursor(6, 0);
  lcd.print("");
  lcd.setCursor(7, 0);
  lcd.print("1");
  lcd.setCursor(8, 0);
  lcd.print("");
  lcd.setCursor(9, 0);
  lcd.print("");
  lcd.setCursor(10, 0);
  lcd.print("");
  lcd.setCursor(11, 0);
  lcd.print("");
  lcd.setCursor(12, 0);
  lcd.print("");
  lcd.setCursor(13, 0);
  lcd.print("");
  lcd.setCursor(14, 0);
  lcd.print("");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("t");
  lcd.setCursor(1, 1);
  lcd.print("u");
  lcd.setCursor(2, 1);
  lcd.print("t");
  lcd.setCursor(3, 1);
  lcd.print("t");
  lcd.setCursor(4, 1);
  lcd.print("i");
  lcd.setCursor(5, 1);
  lcd.print("");
  lcd.setCursor(6, 1);
  lcd.print("i");
  lcd.setCursor(7, 1);
  lcd.print("");
  lcd.setCursor(8, 1);
  lcd.print("L");
  lcd.setCursor(9, 1);
  lcd.print("E");
  lcd.setCursor(10, 1);
  lcd.print("D");
  lcd.setCursor(11, 1);
  lcd.print("");
  lcd.setCursor(12, 1);
  lcd.print("O");
  lcd.setCursor(13, 1);
  lcd.print("F");
  lcd.setCursor(14, 1);
  lcd.print("F");
  lcd.setCursor(15, 1);
  lcd.print("");
  delay(w);
}

//lcd "CAMERA 1 LED grandi ON"
void lcd_led_grandi_on_camera1() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("C");
  lcd.setCursor(1, 0);
  lcd.print("a");
  lcd.setCursor(2, 0);
  lcd.print("m");
  lcd.setCursor(3, 0);
  lcd.print("e");
  lcd.setCursor(4, 0);
  lcd.print("r");
  lcd.setCursor(5, 0);
  lcd.print("a");
  lcd.setCursor(6, 0);
  lcd.print("");
  lcd.setCursor(7, 0);
  lcd.print("1");
  lcd.setCursor(8, 0);
  lcd.print("");
  lcd.setCursor(9, 0);
  lcd.print("");
  lcd.setCursor(10, 0);
  lcd.print("");
  lcd.setCursor(11, 0);
  lcd.print("");
  lcd.setCursor(12, 0);
  lcd.print("");
  lcd.setCursor(13, 0);
  lcd.print("");
  lcd.setCursor(14, 0);
  lcd.print("");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("L");
  lcd.setCursor(1, 1);
  lcd.print("E");
  lcd.setCursor(2, 1);
  lcd.print("D");
  lcd.setCursor(3, 1);
  lcd.print("");
  lcd.setCursor(4, 1);
  lcd.print("g");
  lcd.setCursor(5, 1);
  lcd.print("r");
  lcd.setCursor(6, 1);
  lcd.print("a");
  lcd.setCursor(7, 1);
  lcd.print("n");
  lcd.setCursor(8, 1);
  lcd.print("d");
  lcd.setCursor(9, 1);
  lcd.print("i");
  lcd.setCursor(10, 1);
  lcd.print("");
  lcd.setCursor(11, 1);
  lcd.print("O");
  lcd.setCursor(12, 1);
  lcd.print("N");
  lcd.setCursor(13, 1);
  lcd.print("");
  lcd.setCursor(14, 1);
  lcd.print("");
  lcd.setCursor(15, 1);
  lcd.print("");
  delay(w);
}

//lcd "CAMERA 1 LED piccoli ON"
void lcd_led_piccoli_on_camera1() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("C");
  lcd.setCursor(1, 0);
  lcd.print("a");
  lcd.setCursor(2, 0);
  lcd.print("m");
  lcd.setCursor(3, 0);
  lcd.print("e");
  lcd.setCursor(4, 0);
  lcd.print("r");
  lcd.setCursor(5, 0);
  lcd.print("a");
  lcd.setCursor(6, 0);
  lcd.print("");
  lcd.setCursor(7, 0);
  lcd.print("1");
  lcd.setCursor(8, 0);
  lcd.print("");
  lcd.setCursor(9, 0);
  lcd.print("");
  lcd.setCursor(10, 0);
  lcd.print("");
  lcd.setCursor(11, 0);
  lcd.print("");
  lcd.setCursor(12, 0);
  lcd.print("");
  lcd.setCursor(13, 0);
  lcd.print("");
  lcd.setCursor(14, 0);
  lcd.print("");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("L");
  lcd.setCursor(1, 1);
  lcd.print("E");
  lcd.setCursor(2, 1);
  lcd.print("D");
  lcd.setCursor(3, 1);
  lcd.print("");
  lcd.setCursor(4, 1);
  lcd.print("p");
  lcd.setCursor(5, 1);
  lcd.print("i");
  lcd.setCursor(6, 1);
  lcd.print("c");
  lcd.setCursor(7, 1);
  lcd.print("c");
  lcd.setCursor(8, 1);
  lcd.print("o");
  lcd.setCursor(9, 1);
  lcd.print("l");
  lcd.setCursor(10, 1);
  lcd.print("i");
  lcd.setCursor(11, 1);
  lcd.print("");
  lcd.setCursor(12, 1);
  lcd.print("O");
  lcd.setCursor(13, 1);
  lcd.print("N");
  lcd.setCursor(14, 1);
  lcd.print("");
  lcd.setCursor(15, 1);
  lcd.print("");
  delay(w);
}

//lcd "Camera 1 effetto 1 ATTIVO"
void lcd_effetto_uno_camera1() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("C");
  lcd.setCursor(1, 0);
  lcd.print("a");
  lcd.setCursor(2, 0);
  lcd.print("m");
  lcd.setCursor(3, 0);
  lcd.print("e");
  lcd.setCursor(4, 0);
  lcd.print("r");
  lcd.setCursor(5, 0);
  lcd.print("a");
  lcd.setCursor(6, 0);
  lcd.print("");
  lcd.setCursor(7, 0);
  lcd.print("1");
  lcd.setCursor(8, 0);
  lcd.print("");
  lcd.setCursor(9, 0);
  lcd.print("");
  lcd.setCursor(10, 0);
  lcd.print("");
  lcd.setCursor(11, 0);
  lcd.print("");
  lcd.setCursor(12, 0);
  lcd.print("");
  lcd.setCursor(13, 0);
  lcd.print("");
  lcd.setCursor(14, 0);
  lcd.print("");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("e");
  lcd.setCursor(1, 1);
  lcd.print("f");
  lcd.setCursor(2, 1);
  lcd.print("f");
  lcd.setCursor(3, 1);
  lcd.print("e");
  lcd.setCursor(4, 1);
  lcd.print("t");
  lcd.setCursor(5, 1);
  lcd.print("t");
  lcd.setCursor(6, 1);
  lcd.print("o");
  lcd.setCursor(7, 1);
  lcd.print("");
  lcd.setCursor(8, 1);
  lcd.print("1");
  lcd.setCursor(9, 1);
  lcd.print("");
  lcd.setCursor(10, 1);
  lcd.print("A");
  lcd.setCursor(11, 1);
  lcd.print("T");
  lcd.setCursor(12, 1);
  lcd.print("T");
  lcd.setCursor(13, 1);
  lcd.print("I");
  lcd.setCursor(14, 1);
  lcd.print("V");
  lcd.setCursor(15, 1);
  lcd.print("O");
}

//lcd "Camera 1 effetto 2 ATTIVO"
void lcd_effetto_due_camera1() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("C");
  lcd.setCursor(1, 0);
  lcd.print("a");
  lcd.setCursor(2, 0);
  lcd.print("m");
  lcd.setCursor(3, 0);
  lcd.print("e");
  lcd.setCursor(4, 0);
  lcd.print("r");
  lcd.setCursor(5, 0);
  lcd.print("a");
  lcd.setCursor(6, 0);
  lcd.print("");
  lcd.setCursor(7, 0);
  lcd.print("1");
  lcd.setCursor(8, 0);
  lcd.print("");
  lcd.setCursor(9, 0);
  lcd.print("");
  lcd.setCursor(10, 0);
  lcd.print("");
  lcd.setCursor(11, 0);
  lcd.print("");
  lcd.setCursor(12, 0);
  lcd.print("");
  lcd.setCursor(13, 0);
  lcd.print("");
  lcd.setCursor(14, 0);
  lcd.print("");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("e");
  lcd.setCursor(1, 1);
  lcd.print("f");
  lcd.setCursor(2, 1);
  lcd.print("f");
  lcd.setCursor(3, 1);
  lcd.print("e");
  lcd.setCursor(4, 1);
  lcd.print("t");
  lcd.setCursor(5, 1);
  lcd.print("t");
  lcd.setCursor(6, 1);
  lcd.print("o");
  lcd.setCursor(7, 1);
  lcd.print("");
  lcd.setCursor(8, 1);
  lcd.print("2");
  lcd.setCursor(9, 1);
  lcd.print("");
  lcd.setCursor(10, 1);
  lcd.print("A");
  lcd.setCursor(11, 1);
  lcd.print("T");
  lcd.setCursor(12, 1);
  lcd.print("T");
  lcd.setCursor(13, 1);
  lcd.print("I");
  lcd.setCursor(14, 1);
  lcd.print("V");
  lcd.setCursor(15, 1);
  lcd.print("O");
}

//lcd "Camera 1 effetto 3 ATTIVO"
void lcd_effetto_tre_camera1() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("C");
  lcd.setCursor(1, 0);
  lcd.print("a");
  lcd.setCursor(2, 0);
  lcd.print("m");
  lcd.setCursor(3, 0);
  lcd.print("e");
  lcd.setCursor(4, 0);
  lcd.print("r");
  lcd.setCursor(5, 0);
  lcd.print("a");
  lcd.setCursor(6, 0);
  lcd.print("");
  lcd.setCursor(7, 0);
  lcd.print("1");
  lcd.setCursor(8, 0);
  lcd.print("");
  lcd.setCursor(9, 0);
  lcd.print("");
  lcd.setCursor(10, 0);
  lcd.print("");
  lcd.setCursor(11, 0);
  lcd.print("");
  lcd.setCursor(12, 0);
  lcd.print("");
  lcd.setCursor(13, 0);
  lcd.print("");
  lcd.setCursor(14, 0);
  lcd.print("");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("e");
  lcd.setCursor(1, 1);
  lcd.print("f");
  lcd.setCursor(2, 1);
  lcd.print("f");
  lcd.setCursor(3, 1);
  lcd.print("e");
  lcd.setCursor(4, 1);
  lcd.print("t");
  lcd.setCursor(5, 1);
  lcd.print("t");
  lcd.setCursor(6, 1);
  lcd.print("o");
  lcd.setCursor(7, 1);
  lcd.print("");
  lcd.setCursor(8, 1);
  lcd.print("3");
  lcd.setCursor(9, 1);
  lcd.print("");
  lcd.setCursor(10, 1);
  lcd.print("A");
  lcd.setCursor(11, 1);
  lcd.print("T");
  lcd.setCursor(12, 1);
  lcd.print("T");
  lcd.setCursor(13, 1);
  lcd.print("I");
  lcd.setCursor(14, 1);
  lcd.print("V");
  lcd.setCursor(15, 1);
  lcd.print("O");
}

//lcd "Camera 1 effetto 4 ATTIVO"
void lcd_effetto_quattro_camera1() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("C");
  lcd.setCursor(1, 0);
  lcd.print("a");
  lcd.setCursor(2, 0);
  lcd.print("m");
  lcd.setCursor(3, 0);
  lcd.print("e");
  lcd.setCursor(4, 0);
  lcd.print("r");
  lcd.setCursor(5, 0);
  lcd.print("a");
  lcd.setCursor(6, 0);
  lcd.print("");
  lcd.setCursor(7, 0);
  lcd.print("1");
  lcd.setCursor(8, 0);
  lcd.print("");
  lcd.setCursor(9, 0);
  lcd.print("");
  lcd.setCursor(10, 0);
  lcd.print("");
  lcd.setCursor(11, 0);
  lcd.print("");
  lcd.setCursor(12, 0);
  lcd.print("");
  lcd.setCursor(13, 0);
  lcd.print("");
  lcd.setCursor(14, 0);
  lcd.print("");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("e");
  lcd.setCursor(1, 1);
  lcd.print("f");
  lcd.setCursor(2, 1);
  lcd.print("f");
  lcd.setCursor(3, 1);
  lcd.print("e");
  lcd.setCursor(4, 1);
  lcd.print("t");
  lcd.setCursor(5, 1);
  lcd.print("t");
  lcd.setCursor(6, 1);
  lcd.print("o");
  lcd.setCursor(7, 1);
  lcd.print("");
  lcd.setCursor(8, 1);
  lcd.print("4");
  lcd.setCursor(9, 1);
  lcd.print("");
  lcd.setCursor(10, 1);
  lcd.print("A");
  lcd.setCursor(11, 1);
  lcd.print("T");
  lcd.setCursor(12, 1);
  lcd.print("T");
  lcd.setCursor(13, 1);
  lcd.print("I");
  lcd.setCursor(14, 1);
  lcd.print("V");
  lcd.setCursor(15, 1);
  lcd.print("O");
}


//lcd "CAMERA 2 tutti i led ON"
void lcd_tutti_led_on_camera2() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("C");
  lcd.setCursor(1, 0);
  lcd.print("a");
  lcd.setCursor(2, 0);
  lcd.print("m");
  lcd.setCursor(3, 0);
  lcd.print("e");
  lcd.setCursor(4, 0);
  lcd.print("r");
  lcd.setCursor(5, 0);
  lcd.print("a");
  lcd.setCursor(6, 0);
  lcd.print("");
  lcd.setCursor(7, 0);
  lcd.print("2");
  lcd.setCursor(8, 0);
  lcd.print("");
  lcd.setCursor(9, 0);
  lcd.print("");
  lcd.setCursor(10, 0);
  lcd.print("");
  lcd.setCursor(11, 0);
  lcd.print("");
  lcd.setCursor(12, 0);
  lcd.print("");
  lcd.setCursor(13, 0);
  lcd.print("");
  lcd.setCursor(14, 0);
  lcd.print("");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("t");
  lcd.setCursor(1, 1);
  lcd.print("u");
  lcd.setCursor(2, 1);
  lcd.print("t");
  lcd.setCursor(3, 1);
  lcd.print("t");
  lcd.setCursor(4, 1);
  lcd.print("i");
  lcd.setCursor(5, 1);
  lcd.print("");
  lcd.setCursor(6, 1);
  lcd.print("i");
  lcd.setCursor(7, 1);
  lcd.print("");
  lcd.setCursor(8, 1);
  lcd.print("L");
  lcd.setCursor(9, 1);
  lcd.print("E");
  lcd.setCursor(10, 1);
  lcd.print("D");
  lcd.setCursor(11, 1);
  lcd.print("");
  lcd.setCursor(12, 1);
  lcd.print("O");
  lcd.setCursor(13, 1);
  lcd.print("N");
  lcd.setCursor(14, 1);
  lcd.print("");
  lcd.setCursor(15, 1);
  lcd.print("");
  delay(w);
}

//lcd "CAMERA 2 tutti i led OFF"
void lcd_tutti_led_off_camera2() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("C");
  lcd.setCursor(1, 0);
  lcd.print("a");
  lcd.setCursor(2, 0);
  lcd.print("m");
  lcd.setCursor(3, 0);
  lcd.print("e");
  lcd.setCursor(4, 0);
  lcd.print("r");
  lcd.setCursor(5, 0);
  lcd.print("a");
  lcd.setCursor(6, 0);
  lcd.print("");
  lcd.setCursor(7, 0);
  lcd.print("2");
  lcd.setCursor(8, 0);
  lcd.print("");
  lcd.setCursor(9, 0);
  lcd.print("");
  lcd.setCursor(10, 0);
  lcd.print("");
  lcd.setCursor(11, 0);
  lcd.print("");
  lcd.setCursor(12, 0);
  lcd.print("");
  lcd.setCursor(13, 0);
  lcd.print("");
  lcd.setCursor(14, 0);
  lcd.print("");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("t");
  lcd.setCursor(1, 1);
  lcd.print("u");
  lcd.setCursor(2, 1);
  lcd.print("t");
  lcd.setCursor(3, 1);
  lcd.print("t");
  lcd.setCursor(4, 1);
  lcd.print("i");
  lcd.setCursor(5, 1);
  lcd.print("");
  lcd.setCursor(6, 1);
  lcd.print("i");
  lcd.setCursor(7, 1);
  lcd.print("");
  lcd.setCursor(8, 1);
  lcd.print("L");
  lcd.setCursor(9, 1);
  lcd.print("E");
  lcd.setCursor(10, 1);
  lcd.print("D");
  lcd.setCursor(11, 1);
  lcd.print("");
  lcd.setCursor(12, 1);
  lcd.print("O");
  lcd.setCursor(13, 1);
  lcd.print("F");
  lcd.setCursor(14, 1);
  lcd.print("F");
  lcd.setCursor(15, 1);
  lcd.print("");
  delay(w);
}

//lcd "CAMERA 2 LED grandi ON"
void lcd_led_grandi_on_camera2() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("C");
  lcd.setCursor(1, 0);
  lcd.print("a");
  lcd.setCursor(2, 0);
  lcd.print("m");
  lcd.setCursor(3, 0);
  lcd.print("e");
  lcd.setCursor(4, 0);
  lcd.print("r");
  lcd.setCursor(5, 0);
  lcd.print("a");
  lcd.setCursor(6, 0);
  lcd.print("");
  lcd.setCursor(7, 0);
  lcd.print("2");
  lcd.setCursor(8, 0);
  lcd.print("");
  lcd.setCursor(9, 0);
  lcd.print("");
  lcd.setCursor(10, 0);
  lcd.print("");
  lcd.setCursor(11, 0);
  lcd.print("");
  lcd.setCursor(12, 0);
  lcd.print("");
  lcd.setCursor(13, 0);
  lcd.print("");
  lcd.setCursor(14, 0);
  lcd.print("");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("L");
  lcd.setCursor(1, 1);
  lcd.print("E");
  lcd.setCursor(2, 1);
  lcd.print("D");
  lcd.setCursor(3, 1);
  lcd.print("");
  lcd.setCursor(4, 1);
  lcd.print("g");
  lcd.setCursor(5, 1);
  lcd.print("r");
  lcd.setCursor(6, 1);
  lcd.print("a");
  lcd.setCursor(7, 1);
  lcd.print("n");
  lcd.setCursor(8, 1);
  lcd.print("d");
  lcd.setCursor(9, 1);
  lcd.print("i");
  lcd.setCursor(10, 1);
  lcd.print("");
  lcd.setCursor(11, 1);
  lcd.print("O");
  lcd.setCursor(12, 1);
  lcd.print("N");
  lcd.setCursor(13, 1);
  lcd.print("");
  lcd.setCursor(14, 1);
  lcd.print("");
  lcd.setCursor(15, 1);
  lcd.print("");
  delay(w);
}

//lcd "CAMERA 2 LED piccoli ON"
void lcd_led_piccoli_on_camera2() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("C");
  lcd.setCursor(1, 0);
  lcd.print("a");
  lcd.setCursor(2, 0);
  lcd.print("m");
  lcd.setCursor(3, 0);
  lcd.print("e");
  lcd.setCursor(4, 0);
  lcd.print("r");
  lcd.setCursor(5, 0);
  lcd.print("a");
  lcd.setCursor(6, 0);
  lcd.print("");
  lcd.setCursor(7, 0);
  lcd.print("2");
  lcd.setCursor(8, 0);
  lcd.print("");
  lcd.setCursor(9, 0);
  lcd.print("");
  lcd.setCursor(10, 0);
  lcd.print("");
  lcd.setCursor(11, 0);
  lcd.print("");
  lcd.setCursor(12, 0);
  lcd.print("");
  lcd.setCursor(13, 0);
  lcd.print("");
  lcd.setCursor(14, 0);
  lcd.print("");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("L");
  lcd.setCursor(1, 1);
  lcd.print("E");
  lcd.setCursor(2, 1);
  lcd.print("D");
  lcd.setCursor(3, 1);
  lcd.print("");
  lcd.setCursor(4, 1);
  lcd.print("p");
  lcd.setCursor(5, 1);
  lcd.print("i");
  lcd.setCursor(6, 1);
  lcd.print("c");
  lcd.setCursor(7, 1);
  lcd.print("c");
  lcd.setCursor(8, 1);
  lcd.print("o");
  lcd.setCursor(9, 1);
  lcd.print("l");
  lcd.setCursor(10, 1);
  lcd.print("i");
  lcd.setCursor(11, 1);
  lcd.print("");
  lcd.setCursor(12, 1);
  lcd.print("O");
  lcd.setCursor(13, 1);
  lcd.print("N");
  lcd.setCursor(14, 1);
  lcd.print("");
  lcd.setCursor(15, 1);
  lcd.print("");
  delay(w);
}

//lcd "Camera 2 effetto 1 ATTIVO"
void lcd_effetto_uno_camera2() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("C");
  lcd.setCursor(1, 0);
  lcd.print("a");
  lcd.setCursor(2, 0);
  lcd.print("m");
  lcd.setCursor(3, 0);
  lcd.print("e");
  lcd.setCursor(4, 0);
  lcd.print("r");
  lcd.setCursor(5, 0);
  lcd.print("a");
  lcd.setCursor(6, 0);
  lcd.print("");
  lcd.setCursor(7, 0);
  lcd.print("2");
  lcd.setCursor(8, 0);
  lcd.print("");
  lcd.setCursor(9, 0);
  lcd.print("");
  lcd.setCursor(10, 0);
  lcd.print("");
  lcd.setCursor(11, 0);
  lcd.print("");
  lcd.setCursor(12, 0);
  lcd.print("");
  lcd.setCursor(13, 0);
  lcd.print("");
  lcd.setCursor(14, 0);
  lcd.print("");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("e");
  lcd.setCursor(1, 1);
  lcd.print("f");
  lcd.setCursor(2, 1);
  lcd.print("f");
  lcd.setCursor(3, 1);
  lcd.print("e");
  lcd.setCursor(4, 1);
  lcd.print("t");
  lcd.setCursor(5, 1);
  lcd.print("t");
  lcd.setCursor(6, 1);
  lcd.print("o");
  lcd.setCursor(7, 1);
  lcd.print("");
  lcd.setCursor(8, 1);
  lcd.print("1");
  lcd.setCursor(9, 1);
  lcd.print("");
  lcd.setCursor(10, 1);
  lcd.print("A");
  lcd.setCursor(11, 1);
  lcd.print("T");
  lcd.setCursor(12, 1);
  lcd.print("T");
  lcd.setCursor(13, 1);
  lcd.print("I");
  lcd.setCursor(14, 1);
  lcd.print("V");
  lcd.setCursor(15, 1);
  lcd.print("O");
}

//lcd "Camera 2 effetto 2 ATTIVO"
void lcd_effetto_due_camera2() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("C");
  lcd.setCursor(1, 0);
  lcd.print("a");
  lcd.setCursor(2, 0);
  lcd.print("m");
  lcd.setCursor(3, 0);
  lcd.print("e");
  lcd.setCursor(4, 0);
  lcd.print("r");
  lcd.setCursor(5, 0);
  lcd.print("a");
  lcd.setCursor(6, 0);
  lcd.print("");
  lcd.setCursor(7, 0);
  lcd.print("2");
  lcd.setCursor(8, 0);
  lcd.print("");
  lcd.setCursor(9, 0);
  lcd.print("");
  lcd.setCursor(10, 0);
  lcd.print("");
  lcd.setCursor(11, 0);
  lcd.print("");
  lcd.setCursor(12, 0);
  lcd.print("");
  lcd.setCursor(13, 0);
  lcd.print("");
  lcd.setCursor(14, 0);
  lcd.print("");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("e");
  lcd.setCursor(1, 1);
  lcd.print("f");
  lcd.setCursor(2, 1);
  lcd.print("f");
  lcd.setCursor(3, 1);
  lcd.print("e");
  lcd.setCursor(4, 1);
  lcd.print("t");
  lcd.setCursor(5, 1);
  lcd.print("t");
  lcd.setCursor(6, 1);
  lcd.print("o");
  lcd.setCursor(7, 1);
  lcd.print("");
  lcd.setCursor(8, 1);
  lcd.print("2");
  lcd.setCursor(9, 1);
  lcd.print("");
  lcd.setCursor(10, 1);
  lcd.print("A");
  lcd.setCursor(11, 1);
  lcd.print("T");
  lcd.setCursor(12, 1);
  lcd.print("T");
  lcd.setCursor(13, 1);
  lcd.print("I");
  lcd.setCursor(14, 1);
  lcd.print("V");
  lcd.setCursor(15, 1);
  lcd.print("O");
}

//lcd "Camera 2 effetto 3 ATTIVO"
void lcd_effetto_tre_camera2() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("C");
  lcd.setCursor(1, 0);
  lcd.print("a");
  lcd.setCursor(2, 0);
  lcd.print("m");
  lcd.setCursor(3, 0);
  lcd.print("e");
  lcd.setCursor(4, 0);
  lcd.print("r");
  lcd.setCursor(5, 0);
  lcd.print("a");
  lcd.setCursor(6, 0);
  lcd.print("");
  lcd.setCursor(7, 0);
  lcd.print("2");
  lcd.setCursor(8, 0);
  lcd.print("");
  lcd.setCursor(9, 0);
  lcd.print("");
  lcd.setCursor(10, 0);
  lcd.print("");
  lcd.setCursor(11, 0);
  lcd.print("");
  lcd.setCursor(12, 0);
  lcd.print("");
  lcd.setCursor(13, 0);
  lcd.print("");
  lcd.setCursor(14, 0);
  lcd.print("");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("e");
  lcd.setCursor(1, 1);
  lcd.print("f");
  lcd.setCursor(2, 1);
  lcd.print("f");
  lcd.setCursor(3, 1);
  lcd.print("e");
  lcd.setCursor(4, 1);
  lcd.print("t");
  lcd.setCursor(5, 1);
  lcd.print("t");
  lcd.setCursor(6, 1);
  lcd.print("o");
  lcd.setCursor(7, 1);
  lcd.print("");
  lcd.setCursor(8, 1);
  lcd.print("3");
  lcd.setCursor(9, 1);
  lcd.print("");
  lcd.setCursor(10, 1);
  lcd.print("A");
  lcd.setCursor(11, 1);
  lcd.print("T");
  lcd.setCursor(12, 1);
  lcd.print("T");
  lcd.setCursor(13, 1);
  lcd.print("I");
  lcd.setCursor(14, 1);
  lcd.print("V");
  lcd.setCursor(15, 1);
  lcd.print("O");
}

//lcd "Camera 2 effetto 4 ATTIVO"
void lcd_effetto_quattro_camera2() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("C");
  lcd.setCursor(1, 0);
  lcd.print("a");
  lcd.setCursor(2, 0);
  lcd.print("m");
  lcd.setCursor(3, 0);
  lcd.print("e");
  lcd.setCursor(4, 0);
  lcd.print("r");
  lcd.setCursor(5, 0);
  lcd.print("a");
  lcd.setCursor(6, 0);
  lcd.print("");
  lcd.setCursor(7, 0);
  lcd.print("2");
  lcd.setCursor(8, 0);
  lcd.print("");
  lcd.setCursor(9, 0);
  lcd.print("");
  lcd.setCursor(10, 0);
  lcd.print("");
  lcd.setCursor(11, 0);
  lcd.print("");
  lcd.setCursor(12, 0);
  lcd.print("");
  lcd.setCursor(13, 0);
  lcd.print("");
  lcd.setCursor(14, 0);
  lcd.print("");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("e");
  lcd.setCursor(1, 1);
  lcd.print("f");
  lcd.setCursor(2, 1);
  lcd.print("f");
  lcd.setCursor(3, 1);
  lcd.print("e");
  lcd.setCursor(4, 1);
  lcd.print("t");
  lcd.setCursor(5, 1);
  lcd.print("t");
  lcd.setCursor(6, 1);
  lcd.print("o");
  lcd.setCursor(7, 1);
  lcd.print("");
  lcd.setCursor(8, 1);
  lcd.print("4");
  lcd.setCursor(9, 1);
  lcd.print("");
  lcd.setCursor(10, 1);
  lcd.print("A");
  lcd.setCursor(11, 1);
  lcd.print("T");
  lcd.setCursor(12, 1);
  lcd.print("T");
  lcd.setCursor(13, 1);
  lcd.print("I");
  lcd.setCursor(14, 1);
  lcd.print("V");
  lcd.setCursor(15, 1);
  lcd.print("O");
}


//lcd "CAMERA 3 tutti i led ON"
void lcd_tutti_led_on_camera3() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("C");
  lcd.setCursor(1, 0);
  lcd.print("a");
  lcd.setCursor(2, 0);
  lcd.print("m");
  lcd.setCursor(3, 0);
  lcd.print("e");
  lcd.setCursor(4, 0);
  lcd.print("r");
  lcd.setCursor(5, 0);
  lcd.print("a");
  lcd.setCursor(6, 0);
  lcd.print("");
  lcd.setCursor(7, 0);
  lcd.print("3");
  lcd.setCursor(8, 0);
  lcd.print("");
  lcd.setCursor(9, 0);
  lcd.print("");
  lcd.setCursor(10, 0);
  lcd.print("");
  lcd.setCursor(11, 0);
  lcd.print("");
  lcd.setCursor(12, 0);
  lcd.print("");
  lcd.setCursor(13, 0);
  lcd.print("");
  lcd.setCursor(14, 0);
  lcd.print("");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("t");
  lcd.setCursor(1, 1);
  lcd.print("u");
  lcd.setCursor(2, 1);
  lcd.print("t");
  lcd.setCursor(3, 1);
  lcd.print("t");
  lcd.setCursor(4, 1);
  lcd.print("i");
  lcd.setCursor(5, 1);
  lcd.print("");
  lcd.setCursor(6, 1);
  lcd.print("i");
  lcd.setCursor(7, 1);
  lcd.print("");
  lcd.setCursor(8, 1);
  lcd.print("L");
  lcd.setCursor(9, 1);
  lcd.print("E");
  lcd.setCursor(10, 1);
  lcd.print("D");
  lcd.setCursor(11, 1);
  lcd.print("");
  lcd.setCursor(12, 1);
  lcd.print("O");
  lcd.setCursor(13, 1);
  lcd.print("N");
  lcd.setCursor(14, 1);
  lcd.print("");
  lcd.setCursor(15, 1);
  lcd.print("");
  delay(w);
}

//lcd "CAMERA 3 tutti i led OFF"
void lcd_tutti_led_off_camera3() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("C");
  lcd.setCursor(1, 0);
  lcd.print("a");
  lcd.setCursor(2, 0);
  lcd.print("m");
  lcd.setCursor(3, 0);
  lcd.print("e");
  lcd.setCursor(4, 0);
  lcd.print("r");
  lcd.setCursor(5, 0);
  lcd.print("a");
  lcd.setCursor(6, 0);
  lcd.print("");
  lcd.setCursor(7, 0);
  lcd.print("3");
  lcd.setCursor(8, 0);
  lcd.print("");
  lcd.setCursor(9, 0);
  lcd.print("");
  lcd.setCursor(10, 0);
  lcd.print("");
  lcd.setCursor(11, 0);
  lcd.print("");
  lcd.setCursor(12, 0);
  lcd.print("");
  lcd.setCursor(13, 0);
  lcd.print("");
  lcd.setCursor(14, 0);
  lcd.print("");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("t");
  lcd.setCursor(1, 1);
  lcd.print("u");
  lcd.setCursor(2, 1);
  lcd.print("t");
  lcd.setCursor(3, 1);
  lcd.print("t");
  lcd.setCursor(4, 1);
  lcd.print("i");
  lcd.setCursor(5, 1);
  lcd.print("");
  lcd.setCursor(6, 1);
  lcd.print("i");
  lcd.setCursor(7, 1);
  lcd.print("");
  lcd.setCursor(8, 1);
  lcd.print("L");
  lcd.setCursor(9, 1);
  lcd.print("E");
  lcd.setCursor(10, 1);
  lcd.print("D");
  lcd.setCursor(11, 1);
  lcd.print("");
  lcd.setCursor(12, 1);
  lcd.print("O");
  lcd.setCursor(13, 1);
  lcd.print("F");
  lcd.setCursor(14, 1);
  lcd.print("F");
  lcd.setCursor(15, 1);
  lcd.print("");
  delay(w);
}

//lcd "CAMERA 3 LED grandi ON"
void lcd_led_grandi_on_camera3() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("C");
  lcd.setCursor(1, 0);
  lcd.print("a");
  lcd.setCursor(2, 0);
  lcd.print("m");
  lcd.setCursor(3, 0);
  lcd.print("e");
  lcd.setCursor(4, 0);
  lcd.print("r");
  lcd.setCursor(5, 0);
  lcd.print("a");
  lcd.setCursor(6, 0);
  lcd.print("");
  lcd.setCursor(7, 0);
  lcd.print("3");
  lcd.setCursor(8, 0);
  lcd.print("");
  lcd.setCursor(9, 0);
  lcd.print("");
  lcd.setCursor(10, 0);
  lcd.print("");
  lcd.setCursor(11, 0);
  lcd.print("");
  lcd.setCursor(12, 0);
  lcd.print("");
  lcd.setCursor(13, 0);
  lcd.print("");
  lcd.setCursor(14, 0);
  lcd.print("");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("L");
  lcd.setCursor(1, 1);
  lcd.print("E");
  lcd.setCursor(2, 1);
  lcd.print("D");
  lcd.setCursor(3, 1);
  lcd.print("");
  lcd.setCursor(4, 1);
  lcd.print("g");
  lcd.setCursor(5, 1);
  lcd.print("r");
  lcd.setCursor(6, 1);
  lcd.print("a");
  lcd.setCursor(7, 1);
  lcd.print("n");
  lcd.setCursor(8, 1);
  lcd.print("d");
  lcd.setCursor(9, 1);
  lcd.print("i");
  lcd.setCursor(10, 1);
  lcd.print("");
  lcd.setCursor(11, 1);
  lcd.print("O");
  lcd.setCursor(12, 1);
  lcd.print("N");
  lcd.setCursor(13, 1);
  lcd.print("");
  lcd.setCursor(14, 1);
  lcd.print("");
  lcd.setCursor(15, 1);
  lcd.print("");
  delay(w);
}

//lcd "CAMERA 3 LED piccoli ON"
void lcd_led_piccoli_on_camera3() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("C");
  lcd.setCursor(1, 0);
  lcd.print("a");
  lcd.setCursor(2, 0);
  lcd.print("m");
  lcd.setCursor(3, 0);
  lcd.print("e");
  lcd.setCursor(4, 0);
  lcd.print("r");
  lcd.setCursor(5, 0);
  lcd.print("a");
  lcd.setCursor(6, 0);
  lcd.print("");
  lcd.setCursor(7, 0);
  lcd.print("3");
  lcd.setCursor(8, 0);
  lcd.print("");
  lcd.setCursor(9, 0);
  lcd.print("");
  lcd.setCursor(10, 0);
  lcd.print("");
  lcd.setCursor(11, 0);
  lcd.print("");
  lcd.setCursor(12, 0);
  lcd.print("");
  lcd.setCursor(13, 0);
  lcd.print("");
  lcd.setCursor(14, 0);
  lcd.print("");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("L");
  lcd.setCursor(1, 1);
  lcd.print("E");
  lcd.setCursor(2, 1);
  lcd.print("D");
  lcd.setCursor(3, 1);
  lcd.print("");
  lcd.setCursor(4, 1);
  lcd.print("p");
  lcd.setCursor(5, 1);
  lcd.print("i");
  lcd.setCursor(6, 1);
  lcd.print("c");
  lcd.setCursor(7, 1);
  lcd.print("c");
  lcd.setCursor(8, 1);
  lcd.print("o");
  lcd.setCursor(9, 1);
  lcd.print("l");
  lcd.setCursor(10, 1);
  lcd.print("i");
  lcd.setCursor(11, 1);
  lcd.print("");
  lcd.setCursor(12, 1);
  lcd.print("O");
  lcd.setCursor(13, 1);
  lcd.print("N");
  lcd.setCursor(14, 1);
  lcd.print("");
  lcd.setCursor(15, 1);
  lcd.print("");
  delay(w);
}

//lcd "Camera 3 effetto 1 ATTIVO"
void lcd_effetto_uno_camera3() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("C");
  lcd.setCursor(1, 0);
  lcd.print("a");
  lcd.setCursor(2, 0);
  lcd.print("m");
  lcd.setCursor(3, 0);
  lcd.print("e");
  lcd.setCursor(4, 0);
  lcd.print("r");
  lcd.setCursor(5, 0);
  lcd.print("a");
  lcd.setCursor(6, 0);
  lcd.print("");
  lcd.setCursor(7, 0);
  lcd.print("3");
  lcd.setCursor(8, 0);
  lcd.print("");
  lcd.setCursor(9, 0);
  lcd.print("");
  lcd.setCursor(10, 0);
  lcd.print("");
  lcd.setCursor(11, 0);
  lcd.print("");
  lcd.setCursor(12, 0);
  lcd.print("");
  lcd.setCursor(13, 0);
  lcd.print("");
  lcd.setCursor(14, 0);
  lcd.print("");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("e");
  lcd.setCursor(1, 1);
  lcd.print("f");
  lcd.setCursor(2, 1);
  lcd.print("f");
  lcd.setCursor(3, 1);
  lcd.print("e");
  lcd.setCursor(4, 1);
  lcd.print("t");
  lcd.setCursor(5, 1);
  lcd.print("t");
  lcd.setCursor(6, 1);
  lcd.print("o");
  lcd.setCursor(7, 1);
  lcd.print("");
  lcd.setCursor(8, 1);
  lcd.print("1");
  lcd.setCursor(9, 1);
  lcd.print("");
  lcd.setCursor(10, 1);
  lcd.print("A");
  lcd.setCursor(11, 1);
  lcd.print("T");
  lcd.setCursor(12, 1);
  lcd.print("T");
  lcd.setCursor(13, 1);
  lcd.print("I");
  lcd.setCursor(14, 1);
  lcd.print("V");
  lcd.setCursor(15, 1);
  lcd.print("O");
}

//lcd "Camera 3 effetto 2 ATTIVO"
void lcd_effetto_due_camera3() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("C");
  lcd.setCursor(1, 0);
  lcd.print("a");
  lcd.setCursor(2, 0);
  lcd.print("m");
  lcd.setCursor(3, 0);
  lcd.print("e");
  lcd.setCursor(4, 0);
  lcd.print("r");
  lcd.setCursor(5, 0);
  lcd.print("a");
  lcd.setCursor(6, 0);
  lcd.print("");
  lcd.setCursor(7, 0);
  lcd.print("3");
  lcd.setCursor(8, 0);
  lcd.print("");
  lcd.setCursor(9, 0);
  lcd.print("");
  lcd.setCursor(10, 0);
  lcd.print("");
  lcd.setCursor(11, 0);
  lcd.print("");
  lcd.setCursor(12, 0);
  lcd.print("");
  lcd.setCursor(13, 0);
  lcd.print("");
  lcd.setCursor(14, 0);
  lcd.print("");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("e");
  lcd.setCursor(1, 1);
  lcd.print("f");
  lcd.setCursor(2, 1);
  lcd.print("f");
  lcd.setCursor(3, 1);
  lcd.print("e");
  lcd.setCursor(4, 1);
  lcd.print("t");
  lcd.setCursor(5, 1);
  lcd.print("t");
  lcd.setCursor(6, 1);
  lcd.print("o");
  lcd.setCursor(7, 1);
  lcd.print("");
  lcd.setCursor(8, 1);
  lcd.print("2");
  lcd.setCursor(9, 1);
  lcd.print("");
  lcd.setCursor(10, 1);
  lcd.print("A");
  lcd.setCursor(11, 1);
  lcd.print("T");
  lcd.setCursor(12, 1);
  lcd.print("T");
  lcd.setCursor(13, 1);
  lcd.print("I");
  lcd.setCursor(14, 1);
  lcd.print("V");
  lcd.setCursor(15, 1);
  lcd.print("O");
}

//lcd "Camera 3 effetto 3 ATTIVO"
void lcd_effetto_tre_camera3() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("C");
  lcd.setCursor(1, 0);
  lcd.print("a");
  lcd.setCursor(2, 0);
  lcd.print("m");
  lcd.setCursor(3, 0);
  lcd.print("e");
  lcd.setCursor(4, 0);
  lcd.print("r");
  lcd.setCursor(5, 0);
  lcd.print("a");
  lcd.setCursor(6, 0);
  lcd.print("");
  lcd.setCursor(7, 0);
  lcd.print("3");
  lcd.setCursor(8, 0);
  lcd.print("");
  lcd.setCursor(9, 0);
  lcd.print("");
  lcd.setCursor(10, 0);
  lcd.print("");
  lcd.setCursor(11, 0);
  lcd.print("");
  lcd.setCursor(12, 0);
  lcd.print("");
  lcd.setCursor(13, 0);
  lcd.print("");
  lcd.setCursor(14, 0);
  lcd.print("");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("e");
  lcd.setCursor(1, 1);
  lcd.print("f");
  lcd.setCursor(2, 1);
  lcd.print("f");
  lcd.setCursor(3, 1);
  lcd.print("e");
  lcd.setCursor(4, 1);
  lcd.print("t");
  lcd.setCursor(5, 1);
  lcd.print("t");
  lcd.setCursor(6, 1);
  lcd.print("o");
  lcd.setCursor(7, 1);
  lcd.print("");
  lcd.setCursor(8, 1);
  lcd.print("3");
  lcd.setCursor(9, 1);
  lcd.print("");
  lcd.setCursor(10, 1);
  lcd.print("A");
  lcd.setCursor(11, 1);
  lcd.print("T");
  lcd.setCursor(12, 1);
  lcd.print("T");
  lcd.setCursor(13, 1);
  lcd.print("I");
  lcd.setCursor(14, 1);
  lcd.print("V");
  lcd.setCursor(15, 1);
  lcd.print("O");
}

//lcd "Camera 3 effetto 4 ATTIVO"
void lcd_effetto_quattro_camera3() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("C");
  lcd.setCursor(1, 0);
  lcd.print("a");
  lcd.setCursor(2, 0);
  lcd.print("m");
  lcd.setCursor(3, 0);
  lcd.print("e");
  lcd.setCursor(4, 0);
  lcd.print("r");
  lcd.setCursor(5, 0);
  lcd.print("a");
  lcd.setCursor(6, 0);
  lcd.print("");
  lcd.setCursor(7, 0);
  lcd.print("3");
  lcd.setCursor(8, 0);
  lcd.print("");
  lcd.setCursor(9, 0);
  lcd.print("");
  lcd.setCursor(10, 0);
  lcd.print("");
  lcd.setCursor(11, 0);
  lcd.print("");
  lcd.setCursor(12, 0);
  lcd.print("");
  lcd.setCursor(13, 0);
  lcd.print("");
  lcd.setCursor(14, 0);
  lcd.print("");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("e");
  lcd.setCursor(1, 1);
  lcd.print("f");
  lcd.setCursor(2, 1);
  lcd.print("f");
  lcd.setCursor(3, 1);
  lcd.print("e");
  lcd.setCursor(4, 1);
  lcd.print("t");
  lcd.setCursor(5, 1);
  lcd.print("t");
  lcd.setCursor(6, 1);
  lcd.print("o");
  lcd.setCursor(7, 1);
  lcd.print("");
  lcd.setCursor(8, 1);
  lcd.print("4");
  lcd.setCursor(9, 1);
  lcd.print("");
  lcd.setCursor(10, 1);
  lcd.print("A");
  lcd.setCursor(11, 1);
  lcd.print("T");
  lcd.setCursor(12, 1);
  lcd.print("T");
  lcd.setCursor(13, 1);
  lcd.print("I");
  lcd.setCursor(14, 1);
  lcd.print("V");
  lcd.setCursor(15, 1);
  lcd.print("O");
}


//CAMERA 4
//lcd "CAMERA 4 tutti i led ON"
void lcd_tutti_led_on_camera4() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("C");
  lcd.setCursor(1, 0);
  lcd.print("a");
  lcd.setCursor(2, 0);
  lcd.print("m");
  lcd.setCursor(3, 0);
  lcd.print("e");
  lcd.setCursor(4, 0);
  lcd.print("r");
  lcd.setCursor(5, 0);
  lcd.print("a");
  lcd.setCursor(6, 0);
  lcd.print("");
  lcd.setCursor(7, 0);
  lcd.print("4");
  lcd.setCursor(8, 0);
  lcd.print("");
  lcd.setCursor(9, 0);
  lcd.print("");
  lcd.setCursor(10, 0);
  lcd.print("");
  lcd.setCursor(11, 0);
  lcd.print("");
  lcd.setCursor(12, 0);
  lcd.print("");
  lcd.setCursor(13, 0);
  lcd.print("");
  lcd.setCursor(14, 0);
  lcd.print("");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("t");
  lcd.setCursor(1, 1);
  lcd.print("u");
  lcd.setCursor(2, 1);
  lcd.print("t");
  lcd.setCursor(3, 1);
  lcd.print("t");
  lcd.setCursor(4, 1);
  lcd.print("i");
  lcd.setCursor(5, 1);
  lcd.print("");
  lcd.setCursor(6, 1);
  lcd.print("i");
  lcd.setCursor(7, 1);
  lcd.print("");
  lcd.setCursor(8, 1);
  lcd.print("L");
  lcd.setCursor(9, 1);
  lcd.print("E");
  lcd.setCursor(10, 1);
  lcd.print("D");
  lcd.setCursor(11, 1);
  lcd.print("");
  lcd.setCursor(12, 1);
  lcd.print("O");
  lcd.setCursor(13, 1);
  lcd.print("N");
  lcd.setCursor(14, 1);
  lcd.print("");
  lcd.setCursor(15, 1);
  lcd.print("");
  delay(w);
}

//lcd "CAMERA 4 tutti i led OFF"
void lcd_tutti_led_off_camera4() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("C");
  lcd.setCursor(1, 0);
  lcd.print("a");
  lcd.setCursor(2, 0);
  lcd.print("m");
  lcd.setCursor(3, 0);
  lcd.print("e");
  lcd.setCursor(4, 0);
  lcd.print("r");
  lcd.setCursor(5, 0);
  lcd.print("a");
  lcd.setCursor(6, 0);
  lcd.print("");
  lcd.setCursor(7, 0);
  lcd.print("4");
  lcd.setCursor(8, 0);
  lcd.print("");
  lcd.setCursor(9, 0);
  lcd.print("");
  lcd.setCursor(10, 0);
  lcd.print("");
  lcd.setCursor(11, 0);
  lcd.print("");
  lcd.setCursor(12, 0);
  lcd.print("");
  lcd.setCursor(13, 0);
  lcd.print("");
  lcd.setCursor(14, 0);
  lcd.print("");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("t");
  lcd.setCursor(1, 1);
  lcd.print("u");
  lcd.setCursor(2, 1);
  lcd.print("t");
  lcd.setCursor(3, 1);
  lcd.print("t");
  lcd.setCursor(4, 1);
  lcd.print("i");
  lcd.setCursor(5, 1);
  lcd.print("");
  lcd.setCursor(6, 1);
  lcd.print("i");
  lcd.setCursor(7, 1);
  lcd.print("");
  lcd.setCursor(8, 1);
  lcd.print("L");
  lcd.setCursor(9, 1);
  lcd.print("E");
  lcd.setCursor(10, 1);
  lcd.print("D");
  lcd.setCursor(11, 1);
  lcd.print("");
  lcd.setCursor(12, 1);
  lcd.print("O");
  lcd.setCursor(13, 1);
  lcd.print("F");
  lcd.setCursor(14, 1);
  lcd.print("F");
  lcd.setCursor(15, 1);
  lcd.print("");
  delay(w);
}

//lcd "CAMERA 4 LED grandi ON"
void lcd_led_grandi_on_camera4() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("C");
  lcd.setCursor(1, 0);
  lcd.print("a");
  lcd.setCursor(2, 0);
  lcd.print("m");
  lcd.setCursor(3, 0);
  lcd.print("e");
  lcd.setCursor(4, 0);
  lcd.print("r");
  lcd.setCursor(5, 0);
  lcd.print("a");
  lcd.setCursor(6, 0);
  lcd.print("");
  lcd.setCursor(7, 0);
  lcd.print("4");
  lcd.setCursor(8, 0);
  lcd.print("");
  lcd.setCursor(9, 0);
  lcd.print("");
  lcd.setCursor(10, 0);
  lcd.print("");
  lcd.setCursor(11, 0);
  lcd.print("");
  lcd.setCursor(12, 0);
  lcd.print("");
  lcd.setCursor(13, 0);
  lcd.print("");
  lcd.setCursor(14, 0);
  lcd.print("");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("L");
  lcd.setCursor(1, 1);
  lcd.print("E");
  lcd.setCursor(2, 1);
  lcd.print("D");
  lcd.setCursor(3, 1);
  lcd.print("");
  lcd.setCursor(4, 1);
  lcd.print("g");
  lcd.setCursor(5, 1);
  lcd.print("r");
  lcd.setCursor(6, 1);
  lcd.print("a");
  lcd.setCursor(7, 1);
  lcd.print("n");
  lcd.setCursor(8, 1);
  lcd.print("d");
  lcd.setCursor(9, 1);
  lcd.print("i");
  lcd.setCursor(10, 1);
  lcd.print("");
  lcd.setCursor(11, 1);
  lcd.print("O");
  lcd.setCursor(12, 1);
  lcd.print("N");
  lcd.setCursor(13, 1);
  lcd.print("");
  lcd.setCursor(14, 1);
  lcd.print("");
  lcd.setCursor(15, 1);
  lcd.print("");
  delay(w);
}

//lcd "CAMERA 4 LED piccoli ON"
void lcd_led_piccoli_on_camera4() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("C");
  lcd.setCursor(1, 0);
  lcd.print("a");
  lcd.setCursor(2, 0);
  lcd.print("m");
  lcd.setCursor(3, 0);
  lcd.print("e");
  lcd.setCursor(4, 0);
  lcd.print("r");
  lcd.setCursor(5, 0);
  lcd.print("a");
  lcd.setCursor(6, 0);
  lcd.print("");
  lcd.setCursor(7, 0);
  lcd.print("4");
  lcd.setCursor(8, 0);
  lcd.print("");
  lcd.setCursor(9, 0);
  lcd.print("");
  lcd.setCursor(10, 0);
  lcd.print("");
  lcd.setCursor(11, 0);
  lcd.print("");
  lcd.setCursor(12, 0);
  lcd.print("");
  lcd.setCursor(13, 0);
  lcd.print("");
  lcd.setCursor(14, 0);
  lcd.print("");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("L");
  lcd.setCursor(1, 1);
  lcd.print("E");
  lcd.setCursor(2, 1);
  lcd.print("D");
  lcd.setCursor(3, 1);
  lcd.print("");
  lcd.setCursor(4, 1);
  lcd.print("p");
  lcd.setCursor(5, 1);
  lcd.print("i");
  lcd.setCursor(6, 1);
  lcd.print("c");
  lcd.setCursor(7, 1);
  lcd.print("c");
  lcd.setCursor(8, 1);
  lcd.print("o");
  lcd.setCursor(9, 1);
  lcd.print("l");
  lcd.setCursor(10, 1);
  lcd.print("i");
  lcd.setCursor(11, 1);
  lcd.print("");
  lcd.setCursor(12, 1);
  lcd.print("O");
  lcd.setCursor(13, 1);
  lcd.print("N");
  lcd.setCursor(14, 1);
  lcd.print("");
  lcd.setCursor(15, 1);
  lcd.print("");
  delay(w);
}

//lcd "Camera 4 effetto 1 ATTIVO"
void lcd_effetto_uno_camera4() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("C");
  lcd.setCursor(1, 0);
  lcd.print("a");
  lcd.setCursor(2, 0);
  lcd.print("m");
  lcd.setCursor(3, 0);
  lcd.print("e");
  lcd.setCursor(4, 0);
  lcd.print("r");
  lcd.setCursor(5, 0);
  lcd.print("a");
  lcd.setCursor(6, 0);
  lcd.print("");
  lcd.setCursor(7, 0);
  lcd.print("4");
  lcd.setCursor(8, 0);
  lcd.print("");
  lcd.setCursor(9, 0);
  lcd.print("");
  lcd.setCursor(10, 0);
  lcd.print("");
  lcd.setCursor(11, 0);
  lcd.print("");
  lcd.setCursor(12, 0);
  lcd.print("");
  lcd.setCursor(13, 0);
  lcd.print("");
  lcd.setCursor(14, 0);
  lcd.print("");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("e");
  lcd.setCursor(1, 1);
  lcd.print("f");
  lcd.setCursor(2, 1);
  lcd.print("f");
  lcd.setCursor(3, 1);
  lcd.print("e");
  lcd.setCursor(4, 1);
  lcd.print("t");
  lcd.setCursor(5, 1);
  lcd.print("t");
  lcd.setCursor(6, 1);
  lcd.print("o");
  lcd.setCursor(7, 1);
  lcd.print("");
  lcd.setCursor(8, 1);
  lcd.print("1");
  lcd.setCursor(9, 1);
  lcd.print("");
  lcd.setCursor(10, 1);
  lcd.print("A");
  lcd.setCursor(11, 1);
  lcd.print("T");
  lcd.setCursor(12, 1);
  lcd.print("T");
  lcd.setCursor(13, 1);
  lcd.print("I");
  lcd.setCursor(14, 1);
  lcd.print("V");
  lcd.setCursor(15, 1);
  lcd.print("O");
}

//lcd "Camera 4 effetto 2 ATTIVO"
void lcd_effetto_due_camera4() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("C");
  lcd.setCursor(1, 0);
  lcd.print("a");
  lcd.setCursor(2, 0);
  lcd.print("m");
  lcd.setCursor(3, 0);
  lcd.print("e");
  lcd.setCursor(4, 0);
  lcd.print("r");
  lcd.setCursor(5, 0);
  lcd.print("a");
  lcd.setCursor(6, 0);
  lcd.print("");
  lcd.setCursor(7, 0);
  lcd.print("4");
  lcd.setCursor(8, 0);
  lcd.print("");
  lcd.setCursor(9, 0);
  lcd.print("");
  lcd.setCursor(10, 0);
  lcd.print("");
  lcd.setCursor(11, 0);
  lcd.print("");
  lcd.setCursor(12, 0);
  lcd.print("");
  lcd.setCursor(13, 0);
  lcd.print("");
  lcd.setCursor(14, 0);
  lcd.print("");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("e");
  lcd.setCursor(1, 1);
  lcd.print("f");
  lcd.setCursor(2, 1);
  lcd.print("f");
  lcd.setCursor(3, 1);
  lcd.print("e");
  lcd.setCursor(4, 1);
  lcd.print("t");
  lcd.setCursor(5, 1);
  lcd.print("t");
  lcd.setCursor(6, 1);
  lcd.print("o");
  lcd.setCursor(7, 1);
  lcd.print("");
  lcd.setCursor(8, 1);
  lcd.print("2");
  lcd.setCursor(9, 1);
  lcd.print("");
  lcd.setCursor(10, 1);
  lcd.print("A");
  lcd.setCursor(11, 1);
  lcd.print("T");
  lcd.setCursor(12, 1);
  lcd.print("T");
  lcd.setCursor(13, 1);
  lcd.print("I");
  lcd.setCursor(14, 1);
  lcd.print("V");
  lcd.setCursor(15, 1);
  lcd.print("O");
}

//lcd "Camera 4 effetto 3 ATTIVO"
void lcd_effetto_tre_camera4() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("C");
  lcd.setCursor(1, 0);
  lcd.print("a");
  lcd.setCursor(2, 0);
  lcd.print("m");
  lcd.setCursor(3, 0);
  lcd.print("e");
  lcd.setCursor(4, 0);
  lcd.print("r");
  lcd.setCursor(5, 0);
  lcd.print("a");
  lcd.setCursor(6, 0);
  lcd.print("");
  lcd.setCursor(7, 0);
  lcd.print("4");
  lcd.setCursor(8, 0);
  lcd.print("");
  lcd.setCursor(9, 0);
  lcd.print("");
  lcd.setCursor(10, 0);
  lcd.print("");
  lcd.setCursor(11, 0);
  lcd.print("");
  lcd.setCursor(12, 0);
  lcd.print("");
  lcd.setCursor(13, 0);
  lcd.print("");
  lcd.setCursor(14, 0);
  lcd.print("");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("e");
  lcd.setCursor(1, 1);
  lcd.print("f");
  lcd.setCursor(2, 1);
  lcd.print("f");
  lcd.setCursor(3, 1);
  lcd.print("e");
  lcd.setCursor(4, 1);
  lcd.print("t");
  lcd.setCursor(5, 1);
  lcd.print("t");
  lcd.setCursor(6, 1);
  lcd.print("o");
  lcd.setCursor(7, 1);
  lcd.print("");
  lcd.setCursor(8, 1);
  lcd.print("3");
  lcd.setCursor(9, 1);
  lcd.print("");
  lcd.setCursor(10, 1);
  lcd.print("A");
  lcd.setCursor(11, 1);
  lcd.print("T");
  lcd.setCursor(12, 1);
  lcd.print("T");
  lcd.setCursor(13, 1);
  lcd.print("I");
  lcd.setCursor(14, 1);
  lcd.print("V");
  lcd.setCursor(15, 1);
  lcd.print("O");
}

//lcd "Camera 4 effetto 4 ATTIVO"
void lcd_effetto_quattro_camera4() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("C");
  lcd.setCursor(1, 0);
  lcd.print("a");
  lcd.setCursor(2, 0);
  lcd.print("m");
  lcd.setCursor(3, 0);
  lcd.print("e");
  lcd.setCursor(4, 0);
  lcd.print("r");
  lcd.setCursor(5, 0);
  lcd.print("a");
  lcd.setCursor(6, 0);
  lcd.print("");
  lcd.setCursor(7, 0);
  lcd.print("4");
  lcd.setCursor(8, 0);
  lcd.print("");
  lcd.setCursor(9, 0);
  lcd.print("");
  lcd.setCursor(10, 0);
  lcd.print("");
  lcd.setCursor(11, 0);
  lcd.print("");
  lcd.setCursor(12, 0);
  lcd.print("");
  lcd.setCursor(13, 0);
  lcd.print("");
  lcd.setCursor(14, 0);
  lcd.print("");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("e");
  lcd.setCursor(1, 1);
  lcd.print("f");
  lcd.setCursor(2, 1);
  lcd.print("f");
  lcd.setCursor(3, 1);
  lcd.print("e");
  lcd.setCursor(4, 1);
  lcd.print("t");
  lcd.setCursor(5, 1);
  lcd.print("t");
  lcd.setCursor(6, 1);
  lcd.print("o");
  lcd.setCursor(7, 1);
  lcd.print("");
  lcd.setCursor(8, 1);
  lcd.print("4");
  lcd.setCursor(9, 1);
  lcd.print("");
  lcd.setCursor(10, 1);
  lcd.print("A");
  lcd.setCursor(11, 1);
  lcd.print("T");
  lcd.setCursor(12, 1);
  lcd.print("T");
  lcd.setCursor(13, 1);
  lcd.print("I");
  lcd.setCursor(14, 1);
  lcd.print("V");
  lcd.setCursor(15, 1);
  lcd.print("O");
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//lcd "LED fontanella ATTIVATI"
void lcd_led_fontanella_accesi() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("");
  lcd.setCursor(1, 0);
  lcd.print("L");
  lcd.setCursor(2, 0);
  lcd.print("E");
  lcd.setCursor(3, 0);
  lcd.print("D");
  lcd.setCursor(4, 0);
  lcd.print("");
  lcd.setCursor(5, 0);
  lcd.print("f");
  lcd.setCursor(6, 0);
  lcd.print("o");
  lcd.setCursor(7, 0);
  lcd.print("n");
  lcd.setCursor(8, 0);
  lcd.print("t");
  lcd.setCursor(9, 0);
  lcd.print("a");
  lcd.setCursor(10, 0);
  lcd.print("n");
  lcd.setCursor(11, 0);
  lcd.print("e");
  lcd.setCursor(12, 0);
  lcd.print("l");
  lcd.setCursor(13, 0);
  lcd.print("l");
  lcd.setCursor(14, 0);
  lcd.print("a");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("");
  lcd.setCursor(1, 1);
  lcd.print("");
  lcd.setCursor(2, 1);
  lcd.print("");
  lcd.setCursor(3, 1);
  lcd.print("");
  lcd.setCursor(4, 1);
  lcd.print("A");
  lcd.setCursor(5, 1);
  lcd.print("T");
  lcd.setCursor(6, 1);
  lcd.print("T");
  lcd.setCursor(7, 1);
  lcd.print("I");
  lcd.setCursor(8, 1);
  lcd.print("V");
  lcd.setCursor(9, 1);
  lcd.print("A");
  lcd.setCursor(10, 1);
  lcd.print("T");
  lcd.setCursor(11, 1);
  lcd.print("I");
  lcd.setCursor(12, 1);
  lcd.print("");
  lcd.setCursor(13, 1);
  lcd.print("");
  lcd.setCursor(14, 1);
  lcd.print("");
  lcd.setCursor(15, 1);
  lcd.print("");
  delay(w);
}

//lcd "LED fontanella DISATTIVATI"
void lcd_led_fontanella_spenti() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("");
  lcd.setCursor(1, 0);
  lcd.print("L");
  lcd.setCursor(2, 0);
  lcd.print("E");
  lcd.setCursor(3, 0);
  lcd.print("D");
  lcd.setCursor(4, 0);
  lcd.print("");
  lcd.setCursor(5, 0);
  lcd.print("f");
  lcd.setCursor(6, 0);
  lcd.print("o");
  lcd.setCursor(7, 0);
  lcd.print("n");
  lcd.setCursor(8, 0);
  lcd.print("t");
  lcd.setCursor(9, 0);
  lcd.print("a");
  lcd.setCursor(10, 0);
  lcd.print("n");
  lcd.setCursor(11, 0);
  lcd.print("e");
  lcd.setCursor(12, 0);
  lcd.print("l");
  lcd.setCursor(13, 0);
  lcd.print("l");
  lcd.setCursor(14, 0);
  lcd.print("a");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("");
  lcd.setCursor(1, 1);
  lcd.print("");
  lcd.setCursor(2, 1);
  lcd.print("");
  lcd.setCursor(3, 1);
  lcd.print("D");
  lcd.setCursor(4, 1);
  lcd.print("I");
  lcd.setCursor(5, 1);
  lcd.print("S");
  lcd.setCursor(6, 1);
  lcd.print("A");
  lcd.setCursor(7, 1);
  lcd.print("T");
  lcd.setCursor(8, 1);
  lcd.print("T");
  lcd.setCursor(9, 1);
  lcd.print("I");
  lcd.setCursor(10, 1);
  lcd.print("V");
  lcd.setCursor(11, 1);
  lcd.print("A");
  lcd.setCursor(12, 1);
  lcd.print("T");
  lcd.setCursor(13, 1);
  lcd.print("I");
  lcd.setCursor(14, 1);
  lcd.print("");
  lcd.setCursor(15, 1);
  lcd.print("");
  delay(w);
}

//lcd "FONATENELLA DISATTIVATA"
void lcd_spegnimento_fontanella() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("");
  lcd.setCursor(1, 0);
  lcd.print("");
  lcd.setCursor(2, 0);
  lcd.print("");
  lcd.setCursor(3, 0);
  lcd.print("F");
  lcd.setCursor(4, 0);
  lcd.print("O");
  lcd.setCursor(5, 0);
  lcd.print("N");
  lcd.setCursor(6, 0);
  lcd.print("T");
  lcd.setCursor(7, 0);
  lcd.print("A");
  lcd.setCursor(8, 0);
  lcd.print("N");
  lcd.setCursor(9, 0);
  lcd.print("E");
  lcd.setCursor(10, 0);
  lcd.print("L");
  lcd.setCursor(11, 0);
  lcd.print("L");
  lcd.setCursor(12, 0);
  lcd.print("A");
  lcd.setCursor(13, 0);
  lcd.print("");
  lcd.setCursor(14, 0);
  lcd.print("");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("");
  lcd.setCursor(1, 1);
  lcd.print("");
  lcd.setCursor(2, 1);
  lcd.print("");
  lcd.setCursor(3, 1);
  lcd.print("D");
  lcd.setCursor(4, 1);
  lcd.print("I");
  lcd.setCursor(5, 1);
  lcd.print("S");
  lcd.setCursor(6, 1);
  lcd.print("A");
  lcd.setCursor(7, 1);
  lcd.print("T");
  lcd.setCursor(8, 1);
  lcd.print("T");
  lcd.setCursor(9, 1);
  lcd.print("I");
  lcd.setCursor(10, 1);
  lcd.print("V");
  lcd.setCursor(11, 1);
  lcd.print("A");
  lcd.setCursor(12, 1);
  lcd.print("T");
  lcd.setCursor(13, 1);
  lcd.print("A");
  lcd.setCursor(14, 1);
  lcd.print("");
  lcd.setCursor(15, 1);
  lcd.print("");
  delay(w);
}

//lcd "FONATENELLA ATTIVATA"
void lcd_accensione_fontanella() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("");
  lcd.setCursor(1, 0);
  lcd.print("");
  lcd.setCursor(2, 0);
  lcd.print("");
  lcd.setCursor(3, 0);
  lcd.print("F");
  lcd.setCursor(4, 0);
  lcd.print("O");
  lcd.setCursor(5, 0);
  lcd.print("N");
  lcd.setCursor(6, 0);
  lcd.print("T");
  lcd.setCursor(7, 0);
  lcd.print("A");
  lcd.setCursor(8, 0);
  lcd.print("N");
  lcd.setCursor(9, 0);
  lcd.print("E");
  lcd.setCursor(10, 0);
  lcd.print("L");
  lcd.setCursor(11, 0);
  lcd.print("L");
  lcd.setCursor(12, 0);
  lcd.print("A");
  lcd.setCursor(13, 0);
  lcd.print("");
  lcd.setCursor(14, 0);
  lcd.print("");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("");
  lcd.setCursor(1, 1);
  lcd.print("");
  lcd.setCursor(2, 1);
  lcd.print("");
  lcd.setCursor(3, 1);
  lcd.print("");
  lcd.setCursor(4, 1);
  lcd.print("A");
  lcd.setCursor(5, 1);
  lcd.print("T");
  lcd.setCursor(6, 1);
  lcd.print("T");
  lcd.setCursor(7, 1);
  lcd.print("I");
  lcd.setCursor(8, 1);
  lcd.print("V");
  lcd.setCursor(9, 1);
  lcd.print("A");
  lcd.setCursor(10, 1);
  lcd.print("T");
  lcd.setCursor(11, 1);
  lcd.print("A");
  lcd.setCursor(12, 1);
  lcd.print("");
  lcd.setCursor(13, 1);
  lcd.print("");
  lcd.setCursor(14, 1);
  lcd.print("");
  lcd.setCursor(15, 1);
  lcd.print("");
  delay(w);
}

//lcd "ril. LUMINOSITA' ATTIVATO"
void lcd_luminosita_on() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("r");
  lcd.setCursor(1, 0);
  lcd.print("i");
  lcd.setCursor(2, 0);
  lcd.print("l");
  lcd.setCursor(3, 0);
  lcd.print(".");
  lcd.setCursor(4, 0);
  lcd.print("");
  lcd.setCursor(5, 0);
  lcd.print("L");
  lcd.setCursor(6, 0);
  lcd.print("U");
  lcd.setCursor(7, 0);
  lcd.print("M");
  lcd.setCursor(8, 0);
  lcd.print("I");
  lcd.setCursor(9, 0);
  lcd.print("N");
  lcd.setCursor(10, 0);
  lcd.print("O");
  lcd.setCursor(11, 0);
  lcd.print("S");
  lcd.setCursor(12, 0);
  lcd.print("I");
  lcd.setCursor(13, 0);
  lcd.print("T");
  lcd.setCursor(14, 0);
  lcd.print("A");
  lcd.setCursor(15, 0);
  lcd.print("'");

  lcd.setCursor(0, 1);
  lcd.print("");
  lcd.setCursor(1, 1);
  lcd.print("");
  lcd.setCursor(2, 1);
  lcd.print("");
  lcd.setCursor(3, 1);
  lcd.print("");
  lcd.setCursor(4, 1);
  lcd.print("A");
  lcd.setCursor(5, 1);
  lcd.print("T");
  lcd.setCursor(6, 1);
  lcd.print("T");
  lcd.setCursor(7, 1);
  lcd.print("I");
  lcd.setCursor(8, 1);
  lcd.print("V");
  lcd.setCursor(9, 1);
  lcd.print("A");
  lcd.setCursor(10, 1);
  lcd.print("T");
  lcd.setCursor(11, 1);
  lcd.print("O");
  lcd.setCursor(12, 1);
  lcd.print("");
  lcd.setCursor(13, 1);
  lcd.print("");
  lcd.setCursor(14, 1);
  lcd.print("");
  lcd.setCursor(15, 1);
  lcd.print("");
  delay(w_sensor);
}

//lcd "ril. LUMINOSITA' DISATTIVATO"
void lcd_luminosita_off() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("r");
  lcd.setCursor(1, 0);
  lcd.print("i");
  lcd.setCursor(2, 0);
  lcd.print("l");
  lcd.setCursor(3, 0);
  lcd.print(".");
  lcd.setCursor(4, 0);
  lcd.print("");
  lcd.setCursor(5, 0);
  lcd.print("L");
  lcd.setCursor(6, 0);
  lcd.print("U");
  lcd.setCursor(7, 0);
  lcd.print("M");
  lcd.setCursor(8, 0);
  lcd.print("I");
  lcd.setCursor(9, 0);
  lcd.print("N");
  lcd.setCursor(10, 0);
  lcd.print("O");
  lcd.setCursor(11, 0);
  lcd.print("S");
  lcd.setCursor(12, 0);
  lcd.print("I");
  lcd.setCursor(13, 0);
  lcd.print("T");
  lcd.setCursor(14, 0);
  lcd.print("A");
  lcd.setCursor(15, 0);
  lcd.print("'");

  lcd.setCursor(0, 1);
  lcd.print("");
  lcd.setCursor(1, 1);
  lcd.print("");
  lcd.setCursor(2, 1);
  lcd.print("");
  lcd.setCursor(3, 1);
  lcd.print("D");
  lcd.setCursor(4, 1);
  lcd.print("I");
  lcd.setCursor(5, 1);
  lcd.print("S");
  lcd.setCursor(6, 1);
  lcd.print("A");
  lcd.setCursor(7, 1);
  lcd.print("T");
  lcd.setCursor(8, 1);
  lcd.print("T");
  lcd.setCursor(9, 1);
  lcd.print("I");
  lcd.setCursor(10, 1);
  lcd.print("V");
  lcd.setCursor(11, 1);
  lcd.print("A");
  lcd.setCursor(12, 1);
  lcd.print("T");
  lcd.setCursor(13, 1);
  lcd.print("O");
  lcd.setCursor(14, 1);
  lcd.print("");
  lcd.setCursor(15, 1);
  lcd.print("");
  delay(w_sensor);
}

//lcd "LED giardino ATTIVATI"
void lcd_illuminazione_giardino_on() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("");
  lcd.setCursor(1, 0);
  lcd.print("");
  lcd.setCursor(2, 0);
  lcd.print("L");
  lcd.setCursor(3, 0);
  lcd.print("E");
  lcd.setCursor(4, 0);
  lcd.print("D");
  lcd.setCursor(5, 0);
  lcd.print("");
  lcd.setCursor(6, 0);
  lcd.print("g");
  lcd.setCursor(7, 0);
  lcd.print("i");
  lcd.setCursor(8, 0);
  lcd.print("a");
  lcd.setCursor(9, 0);
  lcd.print("r");
  lcd.setCursor(10, 0);
  lcd.print("d");
  lcd.setCursor(11, 0);
  lcd.print("i");
  lcd.setCursor(12, 0);
  lcd.print("n");
  lcd.setCursor(13, 0);
  lcd.print("o");
  lcd.setCursor(14, 0);
  lcd.print("");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("");
  lcd.setCursor(1, 1);
  lcd.print("");
  lcd.setCursor(2, 1);
  lcd.print("");
  lcd.setCursor(3, 1);
  lcd.print("");
  lcd.setCursor(4, 1);
  lcd.print("A");
  lcd.setCursor(5, 1);
  lcd.print("T");
  lcd.setCursor(6, 1);
  lcd.print("T");
  lcd.setCursor(7, 1);
  lcd.print("I");
  lcd.setCursor(8, 1);
  lcd.print("V");
  lcd.setCursor(9, 1);
  lcd.print("A");
  lcd.setCursor(10, 1);
  lcd.print("T");
  lcd.setCursor(11, 1);
  lcd.print("I");
  lcd.setCursor(12, 1);
  lcd.print("");
  lcd.setCursor(13, 1);
  lcd.print("");
  lcd.setCursor(14, 1);
  lcd.print("");
  lcd.setCursor(15, 1);
  lcd.print("");
  delay(w);
}

//lcd "LED giardino DISATTIVATI"
void lcd_illuminazione_giardino_off() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("");
  lcd.setCursor(1, 0);
  lcd.print("");
  lcd.setCursor(2, 0);
  lcd.print("L");
  lcd.setCursor(3, 0);
  lcd.print("E");
  lcd.setCursor(4, 0);
  lcd.print("D");
  lcd.setCursor(5, 0);
  lcd.print("");
  lcd.setCursor(6, 0);
  lcd.print("g");
  lcd.setCursor(7, 0);
  lcd.print("i");
  lcd.setCursor(8, 0);
  lcd.print("a");
  lcd.setCursor(9, 0);
  lcd.print("r");
  lcd.setCursor(10, 0);
  lcd.print("d");
  lcd.setCursor(11, 0);
  lcd.print("i");
  lcd.setCursor(12, 0);
  lcd.print("n");
  lcd.setCursor(13, 0);
  lcd.print("o");
  lcd.setCursor(14, 0);
  lcd.print("");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("");
  lcd.setCursor(1, 1);
  lcd.print("");
  lcd.setCursor(2, 1);
  lcd.print("");
  lcd.setCursor(3, 1);
  lcd.print("D");
  lcd.setCursor(4, 1);
  lcd.print("I");
  lcd.setCursor(5, 1);
  lcd.print("S");
  lcd.setCursor(6, 1);
  lcd.print("A");
  lcd.setCursor(7, 1);
  lcd.print("T");
  lcd.setCursor(8, 1);
  lcd.print("T");
  lcd.setCursor(9, 1);
  lcd.print("I");
  lcd.setCursor(10, 1);
  lcd.print("V");
  lcd.setCursor(11, 1);
  lcd.print("A");
  lcd.setCursor(12, 1);
  lcd.print("T");
  lcd.setCursor(13, 1);
  lcd.print("I");
  lcd.setCursor(14, 1);
  lcd.print("");
  lcd.setCursor(15, 1);
  lcd.print("");
  delay(w);
}

//lcd "PORTA PRINCIPALE APERTA"
void lcd_porta_principale_aperta() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("P");
  lcd.setCursor(1, 0);
  lcd.print("O");
  lcd.setCursor(2, 0);
  lcd.print("R");
  lcd.setCursor(3, 0);
  lcd.print("T");
  lcd.setCursor(4, 0);
  lcd.print("A");
  lcd.setCursor(5, 0);
  lcd.print("");
  lcd.setCursor(6, 0);
  lcd.print("P");
  lcd.setCursor(7, 0);
  lcd.print("R");
  lcd.setCursor(8, 0);
  lcd.print("I");
  lcd.setCursor(9, 0);
  lcd.print("N");
  lcd.setCursor(10, 0);
  lcd.print("C");
  lcd.setCursor(11, 0);
  lcd.print("I");
  lcd.setCursor(12, 0);
  lcd.print("P");
  lcd.setCursor(13, 0);
  lcd.print("A");
  lcd.setCursor(14, 0);
  lcd.print("L");
  lcd.setCursor(15, 0);
  lcd.print("E");

  lcd.setCursor(0, 1);
  lcd.print("");
  lcd.setCursor(1, 1);
  lcd.print("");
  lcd.setCursor(2, 1);
  lcd.print("");
  lcd.setCursor(3, 1);
  lcd.print("");
  lcd.setCursor(4, 1);
  lcd.print("");
  lcd.setCursor(5, 1);
  lcd.print("A");
  lcd.setCursor(6, 1);
  lcd.print("P");
  lcd.setCursor(7, 1);
  lcd.print("E");
  lcd.setCursor(8, 1);
  lcd.print("R");
  lcd.setCursor(9, 1);
  lcd.print("T");
  lcd.setCursor(10, 1);
  lcd.print("A");
  lcd.setCursor(11, 1);
  lcd.print("");
  lcd.setCursor(12, 1);
  lcd.print("");
  lcd.setCursor(13, 1);
  lcd.print("");
  lcd.setCursor(14, 1);
  lcd.print("");
  lcd.setCursor(15, 1);
  lcd.print("");
}

//lcd "PORTA PRINCIPALE CHIUSA"
void lcd_porta_principale_chiusa() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("P");
  lcd.setCursor(1, 0);
  lcd.print("O");
  lcd.setCursor(2, 0);
  lcd.print("R");
  lcd.setCursor(3, 0);
  lcd.print("T");
  lcd.setCursor(4, 0);
  lcd.print("A");
  lcd.setCursor(5, 0);
  lcd.print("");
  lcd.setCursor(6, 0);
  lcd.print("P");
  lcd.setCursor(7, 0);
  lcd.print("R");
  lcd.setCursor(8, 0);
  lcd.print("I");
  lcd.setCursor(9, 0);
  lcd.print("N");
  lcd.setCursor(10, 0);
  lcd.print("C");
  lcd.setCursor(11, 0);
  lcd.print("I");
  lcd.setCursor(12, 0);
  lcd.print("P");
  lcd.setCursor(13, 0);
  lcd.print("A");
  lcd.setCursor(14, 0);
  lcd.print("L");
  lcd.setCursor(15, 0);
  lcd.print("E");

  lcd.setCursor(0, 1);
  lcd.print("");
  lcd.setCursor(1, 1);
  lcd.print("");
  lcd.setCursor(2, 1);
  lcd.print("");
  lcd.setCursor(3, 1);
  lcd.print("");
  lcd.setCursor(4, 1);
  lcd.print("");
  lcd.setCursor(5, 1);
  lcd.print("C");
  lcd.setCursor(6, 1);
  lcd.print("H");
  lcd.setCursor(7, 1);
  lcd.print("I");
  lcd.setCursor(8, 1);
  lcd.print("U");
  lcd.setCursor(9, 1);
  lcd.print("S");
  lcd.setCursor(10, 1);
  lcd.print("A");
  lcd.setCursor(11, 1);
  lcd.print("");
  lcd.setCursor(12, 1);
  lcd.print("");
  lcd.setCursor(13, 1);
  lcd.print("");
  lcd.setCursor(14, 1);
  lcd.print("");
  lcd.setCursor(15, 1);
  lcd.print("");
}

//lcd "ATTENZIONE! ALLARME ATTIVATO"
void lcd_allarme_on() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("");
  lcd.setCursor(1, 0);
  lcd.print("");
  lcd.setCursor(2, 0);
  lcd.print("");
  lcd.setCursor(3, 0);
  lcd.print("A");
  lcd.setCursor(4, 0);
  lcd.print("T");
  lcd.setCursor(5, 0);
  lcd.print("T");
  lcd.setCursor(6, 0);
  lcd.print("E");
  lcd.setCursor(7, 0);
  lcd.print("N");
  lcd.setCursor(8, 0);
  lcd.print("Z");
  lcd.setCursor(9, 0);
  lcd.print("I");
  lcd.setCursor(10, 0);
  lcd.print("O");
  lcd.setCursor(11, 0);
  lcd.print("N");
  lcd.setCursor(12, 0);
  lcd.print("E");
  lcd.setCursor(13, 0);
  lcd.print("!");
  lcd.setCursor(14, 0);
  lcd.print("");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("A");
  lcd.setCursor(1, 1);
  lcd.print("L");
  lcd.setCursor(2, 1);
  lcd.print("L");
  lcd.setCursor(3, 1);
  lcd.print("A");
  lcd.setCursor(4, 1);
  lcd.print("R");
  lcd.setCursor(5, 1);
  lcd.print("M");
  lcd.setCursor(6, 1);
  lcd.print("E");
  lcd.setCursor(7, 1);
  lcd.print("");
  lcd.setCursor(8, 1);
  lcd.print("A");
  lcd.setCursor(9, 1);
  lcd.print("T");
  lcd.setCursor(10, 1);
  lcd.print("T");
  lcd.setCursor(11, 1);
  lcd.print("I");
  lcd.setCursor(12, 1);
  lcd.print("V");
  lcd.setCursor(13, 1);
  lcd.print("A");
  lcd.setCursor(14, 1);
  lcd.print("T");
  lcd.setCursor(15, 1);
  lcd.print("O");
  delay(w);
}

//lcd "ALLARME DISATTIVATO"
void lcd_allarme_off() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("");
  lcd.setCursor(1, 0);
  lcd.print("");
  lcd.setCursor(2, 0);
  lcd.print("");
  lcd.setCursor(3, 0);
  lcd.print("");
  lcd.setCursor(4, 0);
  lcd.print("");
  lcd.setCursor(5, 0);
  lcd.print("A");
  lcd.setCursor(6, 0);
  lcd.print("L");
  lcd.setCursor(7, 0);
  lcd.print("L");
  lcd.setCursor(8, 0);
  lcd.print("A");
  lcd.setCursor(9, 0);
  lcd.print("R");
  lcd.setCursor(10, 0);
  lcd.print("M");
  lcd.setCursor(11, 0);
  lcd.print("E");
  lcd.setCursor(12, 0);
  lcd.print("");
  lcd.setCursor(13, 0);
  lcd.print("");
  lcd.setCursor(14, 0);
  lcd.print("");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("");
  lcd.setCursor(1, 1);
  lcd.print("");
  lcd.setCursor(2, 1);
  lcd.print("");
  lcd.setCursor(3, 1);
  lcd.print("D");
  lcd.setCursor(4, 1);
  lcd.print("I");
  lcd.setCursor(5, 1);
  lcd.print("S");
  lcd.setCursor(6, 1);
  lcd.print("A");
  lcd.setCursor(7, 1);
  lcd.print("T");
  lcd.setCursor(8, 1);
  lcd.print("T");
  lcd.setCursor(9, 1);
  lcd.print("I");
  lcd.setCursor(10, 1);
  lcd.print("V");
  lcd.setCursor(11, 1);
  lcd.print("A");
  lcd.setCursor(12, 1);
  lcd.print("T");
  lcd.setCursor(13, 1);
  lcd.print("O");
  lcd.setCursor(14, 1);
  lcd.print("");
  lcd.setCursor(15, 1);
  lcd.print("");
}

//lcd "TUTTI I SENSORI ATTIVATI"
void lcd_tutti_sensori_on() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T");
  lcd.setCursor(1, 0);
  lcd.print("U");
  lcd.setCursor(2, 0);
  lcd.print("T");
  lcd.setCursor(3, 0);
  lcd.print("T");
  lcd.setCursor(4, 0);
  lcd.print("I");
  lcd.setCursor(5, 0);
  lcd.print("");
  lcd.setCursor(6, 0);
  lcd.print("I");
  lcd.setCursor(7, 0);
  lcd.print("");
  lcd.setCursor(8, 0);
  lcd.print("S");
  lcd.setCursor(9, 0);
  lcd.print("E");
  lcd.setCursor(10, 0);
  lcd.print("N");
  lcd.setCursor(11, 0);
  lcd.print("S");
  lcd.setCursor(12, 0);
  lcd.print("O");
  lcd.setCursor(13, 0);
  lcd.print("R");
  lcd.setCursor(14, 0);
  lcd.print("I");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("");
  lcd.setCursor(1, 1);
  lcd.print("");
  lcd.setCursor(2, 1);
  lcd.print("");
  lcd.setCursor(3, 1);
  lcd.print("");
  lcd.setCursor(4, 1);
  lcd.print("A");
  lcd.setCursor(5, 1);
  lcd.print("T");
  lcd.setCursor(6, 1);
  lcd.print("T");
  lcd.setCursor(7, 1);
  lcd.print("I");
  lcd.setCursor(8, 1);
  lcd.print("V");
  lcd.setCursor(9, 1);
  lcd.print("A");
  lcd.setCursor(10, 1);
  lcd.print("T");
  lcd.setCursor(11, 1);
  lcd.print("I");
  lcd.setCursor(12, 1);
  lcd.print("");
  lcd.setCursor(13, 1);
  lcd.print("");
  lcd.setCursor(14, 1);
  lcd.print("");
  lcd.setCursor(15, 1);
  lcd.print("");
  delay(w_sensor);
}

//lcd "TUTTI I SENSORI DISATTIVATI"
void lcd_tutti_sensori_off() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T");
  lcd.setCursor(1, 0);
  lcd.print("U");
  lcd.setCursor(2, 0);
  lcd.print("T");
  lcd.setCursor(3, 0);
  lcd.print("T");
  lcd.setCursor(4, 0);
  lcd.print("I");
  lcd.setCursor(5, 0);
  lcd.print("");
  lcd.setCursor(6, 0);
  lcd.print("I");
  lcd.setCursor(7, 0);
  lcd.print("");
  lcd.setCursor(8, 0);
  lcd.print("S");
  lcd.setCursor(9, 0);
  lcd.print("E");
  lcd.setCursor(10, 0);
  lcd.print("N");
  lcd.setCursor(11, 0);
  lcd.print("S");
  lcd.setCursor(12, 0);
  lcd.print("O");
  lcd.setCursor(13, 0);
  lcd.print("R");
  lcd.setCursor(14, 0);
  lcd.print("I");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("");
  lcd.setCursor(1, 1);
  lcd.print("");
  lcd.setCursor(2, 1);
  lcd.print("");
  lcd.setCursor(3, 1);
  lcd.print("D");
  lcd.setCursor(4, 1);
  lcd.print("I");
  lcd.setCursor(5, 1);
  lcd.print("S");
  lcd.setCursor(6, 1);
  lcd.print("A");
  lcd.setCursor(7, 1);
  lcd.print("T");
  lcd.setCursor(8, 1);
  lcd.print("T");
  lcd.setCursor(9, 1);
  lcd.print("I");
  lcd.setCursor(10, 1);
  lcd.print("V");
  lcd.setCursor(11, 1);
  lcd.print("A");
  lcd.setCursor(12, 1);
  lcd.print("T");
  lcd.setCursor(13, 1);
  lcd.print("I");
  lcd.setCursor(14, 1);
  lcd.print("");
  lcd.setCursor(15, 1);
  lcd.print("");
  delay(w_sensor);
}

//lcd "rilev. GAS ATTIVATO"
void lcd_gas_on() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("");
  lcd.setCursor(1, 0);
  lcd.print("");
  lcd.setCursor(2, 0);
  lcd.print("");
  lcd.setCursor(3, 0);
  lcd.print("r");
  lcd.setCursor(4, 0);
  lcd.print("i");
  lcd.setCursor(5, 0);
  lcd.print("l");
  lcd.setCursor(6, 0);
  lcd.print("e");
  lcd.setCursor(7, 0);
  lcd.print("v");
  lcd.setCursor(8, 0);
  lcd.print(".");
  lcd.setCursor(9, 0);
  lcd.print("");
  lcd.setCursor(10, 0);
  lcd.print("G");
  lcd.setCursor(11, 0);
  lcd.print("A");
  lcd.setCursor(12, 0);
  lcd.print("S");
  lcd.setCursor(13, 0);
  lcd.print("");
  lcd.setCursor(14, 0);
  lcd.print("");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("");
  lcd.setCursor(1, 1);
  lcd.print("");
  lcd.setCursor(2, 1);
  lcd.print("");
  lcd.setCursor(3, 1);
  lcd.print("");
  lcd.setCursor(4, 1);
  lcd.print("A");
  lcd.setCursor(5, 1);
  lcd.print("T");
  lcd.setCursor(6, 1);
  lcd.print("T");
  lcd.setCursor(7, 1);
  lcd.print("I");
  lcd.setCursor(8, 1);
  lcd.print("V");
  lcd.setCursor(9, 1);
  lcd.print("A");
  lcd.setCursor(10, 1);
  lcd.print("T");
  lcd.setCursor(11, 1);
  lcd.print("O");
  lcd.setCursor(12, 1);
  lcd.print("");
  lcd.setCursor(13, 1);
  lcd.print("");
  lcd.setCursor(14, 1);
  lcd.print("");
  lcd.setCursor(15, 1);
  lcd.print("");
  delay(w_sensor);
}

//lcd "rilev. GAS DISATTIVATO"
void lcd_gas_off() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("");
  lcd.setCursor(1, 0);
  lcd.print("");
  lcd.setCursor(2, 0);
  lcd.print("");
  lcd.setCursor(3, 0);
  lcd.print("r");
  lcd.setCursor(4, 0);
  lcd.print("i");
  lcd.setCursor(5, 0);
  lcd.print("l");
  lcd.setCursor(6, 0);
  lcd.print("e");
  lcd.setCursor(7, 0);
  lcd.print("v");
  lcd.setCursor(8, 0);
  lcd.print(".");
  lcd.setCursor(9, 0);
  lcd.print("");
  lcd.setCursor(10, 0);
  lcd.print("G");
  lcd.setCursor(11, 0);
  lcd.print("A");
  lcd.setCursor(12, 0);
  lcd.print("S");
  lcd.setCursor(13, 0);
  lcd.print("");
  lcd.setCursor(14, 0);
  lcd.print("");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("");
  lcd.setCursor(1, 1);
  lcd.print("");
  lcd.setCursor(2, 1);
  lcd.print("");
  lcd.setCursor(3, 1);
  lcd.print("D");
  lcd.setCursor(4, 1);
  lcd.print("I");
  lcd.setCursor(5, 1);
  lcd.print("S");
  lcd.setCursor(6, 1);
  lcd.print("A");
  lcd.setCursor(7, 1);
  lcd.print("T");
  lcd.setCursor(8, 1);
  lcd.print("T");
  lcd.setCursor(9, 1);
  lcd.print("I");
  lcd.setCursor(10, 1);
  lcd.print("V");
  lcd.setCursor(11, 1);
  lcd.print("A");
  lcd.setCursor(12, 1);
  lcd.print("T");
  lcd.setCursor(13, 1);
  lcd.print("O");
  lcd.setCursor(14, 1);
  lcd.print("");
  lcd.setCursor(15, 1);
  lcd.print("");
  delay(w_sensor);
}

//lcd "rilev. FIAMMA ATTIVATO"
void lcd_fiamma_on() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("");
  lcd.setCursor(1, 0);
  lcd.print("");
  lcd.setCursor(2, 0);
  lcd.print("r");
  lcd.setCursor(3, 0);
  lcd.print("i");
  lcd.setCursor(4, 0);
  lcd.print("l");
  lcd.setCursor(5, 0);
  lcd.print("e");
  lcd.setCursor(6, 0);
  lcd.print("v");
  lcd.setCursor(7, 0);
  lcd.print(".");
  lcd.setCursor(8, 0);
  lcd.print("");
  lcd.setCursor(9, 0);
  lcd.print("F");
  lcd.setCursor(10, 0);
  lcd.print("I");
  lcd.setCursor(11, 0);
  lcd.print("A");
  lcd.setCursor(12, 0);
  lcd.print("M");
  lcd.setCursor(13, 0);
  lcd.print("M");
  lcd.setCursor(14, 0);
  lcd.print("A");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("");
  lcd.setCursor(1, 1);
  lcd.print("");
  lcd.setCursor(2, 1);
  lcd.print("");
  lcd.setCursor(3, 1);
  lcd.print("");
  lcd.setCursor(4, 1);
  lcd.print("A");
  lcd.setCursor(5, 1);
  lcd.print("T");
  lcd.setCursor(6, 1);
  lcd.print("T");
  lcd.setCursor(7, 1);
  lcd.print("I");
  lcd.setCursor(8, 1);
  lcd.print("V");
  lcd.setCursor(9, 1);
  lcd.print("A");
  lcd.setCursor(10, 1);
  lcd.print("T");
  lcd.setCursor(11, 1);
  lcd.print("O");
  lcd.setCursor(12, 1);
  lcd.print("");
  lcd.setCursor(13, 1);
  lcd.print("");
  lcd.setCursor(14, 1);
  lcd.print("");
  lcd.setCursor(15, 1);
  lcd.print("");
  delay(w_sensor);
}

//lcd "rilev. FIAMMA DISATTIVATO"
void lcd_fiamma_off() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("");
  lcd.setCursor(1, 0);
  lcd.print("");
  lcd.setCursor(2, 0);
  lcd.print("r");
  lcd.setCursor(3, 0);
  lcd.print("i");
  lcd.setCursor(4, 0);
  lcd.print("l");
  lcd.setCursor(5, 0);
  lcd.print("e");
  lcd.setCursor(6, 0);
  lcd.print("v");
  lcd.setCursor(7, 0);
  lcd.print(".");
  lcd.setCursor(8, 0);
  lcd.print("");
  lcd.setCursor(9, 0);
  lcd.print("F");
  lcd.setCursor(10, 0);
  lcd.print("I");
  lcd.setCursor(11, 0);
  lcd.print("A");
  lcd.setCursor(12, 0);
  lcd.print("M");
  lcd.setCursor(13, 0);
  lcd.print("M");
  lcd.setCursor(14, 0);
  lcd.print("A");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("");
  lcd.setCursor(1, 1);
  lcd.print("");
  lcd.setCursor(2, 1);
  lcd.print("");
  lcd.setCursor(3, 1);
  lcd.print("D");
  lcd.setCursor(4, 1);
  lcd.print("I");
  lcd.setCursor(5, 1);
  lcd.print("S");
  lcd.setCursor(6, 1);
  lcd.print("A");
  lcd.setCursor(7, 1);
  lcd.print("T");
  lcd.setCursor(8, 1);
  lcd.print("T");
  lcd.setCursor(9, 1);
  lcd.print("I");
  lcd.setCursor(10, 1);
  lcd.print("V");
  lcd.setCursor(11, 1);
  lcd.print("A");
  lcd.setCursor(12, 1);
  lcd.print("T");
  lcd.setCursor(13, 1);
  lcd.print("O");
  lcd.setCursor(14, 1);
  lcd.print("");
  lcd.setCursor(15, 1);
  lcd.print("");
  delay(w_sensor);
}

//lcd "ril. TEMPERATURA ATTIVATO"
void lcd_dht11_on() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("r");
  lcd.setCursor(1, 0);
  lcd.print("i");
  lcd.setCursor(2, 0);
  lcd.print("l");
  lcd.setCursor(3, 0);
  lcd.print(".");
  lcd.setCursor(4, 0);
  lcd.print("");
  lcd.setCursor(5, 0);
  lcd.print("T");
  lcd.setCursor(6, 0);
  lcd.print("E");
  lcd.setCursor(7, 0);
  lcd.print("M");
  lcd.setCursor(8, 0);
  lcd.print("P");
  lcd.setCursor(9, 0);
  lcd.print("E");
  lcd.setCursor(10, 0);
  lcd.print("R");
  lcd.setCursor(11, 0);
  lcd.print("A");
  lcd.setCursor(12, 0);
  lcd.print("T");
  lcd.setCursor(13, 0);
  lcd.print("U");
  lcd.setCursor(14, 0);
  lcd.print("R");
  lcd.setCursor(15, 0);
  lcd.print("A");

  lcd.setCursor(0, 1);
  lcd.print("");
  lcd.setCursor(1, 1);
  lcd.print("");
  lcd.setCursor(2, 1);
  lcd.print("");
  lcd.setCursor(3, 1);
  lcd.print("");
  lcd.setCursor(4, 1);
  lcd.print("A");
  lcd.setCursor(5, 1);
  lcd.print("T");
  lcd.setCursor(6, 1);
  lcd.print("T");
  lcd.setCursor(7, 1);
  lcd.print("I");
  lcd.setCursor(8, 1);
  lcd.print("V");
  lcd.setCursor(9, 1);
  lcd.print("A");
  lcd.setCursor(10, 1);
  lcd.print("T");
  lcd.setCursor(11, 1);
  lcd.print("O");
  lcd.setCursor(12, 1);
  lcd.print("");
  lcd.setCursor(13, 1);
  lcd.print("");
  lcd.setCursor(14, 1);
  lcd.print("");
  lcd.setCursor(15, 1);
  lcd.print("");
  delay(w_sensor);
}


//lcd "ril. TEMPERATURA DISATTIVATO"
void lcd_dht11_off() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("r");
  lcd.setCursor(1, 0);
  lcd.print("i");
  lcd.setCursor(2, 0);
  lcd.print("l");
  lcd.setCursor(3, 0);
  lcd.print(".");
  lcd.setCursor(4, 0);
  lcd.print("");
  lcd.setCursor(5, 0);
  lcd.print("T");
  lcd.setCursor(6, 0);
  lcd.print("E");
  lcd.setCursor(7, 0);
  lcd.print("M");
  lcd.setCursor(8, 0);
  lcd.print("P");
  lcd.setCursor(9, 0);
  lcd.print("E");
  lcd.setCursor(10, 0);
  lcd.print("R");
  lcd.setCursor(11, 0);
  lcd.print("A");
  lcd.setCursor(12, 0);
  lcd.print("T");
  lcd.setCursor(13, 0);
  lcd.print("U");
  lcd.setCursor(14, 0);
  lcd.print("R");
  lcd.setCursor(15, 0);
  lcd.print("A");

  lcd.setCursor(0, 1);
  lcd.print("");
  lcd.setCursor(1, 1);
  lcd.print("");
  lcd.setCursor(2, 1);
  lcd.print("");
  lcd.setCursor(3, 1);
  lcd.print("D");
  lcd.setCursor(4, 1);
  lcd.print("I");
  lcd.setCursor(5, 1);
  lcd.print("S");
  lcd.setCursor(6, 1);
  lcd.print("A");
  lcd.setCursor(7, 1);
  lcd.print("T");
  lcd.setCursor(8, 1);
  lcd.print("T");
  lcd.setCursor(9, 1);
  lcd.print("I");
  lcd.setCursor(10, 1);
  lcd.print("V");
  lcd.setCursor(11, 1);
  lcd.print("A");
  lcd.setCursor(12, 1);
  lcd.print("T");
  lcd.setCursor(13, 1);
  lcd.print("O");
  lcd.setCursor(14, 1);
  lcd.print("");
  lcd.setCursor(15, 1);
  lcd.print("");
  delay(w_sensor);
}

//lcd "ventola attivata"
void lcd_ventola_on() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("");
  lcd.setCursor(1, 0);
  lcd.print("");
  lcd.setCursor(2, 0);
  lcd.print("");
  lcd.setCursor(3, 0);
  lcd.print("");
  lcd.setCursor(4, 0);
  lcd.print("V");
  lcd.setCursor(5, 0);
  lcd.print("E");
  lcd.setCursor(6, 0);
  lcd.print("N");
  lcd.setCursor(7, 0);
  lcd.print("T");
  lcd.setCursor(8, 0);
  lcd.print("O");
  lcd.setCursor(9, 0);
  lcd.print("L");
  lcd.setCursor(10, 0);
  lcd.print("I");
  lcd.setCursor(11, 0);
  lcd.print("N");
  lcd.setCursor(12, 0);
  lcd.print("A");
  lcd.setCursor(13, 0);
  lcd.print("");
  lcd.setCursor(14, 0);
  lcd.print("");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("");
  lcd.setCursor(1, 1);
  lcd.print("");
  lcd.setCursor(2, 1);
  lcd.print("");
  lcd.setCursor(3, 1);
  lcd.print("");
  lcd.setCursor(4, 1);
  lcd.print("A");
  lcd.setCursor(5, 1);
  lcd.print("T");
  lcd.setCursor(6, 1);
  lcd.print("T");
  lcd.setCursor(7, 1);
  lcd.print("I");
  lcd.setCursor(8, 1);
  lcd.print("V");
  lcd.setCursor(9, 1);
  lcd.print("A");
  lcd.setCursor(10, 1);
  lcd.print("T");
  lcd.setCursor(11, 1);
  lcd.print("A");
  lcd.setCursor(12, 1);
  lcd.print("");
  lcd.setCursor(13, 1);
  lcd.print("");
  lcd.setCursor(14, 1);
  lcd.print("");
  lcd.setCursor(15, 1);
  lcd.print("");
  delay(w);
}

//lcd "ventola disattivata"
void lcd_ventola_off() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("");
  lcd.setCursor(1, 0);
  lcd.print("");
  lcd.setCursor(2, 0);
  lcd.print("");
  lcd.setCursor(3, 0);
  lcd.print("");
  lcd.setCursor(4, 0);
  lcd.print("V");
  lcd.setCursor(5, 0);
  lcd.print("E");
  lcd.setCursor(6, 0);
  lcd.print("N");
  lcd.setCursor(7, 0);
  lcd.print("T");
  lcd.setCursor(8, 0);
  lcd.print("O");
  lcd.setCursor(9, 0);
  lcd.print("L");
  lcd.setCursor(10, 0);
  lcd.print("I");
  lcd.setCursor(11, 0);
  lcd.print("N");
  lcd.setCursor(12, 0);
  lcd.print("A");
  lcd.setCursor(13, 0);
  lcd.print("");
  lcd.setCursor(14, 0);
  lcd.print("");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("");
  lcd.setCursor(1, 1);
  lcd.print("");
  lcd.setCursor(2, 1);
  lcd.print("");
  lcd.setCursor(3, 1);
  lcd.print("D");
  lcd.setCursor(4, 1);
  lcd.print("I");
  lcd.setCursor(5, 1);
  lcd.print("S");
  lcd.setCursor(6, 1);
  lcd.print("A");
  lcd.setCursor(7, 1);
  lcd.print("T");
  lcd.setCursor(8, 1);
  lcd.print("T");
  lcd.setCursor(9, 1);
  lcd.print("I");
  lcd.setCursor(10, 1);
  lcd.print("V");
  lcd.setCursor(11, 1);
  lcd.print("A");
  lcd.setCursor(12, 1);
  lcd.print("T");
  lcd.setCursor(13, 1);
  lcd.print("A");
  lcd.setCursor(14, 1);
  lcd.print("");
  lcd.setCursor(15, 1);
  lcd.print("");
  delay(w);
}

//lcd "rilev. DISTANZA ATTIVATO"
void lcd_distanza_on() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("r");
  lcd.setCursor(1, 0);
  lcd.print("i");
  lcd.setCursor(2, 0);
  lcd.print("l");
  lcd.setCursor(3, 0);
  lcd.print("e");
  lcd.setCursor(4, 0);
  lcd.print("v");
  lcd.setCursor(5, 0);
  lcd.print(".");
  lcd.setCursor(6, 0);
  lcd.print("");
  lcd.setCursor(7, 0);
  lcd.print("D");
  lcd.setCursor(8, 0);
  lcd.print("I");
  lcd.setCursor(9, 0);
  lcd.print("S");
  lcd.setCursor(10, 0);
  lcd.print("T");
  lcd.setCursor(11, 0);
  lcd.print("A");
  lcd.setCursor(12, 0);
  lcd.print("N");
  lcd.setCursor(13, 0);
  lcd.print("Z");
  lcd.setCursor(14, 0);
  lcd.print("A");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("");
  lcd.setCursor(1, 1);
  lcd.print("");
  lcd.setCursor(2, 1);
  lcd.print("");
  lcd.setCursor(3, 1);
  lcd.print("");
  lcd.setCursor(4, 1);
  lcd.print("A");
  lcd.setCursor(5, 1);
  lcd.print("T");
  lcd.setCursor(6, 1);
  lcd.print("T");
  lcd.setCursor(7, 1);
  lcd.print("I");
  lcd.setCursor(8, 1);
  lcd.print("V");
  lcd.setCursor(9, 1);
  lcd.print("A");
  lcd.setCursor(10, 1);
  lcd.print("T");
  lcd.setCursor(11, 1);
  lcd.print("O");
  lcd.setCursor(12, 1);
  lcd.print("");
  lcd.setCursor(13, 1);
  lcd.print("");
  lcd.setCursor(14, 1);
  lcd.print("");
  lcd.setCursor(15, 1);
  lcd.print("");
  delay(w_sensor);
}

//lcd "rilev. DISTANZA DISATTIVATO"
void lcd_distanza_off() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("r");
  lcd.setCursor(1, 0);
  lcd.print("i");
  lcd.setCursor(2, 0);
  lcd.print("l");
  lcd.setCursor(3, 0);
  lcd.print("e");
  lcd.setCursor(4, 0);
  lcd.print("v");
  lcd.setCursor(5, 0);
  lcd.print(".");
  lcd.setCursor(6, 0);
  lcd.print("");
  lcd.setCursor(7, 0);
  lcd.print("D");
  lcd.setCursor(8, 0);
  lcd.print("I");
  lcd.setCursor(9, 0);
  lcd.print("S");
  lcd.setCursor(10, 0);
  lcd.print("T");
  lcd.setCursor(11, 0);
  lcd.print("A");
  lcd.setCursor(12, 0);
  lcd.print("N");
  lcd.setCursor(13, 0);
  lcd.print("Z");
  lcd.setCursor(14, 0);
  lcd.print("A");
  lcd.setCursor(15, 0);
  lcd.print("");

  lcd.setCursor(0, 1);
  lcd.print("");
  lcd.setCursor(1, 1);
  lcd.print("");
  lcd.setCursor(2, 1);
  lcd.print("");
  lcd.setCursor(3, 1);
  lcd.print("D");
  lcd.setCursor(4, 1);
  lcd.print("I");
  lcd.setCursor(5, 1);
  lcd.print("S");
  lcd.setCursor(6, 1);
  lcd.print("A");
  lcd.setCursor(7, 1);
  lcd.print("T");
  lcd.setCursor(8, 1);
  lcd.print("T");
  lcd.setCursor(9, 1);
  lcd.print("I");
  lcd.setCursor(10, 1);
  lcd.print("V");
  lcd.setCursor(11, 1);
  lcd.print("A");
  lcd.setCursor(12, 1);
  lcd.print("T");
  lcd.setCursor(13, 1);
  lcd.print("O");
  lcd.setCursor(14, 1);
  lcd.print("");
  lcd.setCursor(15, 1);
  lcd.print("");
  delay(w_sensor);
}

//lcd "rilev. MOVIMENTO ATTIVATO"
void lcd_movimento_on() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("r");
  lcd.setCursor(1, 0);
  lcd.print("i");
  lcd.setCursor(2, 0);
  lcd.print("l");
  lcd.setCursor(3, 0);
  lcd.print("e");
  lcd.setCursor(4, 0);
  lcd.print("v");
  lcd.setCursor(5, 0);
  lcd.print(".");
  lcd.setCursor(6, 0);
  lcd.print(" ");
  lcd.setCursor(7, 0);
  lcd.print("m");
  lcd.setCursor(8, 0);
  lcd.print("o");
  lcd.setCursor(9, 0);
  lcd.print("v");
  lcd.setCursor(10, 0);
  lcd.print("i");
  lcd.setCursor(11, 0);
  lcd.print("m");
  lcd.setCursor(12, 0);
  lcd.print("e");
  lcd.setCursor(13, 0);
  lcd.print("n");
  lcd.setCursor(14, 0);
  lcd.print("t");
  lcd.setCursor(15, 0);
  lcd.print("o");

  lcd.setCursor(0, 1);
  lcd.print(" ");
  lcd.setCursor(1, 1);
  lcd.print(" ");
  lcd.setCursor(2, 1);
  lcd.print(" ");
  lcd.setCursor(3, 1);
  lcd.print(" ");
  lcd.setCursor(4, 1);
  lcd.print("A");
  lcd.setCursor(5, 1);
  lcd.print("T");
  lcd.setCursor(6, 1);
  lcd.print("T");
  lcd.setCursor(7, 1);
  lcd.print("I");
  lcd.setCursor(8, 1);
  lcd.print("V");
  lcd.setCursor(9, 1);
  lcd.print("A");
  lcd.setCursor(10, 1);
  lcd.print("T");
  lcd.setCursor(11, 1);
  lcd.print("O");
  lcd.setCursor(12, 1);
  lcd.print(" ");
  lcd.setCursor(13, 1);
  lcd.print(" ");
  lcd.setCursor(14, 1);
  lcd.print(" ");
  lcd.setCursor(15, 1);
  lcd.print(" ");
  delay(w_sensor_movimento);
}


//lcd "rilev. MOVIMENTO DISATTIVATO"
void lcd_movimento_off() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("r");
  lcd.setCursor(1, 0);
  lcd.print("i");
  lcd.setCursor(2, 0);
  lcd.print("l");
  lcd.setCursor(3, 0);
  lcd.print("e");
  lcd.setCursor(4, 0);
  lcd.print("v");
  lcd.setCursor(5, 0);
  lcd.print(".");
  lcd.setCursor(6, 0);
  lcd.print("");
  lcd.setCursor(7, 0);
  lcd.print("m");
  lcd.setCursor(8, 0);
  lcd.print("o");
  lcd.setCursor(9, 0);
  lcd.print("v");
  lcd.setCursor(10, 0);
  lcd.print("i");
  lcd.setCursor(11, 0);
  lcd.print("m");
  lcd.setCursor(12, 0);
  lcd.print("e");
  lcd.setCursor(13, 0);
  lcd.print("n");
  lcd.setCursor(14, 0);
  lcd.print("t");
  lcd.setCursor(15, 0);
  lcd.print("o");

  lcd.setCursor(0, 1);
  lcd.print("");
  lcd.setCursor(1, 1);
  lcd.print("");
  lcd.setCursor(2, 1);
  lcd.print("");
  lcd.setCursor(3, 1);
  lcd.print("D");
  lcd.setCursor(4, 1);
  lcd.print("I");
  lcd.setCursor(5, 1);
  lcd.print("S");
  lcd.setCursor(6, 1);
  lcd.print("A");
  lcd.setCursor(7, 1);
  lcd.print("T");
  lcd.setCursor(8, 1);
  lcd.print("T");
  lcd.setCursor(9, 1);
  lcd.print("I");
  lcd.setCursor(10, 1);
  lcd.print("V");
  lcd.setCursor(11, 1);
  lcd.print("A");
  lcd.setCursor(12, 1);
  lcd.print("T");
  lcd.setCursor(13, 1);
  lcd.print("O");
  lcd.setCursor(14, 1);
  lcd.print("");
  lcd.setCursor(15, 1);
  lcd.print("");
  delay(w_sensor);
}

void logout() {
  if (cmd == '$') {
    //avviso sonoro
    digitalWrite(buzzer1, HIGH);
    digitalWrite(buzzer2, HIGH);
    delay(tempo_logout_buzzer_segnalazione_sonora);
    digitalWrite(buzzer1, LOW);
    digitalWrite(buzzer2, LOW);
    delay(tempo_logout_buzzer_segnalazione_sonora);
    digitalWrite(buzzer1, HIGH);
    digitalWrite(buzzer2, HIGH);
    delay(tempo_logout_buzzer_segnalazione_sonora);
    digitalWrite(buzzer1, LOW);
    digitalWrite(buzzer2, LOW);
    delay(tempo_logout_buzzer_segnalazione_sonora);
    digitalWrite(buzzer1, HIGH);
    digitalWrite(buzzer2, HIGH);
    delay(tempo_logout_buzzer_segnalazione_sonora);
    digitalWrite(buzzer1, LOW);
    digitalWrite(buzzer2, LOW);
    delay(tempo_logout_buzzer_segnalazione_sonora);
    delay(30);

    //lcd "ciao Andrea!"
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(" ");
    lcd.setCursor(1, 0);
    lcd.print(" ");
    lcd.setCursor(2, 0);
    lcd.print("C");
    lcd.setCursor(3, 0);
    lcd.print("i");
    lcd.setCursor(4, 0);
    lcd.print("a");
    lcd.setCursor(5, 0);
    lcd.print("o");
    lcd.setCursor(6, 0);
    lcd.print(" ");
    lcd.setCursor(7, 0);
    lcd.print("A");
    lcd.setCursor(8, 0);
    lcd.print("n");
    lcd.setCursor(9, 0);
    lcd.print("d");
    lcd.setCursor(10, 0);
    lcd.print("r");
    lcd.setCursor(11, 0);
    lcd.print("e");
    lcd.setCursor(12, 0);
    lcd.print("a");
    lcd.setCursor(13, 0);
    lcd.print("!");
    lcd.setCursor(14, 0);
    lcd.print(" ");
    lcd.setCursor(15, 0);
    lcd.print(" ");
    delay(250);

    //lcd "SPEGNIMENTO IN CORSO"
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("S");
    lcd.setCursor(1, 0);
    lcd.print("P");
    lcd.setCursor(2, 0);
    lcd.print("E");
    lcd.setCursor(3, 0);
    lcd.print("G");
    lcd.setCursor(4, 0);
    lcd.print("N");
    lcd.setCursor(5, 0);
    lcd.print("I");
    lcd.setCursor(6, 0);
    lcd.print("M");
    lcd.setCursor(7, 0);
    lcd.print("E");
    lcd.setCursor(8, 0);
    lcd.print("N");
    lcd.setCursor(9, 0);
    lcd.print("T");
    lcd.setCursor(10, 0);
    lcd.print("O");
    lcd.setCursor(11, 0);
    lcd.print("");
    lcd.setCursor(12, 0);
    lcd.print("L");
    lcd.setCursor(13, 0);
    lcd.print("U");
    lcd.setCursor(14, 0);
    lcd.print("C");
    lcd.setCursor(15, 0);
    lcd.print("I");

    lcd.setCursor(0, 1);
    lcd.print("");
    lcd.setCursor(1, 1);
    lcd.print("");
    lcd.setCursor(2, 1);
    lcd.print("");
    lcd.setCursor(3, 1);
    lcd.print("");
    lcd.setCursor(4, 1);
    lcd.print("I");
    lcd.setCursor(5, 1);
    lcd.print("N");
    lcd.setCursor(6, 1);
    lcd.print("");
    lcd.setCursor(7, 1);
    lcd.print("C");
    lcd.setCursor(8, 1);
    lcd.print("O");
    lcd.setCursor(9, 1);
    lcd.print("R");
    lcd.setCursor(10, 1);
    lcd.print("S");
    lcd.setCursor(11, 1);
    lcd.print("O");
    lcd.setCursor(12, 1);
    lcd.print("");
    lcd.setCursor(13, 1);
    lcd.print("");
    lcd.setCursor(14, 1);
    lcd.print("");
    lcd.setCursor(15, 1);
    lcd.print("");

    //sensori disabilitati
    digitalWrite(buzzer1, LOW);
    digitalWrite(buzzer2, LOW);

    digitalWrite(led_verde_sensore_gas, LOW);
    digitalWrite(led_verde_sensore_movimento, LOW);
    digitalWrite(led_verde_sensore_fiamma, LOW);
    digitalWrite(led_verde_sensore_dht11, LOW);
    digitalWrite(led_verde_sensore_distanza, LOW);
    digitalWrite(led_verde_sensore_luminosita, LOW);
    digitalWrite(ventola, LOW);

    digitalWrite(led_rosso_sensore_gas, HIGH);
    digitalWrite(led_rosso_sensore_movimento, HIGH);
    digitalWrite(led_rosso_sensore_fiamma, HIGH);
    digitalWrite(led_rosso_sensore_dht11, HIGH);
    digitalWrite(led_rosso_sensore_distanza, HIGH);
    digitalWrite(led_rosso_sensore_luminosita, HIGH);

    //illuminazione giardino
    digitalWrite(led_giardino, HIGH);
    delay(tempo_logout_spegnimento);

    //corridoio
    digitalWrite(rosso_rgb1, LOW);
    digitalWrite(verde_rgb1, LOW);
    digitalWrite(blu_rgb1, LOW);

    digitalWrite(rosso_rgb2, LOW);
    digitalWrite(verde_rgb2, LOW);
    digitalWrite(blu_rgb2, LOW);

    digitalWrite(rosso_rgb3, LOW);
    digitalWrite(verde_rgb3, LOW);
    digitalWrite(blu_rgb3, LOW);
    delay(tempo_logout_spegnimento);

    digitalWrite(bianchi_pin_piccoli, LOW);
    digitalWrite(rossi_pin_piccoli, LOW);
    digitalWrite(verdi_pin_piccoli, LOW);
    digitalWrite(blu_pin_piccoli, LOW);
    digitalWrite(gialli_pin_piccoli, LOW);

    digitalWrite(bianchi_pin_grandi, LOW);
    digitalWrite(rossi_pin_grandi, LOW);
    digitalWrite(verdi_pin_grandi, LOW);
    digitalWrite(blu_pin_grandi, LOW);
    digitalWrite(gialli_pin_grandi, LOW);
    delay(tempo_logout_spegnimento);

    //led camera 1
    digitalWrite(led_grandi_camera1, LOW);
    digitalWrite(led_piccoli_1_camera1, LOW);
    digitalWrite(led_piccoli_2_camera1, LOW);
    delay(tempo_logout_spegnimento);

    //led camera 2
    digitalWrite(led_grandi_camera2, LOW);
    digitalWrite(led_piccoli_1_camera2, LOW);
    digitalWrite(led_piccoli_2_camera2, LOW);
    delay(tempo_logout_spegnimento);

    //led camera 3
    digitalWrite(led_grandi_camera3, LOW);
    digitalWrite(led_piccoli_1_camera3, LOW);
    digitalWrite(led_piccoli_2_camera3, LOW);
    delay(tempo_logout_spegnimento);

    //led camera 4
    digitalWrite(led_grandi_camera4, LOW);
    digitalWrite(led_piccoli_1_camera4, LOW);
    digitalWrite(led_piccoli_2_camera4, LOW);
    delay(tempo_logout_spegnimento);

    //ventola
    digitalWrite(ventola, LOW);

    //fontanella
    digitalWrite(pompa_acqua, HIGH); //il relay funziona al contrario HIGH --> LOW
    digitalWrite(led_pompa_acqua, LOW);
    delay(tempo_logout_spegnimento);
    lcd_testo_iniziale();

    cmd = ' ';
  }
}
