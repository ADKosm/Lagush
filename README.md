# Lagush

Космачев Алексей Дмитриевич. 142-2. Веб-сервер с функциональностью CGI.

CGI Web-Server

Веб-сервер написанный на С++ с поддержкой CGI (Common Gateway Interface).

Данный веб-сервер компактен и не требователен к ресурсам.

Сервер имеет средства изоляции для потенциально-опастных скриптов.

#Критерии

1)Реализован веб-сервер, который умеет выдавать статические страницы и обслуживать одновременно несколько подключений
   
Основной процесс прослушивает указанный в конфигурации порт и при появлении нового соединения создает дочерний процесс. Дочерний процесс считывает запрос, и далее выдает запрошенный контент. Основной процесс при этом сразу переходит в состояние ожидания нового соединения.
    
2)Помимо выдачи статического содержимого, сервер умеет выполнять сторонние программы и выдавать результат их работы

Если запрошенный файл является СGI-скриптом, то создается новый процесс, у него перенаправляются основные потоки ввода\вывода и запускается CGI скрипт. Выдача скрипта передается клиенту.

3)Реализована функциональность прерывания работы и изоляции потенциально опасных CGI-скриптов.

Если включен режим изоляции, то при запуске сервер подгатавливает указанную директорию: переносит необходимые файлы. При запросе CGI-скрипта, сервер копирует необходимые файлы для запуска, переносит скрипт в защищенную директорию, с помошью chroot меняет коревой каталог. Далее сервер работает с ним, так же как и с любым другим CGI-скриптом (см. выше.)

4)Реализована поддержка POST-запросов

При считывании заголовков, сервер также умеет распозновать POST запросы: он считывает переданные данные и отдает их CGI-скрипту.

# Config file
На данный момент сервер берет настройки из файла конфигурации.

Файл должен называться config и лежать в одной из сделующих директорий

* /path/to/server/config
* /path/to/server/../config
* /path/to/server/../CGI/config
* /path/to/server/../Lagush/config

Пример файла конфигурации:

    # Основная директория
    root_directory:/path/Lagush/web

    # Директория с стандартными ответами сервера (404 и т.д.)
    responses:/path/Lagush/responses

    # Файл для записи логов
    log:/path/Lagush/log/logfile

    # Порт, занимаемый сервером
    port:3001

    # Время ожидания ответа от клиента
    timelimit:10

    # Расширения файлов, являющиеся CGI-скриптами
    cgi: .cgi .py 

    # Путь к jail директории
    jail:/path/to/jail

    # Включить ли защищенный режим или нет
    jail_enable:true

Важный момент - все пути абсолютные

Защищенный режим не позволяет запускаемым сторонним скриптам нанести урон реальной системе. Однако данный режим несколько замедляет запуск скриптов (особенно первый запуск - сервер должен перенести все необходимые библиотеки для работы скрипта).

# Build

Склонируйте репозиторий

В директории Lagush запустите qmake-qt4

При необходимости установите qmake:

    sudo apt-get install qt4-qmake

(команда установит только qmake)

Запустите make

В CGI-build (или другой подходящей папке (см. выше)) создайте конфиг-файл

Если Вам требуется изоляция потенциально опастных скриптов, установите SUID флаг на cgi-module

Запустите сервер

    ./server

??????

PROFIT!
