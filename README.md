# Connessione Bluetooth Low Energy (BLE) con Finder Opta

Questo repository contiene uno sketch Arduino progettato per facilitare la comunicazione **Bluetooth Low Energy (BLE)** con un dispositivo **Finder Opta**. Permette la lettura degli stati degli **input digitali** e **analogici**, oltre all'invio di una **stringa di testo** (in questo caso un indirizzo IP), il tutto gestibile tramite un'applicazione mobile dedicata oppure con un applicazione generica (es. nRF Connect).

## Funzionalità Principali

Lo sketch offre una serie di funzionalità chiave per il monitoraggio e la comunicazione:

* **Monitoraggio Input Digitali**:
    * Legge lo stato degli input digitali (es. `A0` - `A6`).
    * Trasmette lo stato via BLE come **ON** (`0x01`) o **OFF** (`0x00`).
    * Le notifiche BLE vengono inviate solo quando lo stato di un input digitale cambia.

* **Lettura Input Analogici**:
    * Acquisisce i valori dagli input analogici configurati (es. `A7`).
    * Converte i valori raw in **tensione (Volt)**.
    * Le notifiche BLE vengono inviate solo se la variazione di tensione supera una soglia predefinita.

* **Trasmissione Stringa (Indirizzo IP)**:
    * Invia una stringa (nello sketch un indirizzo IP) tramite una caratteristica BLE dedicata.
    * Questa sezione è facilmente estendibile per supportare l'invio di alcuni dati (indirizzo IP, stato connessioni, ecc).

* **Notifiche BLE Integrate**:
    * Utilizza le capacità di notifica BLE (`BLENotify`) per avvisare in tempo reale i dispositivi connessi (come uno smartphone) ogni volta che lo stato degli input digitali, analogici o l'indirizzo IP subiscono variazioni.

* **Modalità Debug**:
    * Includendo `bool DEBUG = true;` all'inizio dello sketch, è possibile abilitare i messaggi di debug sulla console seriale. Questi messaggi forniscono dettagli sullo stato della connessione BLE e sui valori letti dagli input.


## UUID dei Servizi e delle Caratteristiche

Per stabilire una comunicazione BLE efficace e senza interruzioni tra il Finder Opta e la tua applicazione mobile, è fondamentale che gli **UUID** (Universally Unique Identifiers) del servizio BLE e delle sue caratteristiche siano **identici** in entrambi i lati della connessione.

Gli UUID possono essere generati facilmente online (ad esempio su [uuidgenerator.net](https://www.uuidgenerator.net/)).

## Configurazione Pin

Lo sketch è configurato per utilizzare i seguenti pin del Finder Opta:

* **Input Digitali**: `A0`, `A1`, `A2`, `A3`, `A4`, `A5`, `A6`
* **Input Analogici**: `A7`

I LED integrati nel Finder Opta (`LED_D0`, `LED_D1`) vengono utilizzati come indicatori visivi dello stato della connessione BLE:

* 🟢 `LED_D0` acceso: Il modulo BLE è avviato ed è in attesa di connessioni.
* 🟢 `LED_D1` acceso: Un dispositivo BLE è attualmente connesso al Finder Opta.

## Requisiti

Per compilare e caricare questo sketch, avrai bisogno di:

* Un dispositivo **Finder Opta** con modulo BT (OPTA ADVANCED 8A.04.9.024.8320).
* La libreria **ArduinoBLE** installata nel tuo IDE Arduino.
* L'ambiente di sviluppo **Arduino IDE**.


## Video Demo

[![Android Demo](https://img.youtube.com/vi/2NFx34HhVEc/0.jpg)](https://youtube.com/shorts/2NFx34HhVEc?si=G3lKn_GWNbW9LxKz "Android Demo")
