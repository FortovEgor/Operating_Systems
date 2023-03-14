### Фортов Егор Кириллович, БПИ214
### Вариант 23

## Задание:
Разработать программу, которая ищет в ASCII-строке заданную подстроку и возвращает индекс первого символа первого вхождения подстроки в строке. Подстрока вводится как параметр.
> Программа на 4 балла (именованные каналы, 3 процесса) ![Build Status](https://travis-ci.org/joemccann/dillinger.svg?branch=master) ![Best code editor](https://img.shields.io/badge/Visual_Studio_Code-0078D4?style=for-the-badge&logo=visual%20studio%20code&logoColor=white) ![Github rules](https://img.shields.io/badge/GitHub-100000?style=for-the-badge&logo=github&logoColor=white)

![](https://viewer.diagrams.net/?tags=%7B%7D&highlight=0000ff&edit=_blank&layers=1&nav=1&title=scheme.png#R7Vttc6M2EP41zLQf4gHEi%2F0xtuO0nd40M7m5Nh8JyKAeRhzgmPTXVwIBEoIYOzG229wHTqzeVs%2FuPiyLo4DFJr9PnDj4gj0YKrrq5QpYKrpuTgG5UsFrKbAAE%2FgJ8kqR1gge0T%2BQCVUm3SIPpsLADOMwQ7EodHEUQTcTZE6S4J04bI1DcdfY8aEkeHSdUJb%2BibwsKKVT3W7kv0DkB9XOmjUrezZONZidJA0cD%2B84EbhTwCLBOCtbm3wBQ4pdhUs5b9XTWyuWwCgbMuHLD9WFKxDvvu6%2B%2FWb%2BvXqK%2FPWNXq7y4oRbdmC2IFk5wS5MU6Z89lohkuBt5EG6qKaA%2BS5AGXyMHZf27ogLEFmQbULWLSvJ9H6BSQZzTsSUvod4A7PklQxhvbrFAGQepJnsftfYQ1OZLOBsUY1zmAv49dINSqTBgDoANDAANPWnBG4cFKU%2Fnx1AbSoCqJ8dQEMC0A0QCUweP0UH2tmhA8alQWdfme%2FpIoDg7ABqMuV1Op9%2Bdux04%2BKwkwP3or3PFAE0zg%2BgNcz5wNmh02zz0rCbSthJKJEcK6bNdQjzW5r9ETBg5LHm0g2dNEWuCFaDrFqOrvI8u7xlOak9IVYB83WId7cRItkdwtE%2BuKEnJJcy2ByY5htYJjAk%2B72IKWkXwGyHB4yKmKzDoPUUs42JKS6S4m3iQjaPTyJbSxntB6K8VOYkPsykpQqb10d%2FhxvMJDf4NYq3VMc1In7Q5xIBzB2fmmwewwQRLWDSSB8qkb4%2FuNYoh5VTfBhP7U8zZmPGWvXU%2Foy1Q2ONhMhEZM7auIfGWsdSVmupE8da7cmNG%2Fyxza482NqvQzWoIwSb7%2BbY2H5H29jxwEuQ3iPg3sggS7geGFc5yv6i7YlustsnNpC2lzk3bvnKbg4KttKF3%2FKcgUH53mhrZSn68bHWWqht7Y%2BLtE4fkF9KFN0KM2oXXOjZOIP1Y4urjpu0CI1bMkAz4rwwWdWv0KJYiBNhEskt12tVVae8yPLp%2F1q1IzlAuWkpl52RGPV35xmG1PEg0cB5LrpU0SedEPnUkVziPDT85zREkeuEt6xjgzyPTuxws7fihFX62KZKXV8bwACDve1d8SyXhz4snu3h8UywTF7LWWZ1%2B8T3NdOKu5PyQBk9b4wzxuELAFqJ1tF8YfdkbCPxhfwi%2FskXPXF48XxhSrY8SwIwCmHsJQJ7pMShXZc8lgi01tcJMDIRyAWl0Ymg3vE5aTjgsqnBvBJqGFDy%2Bq%2BkEnuZoSrcn5wa2mX3o3OEmfhOMTY1yIWy81HDpRLB9EqIQOsqyAnG7LeLKtjvGKMfaEwCWSbaLc0S%2FB0umIoRjmBR2wnDlmi4fbvqRiLV9TLN8FKR0Zflc6UivaNUpJ2sVHQZtaILeSCM9NKotT8jHp0rtsr8BjAmM%2F7fqI%2BHjg%2Fhoz8fwPWljtq1lKE6vjV%2FGFdYlsaRhUq8%2BDSEQRd54L4gfHyByh5KNtZIZNP6OmG0nyWDyaZV6jJO9%2B2o2%2F3kXyp90ssAerGuhV663k1HzkiN%2F11GqrdSCFM7d0ba%2F5YZaJ3GDVEEbyrdqHlpemV22vdkDjXtcSiF2Gm2UoiHEBom7fm0uC6Kq1nI7aJd9i65MWavL9ZiCogojHtOuv8E2lsnKDXVCr3mTEe9VNkornqlslqNJFedO7fJzb0VV5AhuWPTqeMNNFgHWRcL3nFKTjmMVU6Tu%2BoIZNNy1qzSbckN4DVUe42jluo5GxqutaxZZSWswlQhbYtTkRxmUc2qVSwBrdsqp64pbrHk8K3NVnbx7fqQK8lOhmjFGi%2B9siKRr0RT1QrzOK647eaVbtLcZq8Vp1KNjMopMOeuJqfMgrfWZH%2FkxCdg9pRQNYp8IjCbu6%2BY%2BMGS%2Fv6pm9AxoWqagBJJQIgfRqcgedD%2BptDxCxWt6ycq9a%2BtD6B5ctv8DUqZ%2FDV%2FyAPu%2FgU%3D)

### Входные данные
Запуск программы происходит с помощью выполнения исполняемого файла с 3 аргументами командной строки:
 - Имя файла (с расширением), в котором лежит строка (просто цельная строка, без доп. символов), в которой мы будем искать подстроку;
 - Имя файла (с расширением), в который мы будем писать ответ;
 - Подстрока (цельная строка без доп. символов).


### Отчёт о реализации программы
Решение данной задачи было спроектировано, используя многопроцессорность и неимеованные каналы. Было сделано 3 дочерних процесса:
 - 1-ый процесс (child process #1) читает текстовые данные из заданного файла (Input file) и через неименованный канал передает их второму процессу;
 - 2-ой (child process #2) процесс осуществляет обработку данных в соответствии с заданием и передает результат обработки через неименованный канал третьему процессу;
 - 3-ий (child process #3) процесс осуществляет вывод данных в заданный файл (Output file).

> Схема, демонстрирующая архитектуру приложения:

<img src="scheme.png" alt="Picture for VS Code" />


### Результаты тестирования
Было проведено 5 тестов (str - строка, в которойищем подстроку s):
 - str = "abcdefghjklmn"; s = "def"; RESULT: 3 (correct)
 - str = "klklksssmensjs"; s = "sss"; RESULT: 5 (correct)
 - str = "askdlfdekhefkjcabk"; s = "hefkj"; RESULT: 9 (correct)
 - str = "kjdsdjkbnjcefjnledkjbwfejkcewjklefwbn"; s = "lefwbn"; RESULT: 31 (correct)
 - str = "jkewncalkefunqwklruilqwadjkbefjkfkabekfuif"; s = "lefwbn"; RESULT: -1 (correct)






