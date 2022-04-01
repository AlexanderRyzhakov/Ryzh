# Transport Catalogue
---
Транспортный справочник, реализовано добавление остановок с названием и координатами и маршрутов. 

![map](https://downloader.disk.yandex.ru/preview/a04afda2b45ef06049865f6585f6942a5e60c4cdd0e2f44e41a3dc20a6f6205c/62476392/d8B-YuYEkT5v5j0P9XW1AUewc7Ldimf8-2SltRK5ymI2BoTsuTLlajnRank8M8DmEGdj9phA_9cIWpy4duDxvQ%3D%3D?uid=0&filename=transport_catalogue.jpg&disposition=inline&hash=&limit=0&content_type=image%2Fjpeg&owner_uid=0&tknv=v2&size=2048x2048)
 
## Использование: 
---
1.	Запросы к базе в формате JSON (пример запроса ниже)
2.	Добавление к базе идет в блоке “ base_requests” остановки – по команде “Stop” (задаем название, координаты, расстояние до других остановок); маршрут – команда “Bus” (задаем название и список остановок)
3.	В блоке “render_settings” задаются параметры графики (SVG)
4.	Блок "stat_requests" – запросы к базе. Выводится статистика для маршрута или остановки, либо строка в формате SVG для отрисовки карты.

## Системные требования:
---
1.	С++17
2.	GCC 11.20

## Стек технологий:
---
1.	С++17
2.	XML
3.	SVG
4.	JSON

## Пример запроса для отрисовки карты 
---
```
{
    "base_requests": [
      {
        "type": "Stop",
        "name": "Sportivnaya",
        "latitude": 17.8,
        "longitude": 19.0
      },
      {
        "type": "Stop",
        "name": "Frunzenskaya",
        "latitude": 32.1,
        "longitude": 33.3
      },
      {
        "type": "Stop",
        "name": "Park Yandex",
        "latitude": 43.4,
        "longitude": 48.4
      },
      {
        "type": "Stop",
        "name": "Kropotkinskaya",
        "latitude": 50.4,
        "longitude": 65.8
      },
      {
        "type": "Stop",
        "name": "Biblioteka Lenina",
        "latitude": 58.8,
        "longitude": 79.8
      },
      {
        "type": "Stop",
        "name": "Ohotniy riad",
        "latitude": 80.2,
        "longitude": 101.3
      },
      {
        "type": "Stop",
        "name": "Lubianka",
        "latitude": 96.4,
        "longitude": 117.2
      },
      {
        "type": "Stop",
        "name": "Chistyie prudi",
        "latitude": 112.5,
        "longitude": 133.8
      },
      {
        "type": "Stop",
        "name": "Krasnie vorota",
        "latitude": 120.2,
        "longitude": 141.5
      },
      {
        "type": "Stop",
        "name": "Komsomolskaya",
        "latitude": 128.2,
        "longitude": 148.6
      },
      {
        "type": "Stop",
        "name": "Krasnoselskaya",
        "latitude": 137.9,
        "longitude": 159.0
      },
      {
        "type": "Stop",
        "name": "Sokolniki",
        "latitude": 153.5,
        "longitude": 175.1
      },

      {
        "type": "Bus",
        "name": "1",
        "stops": ["Sportivnaya", "Frunzenskaya", "Park Yandex", "Kropotkinskaya", 
"Biblioteka Lenina", "Ohotniy riad", "Lubianka", "Chistyie prudi", "Krasnie vorota",
 "Komsomolskaya", "Krasnoselskaya", "Sokolniki"],
        "is_roundtrip": false
      },


      {
        "type": "Stop",
        "name": "Shabolovskaya",
        "latitude": 11.8,
        "longitude": 66.1
      },
      {
        "type": "Stop",
        "name": "Oktyabrskaya",
        "latitude": 26.5,
        "longitude": 75.6
      },
      {
        "type": "Stop",
        "name": "Trtiakovskaya",
        "latitude": 50.6,
        "longitude": 123.2
      },
      {
        "type": "Stop",
        "name": "Kitay-gorod",
        "latitude": 86.0,
        "longitude": 132.0
      },
      {
        "type": "Stop",
        "name": "Suharievskaya",
        "latitude": 135.6,
        "longitude": 119.0
      },
      {
        "type": "Stop",
        "name": "Prospekt mira",
        "latitude": 144.3,
        "longitude": 126.2
      },
      {
        "type": "Stop",
        "name": "Rizhskaya",
        "latitude": 159.4,
        "longitude": 132.4
      },
      {
        "type": "Stop",
        "name": "Alekseevskaya",
        "latitude": 171.3,
        "longitude": 143.1
      },
      {
        "type": "Stop",
        "name": "VDNKH",
        "latitude": 178.9,
        "longitude": 143.1
      },
      {
        "type": "Stop",
        "name": "Nerd garden",
        "latitude": 191.6,
        "longitude": 142.6
      },

      {
        "type": "Bus",
        "name": "2",
        "stops": ["Shabolovskaya", "Oktyabrskaya", "Trtiakovskaya", "Kitay-gorod", 
"Chistyie prudi", "Suharievskaya", "Prospekt mira", "Rizhskaya", "Alekseevskaya",
 "VDNKH", "Nerd garden"],
        "is_roundtrip": false
      },


      {
        "type": "Stop",
        "name": "Kurskaya",
        "latitude": 98.74,
        "longitude": 163.3
      },
      {
        "type": "Stop",
        "name": "Taganskaya",
        "latitude": 58.8,
        "longitude": 158.1
      },
      {
        "type": "Stop",
        "name": "Paveleckaya",
        "latitude": 38.5,
        "longitude": 142.7
      },
      {
        "type": "Stop",
        "name": "Dobrininskaya",
        "latitude": 21.1,
        "longitude": 103.3
      },
      {
        "type": "Stop",
        "name": "Kievskaya",
        "latitude": 74.7,
        "longitude": 36.4
      },
      {
        "type": "Stop",
        "name": "Krasnopresnenskaya",
        "latitude": 97.4,
        "longitude": 36.1
      },
      {
        "type": "Stop",
        "name": "Belorusskaya",
        "latitude": 127.9,
        "longitude": 51.2
      },
      {
        "type": "Stop",
        "name": "Novoslobodskaya",
        "latitude": 147.9,
        "longitude": 83.3
      },

      {
        "type": "Bus",
        "name": "5",
        "stops": ["Kurskaya", "Taganskaya", "Paveleckaya", "Dobrininskaya", 
"Oktyabrskaya", "Park Yandex", "Kievskaya", "Krasnopresnenskaya", "Belorusskaya",
 "Novoslobodskaya", "Prospekt mira", "Komsomolskaya", "Kurskaya"],
        "is_roundtrip": true
      }


    ],
    "render_settings": {
      "width": 800,
      "height": 800,
      "padding": 30,
      "stop_radius": 5,
      "line_width": 14,
      "bus_label_font_size": 20,
      "bus_label_offset": [7, 15],
      "stop_label_font_size": 18,
      "stop_label_offset": [7, -3],
      "underlayer_color": [255,255,255,0.85],
      "underlayer_width": 3,
      "color_palette": [[223,0,26], [252,117,34],[151,77,51]]
    },
    "stat_requests": [
      { "id": 1, "type": "Map" }
    ]
  }
```
