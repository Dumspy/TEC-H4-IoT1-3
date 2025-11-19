# arbejds-portfolio 

## Produktkrav
- Anordning har 4 knapper, med tilhørende smileys.
- Der er respons på LED ved hver knap.
- På knapperne implementeres [*debounce*](#debounce) funktionalitet.
- Efter tryk på en knap, er anordningen låst i 7 sekunder, før ny feedback modtages.
    - LED ud for den trykkede knap lyser i alle 7 sekunder
- Anordningen tilsluttes internet, via [wi-fi](#wi-fi) access-punkt i undervisningslokalet
- Knaptryk-data (værdi og timestamp), overføres via [MQTT](#mqtt).
    - Der bruges brugernavn og password, og hele kommunikationen er sikret ved brug af [TLS-kryptering](#tls-kryptering)
- Anordningen er designet strømbesparende, ved brug af [DeepSleep](#deepsleep) og batteridrift
    - Anordningen skal spare strøm ved at skifte til deep-sleep, når den er ubeskæftiget i et stykke tid.
    - Eksperimenter og analyser skal vise de bedste tidsgrænser for hvor længe anordningen er ubeskæftiget, men aktiv.
    - Samt, hvordan skal opvågnen fungere, så anordningen opdager hvad der sker, og falder i søvn igen.
    - En god analyse og overvejelser danner grundlag for hvor længe anordningen sover, inden den vågner rutinemæssigt

### PoC
Initiale Proof of concept eksisterer stadig [her](https://github.com/Dumspy/TEC-H4-IoT1-3/tree/poc), denne poc er blevet brugt som base til det færdige project.

### Debounce
**Hvordan løser vi dette?**
    - debounce burde ikke opstå/være et problem, da vi kun håndterer det enkelte tryk som forårsaget wake up

### Wi-Fi
- Hvad sker der hvis wifi fejler? log til file og submit senere?
    - Dependencien på wifi i hvert wake up/knappe tryk er blevet fjernet(i forhold til poc) og vi logger istedet til fil som kan syncs senere
- Måske Wifi i sync step istedet for hvert klik, gem til file i mellem tiden.
    - Wifi er flyttet til kun at håndteres i "sync" steppet til både at sync fra vores locale log fil til mqtt & sync ntp

## TODO

### MQTT
**Hvordan implementerer vi dette?**

### TLS-kryptering
**Hvordan implementerer vi dette?**

### DeepSleep
**Analyse:**
- Hvor længe skal anordningen være aktiv inden DeepSleep aktiveres?
- Hvordan skal opvågen fungere?
- Altid sleep efter hver handling eller forbliv oppe og håndtere flere klik og sleep efter x tid
