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


## TODO

### Debounce
**Hvordan løser vi dette?**

### Wi-Fi
**Hvad sker der hvis wifi fejler? log til file og submit senere?**

### MQTT
**Hvordan implementerer vi dette?**

### TLS-kryptering
**Hvordan implementerer vi dette?**

### DeepSleep
**Analyse:**
- Hvor længe skal anordningen være aktiv inden DeepSleep aktiveres?
- Hvordan skal opvågen fungere?