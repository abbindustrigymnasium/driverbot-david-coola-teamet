![New Project(21)](https://github.com/abbindustrigymnasium/driverbot-david-coola-teamet/assets/110134664/0fe113fc-cbc4-4f04-b2f2-12c4a163622b)
![New Project(22)](https://github.com/abbindustrigymnasium/driverbot-david-coola-teamet/assets/110134664/eeb26018-4bb7-469f-9518-ba52829b64e8)
![New Project(1)(1)](https://github.com/abbindustrigymnasium/driverbot-david-coola-teamet/assets/110134664/8c92283b-a4c4-4fed-be80-5279af22f125)

# Min driver bot mål(Software):

1. Skapa en hemsida med hjälp av svelte

2. upprätta en anslutning till en server med hjälp av mqtt

3. Skicka data från ui joysticks(se bild) till driverbot genom en broker.

4. Uppfatta datan med arduino kod för att ta emot rörelse datan från hemsidan för att röra och styra bilen.

**Om jag hinner**
1. Lägg till en status symbol(se bild "Status: active") för att visa om driverbot har en anslutning och är redo att använda
2. Lägg till autonomous sida på websidan för att starta path finding på driver bot
3. Lägg till funktionen för autonomous sida(lägg till path finding komponenter och software till driverbot)
  ideer för en autonomous driverbot
    1. Yolov8 object detection för att identifera hinder(obs kräver rasberry pi/pc för realtime detection)
    2. Line follower(kräver färg sensor för att följa en svart linje på marken)
    3. Sonar/Laser distance sensor för att identifera hinder(enkel autonomi. Problem kan vara att den åker i cirklar utan mål)
4. I autonomi fliken lägg till data som "Time online" och "Objects detected"
5. lägg till hamburger menu(se bild) för att ge flera menyer som t.ex data.


# Tidsplan:
**Tisdag 23/4:** Börja på HTML kod för att göra om psd sketchen till en väldigt enkel hemsida.

**Onsdag 24/4:** Tidsbuffert för att göra klart Html

**Fredag 26/4:** Research och implementering av initialisering av MQTT

**Måndag 29/4:** Tidsbuffert ifall jag har problem med att skapa en connection med broker

**Tisdag 30/4:** Mål att börja skicka packets från hemsida till mqtt broker

**Måndag 6/5:** Mål att skicka packets från mqtt broker till esp8266 och läsa rörelse datan

**Tisdag 7/5:** Använd rörelse datan från mqtt broker för att driva motorerna och om det behövs minska delay mellan input på websidan och output till motorn.

**Måndag 13/5 och Tisdag 14/5:** Tidsbuffert eftersom att det kan finnas problem med packets och mqtt. Kan också användas för att bygga klart bilen

**Måndag 20/5:** Göra hemsidan mer komplett och börja research på path finding för driver bot

**Måndag 21/5:** Implementera path finding lokalt på driverbot 

**Torsdag 23/5:** Lägg till funktion på autonomous sidan på websidan så att start knappen aktiverar path finding

**Måndag 27/5 och Tisdag 28/5** Tidsbuffert för att göra hemsidan mer komplett och för att försäkra att allt funkar

**Fredag 31/5** Uppkörning 😱




    
