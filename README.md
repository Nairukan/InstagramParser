Ухожу в армию поэтому не смогу дальше помогать вам, поэтому открыл репозиторий, сделал установщик (Смотрите в релизах) и написал маломальскую инструкцию. Прога на Винде получилась косячная, но попытался по максимуму исправить. Возвращаюсь в начале мая тогда смогу исправить что-то. Отправление у меня 4 ноября, поэтому до того времени можете писать вопросы - на всё отвечу. С любовью, ваш Никита

Инструкция
Перед запуском необходимо заполнить файлы:
- parsing_acc.txt (по одному имени аккаунта информацию с которого нужно собирать (без пробелов))
например ссылка на аккаунт https://www.instagram.com/nikita_parser/, значит в список пишем nikita_parser
- Init.ini (в первой строке пишем логин, а во второй пароль от аккаунта от имени которого будет работать парсер)

Параметры запуска:
По умолчанию запускается без параметров (сбор с нуля в стандартном формате)
Существуют также другие параметры для их вызова необходимо запускать программу из консоли

Доп Параметры:
- ".\InstagramParser.exe -ExelOnly" (не производит непосредственный сбор, после ввода промежутка, ищет ранее сделанные отчёты (возможно незавершённые) и собирает их вместе)
Особенно полезно если в случае сбора набор был собран 2 частями, и необходимо их объеденить или сделать подвыборку.
!ДАТЫ НАЧАЛА  И КОНЦА ДОЛЖНЫ АБСОЛЮТНО СОВПАДАТЬ!

- ".\InstagramParser.exe -IgnorList" (Включает режим фильтровки постов, читает файл ignor.txt, содержащий в себе ссылки на рилсы которе необходимо игнорировать)
- ".\InstagramParser.exe -FromResource result\csv_s\ParsingProtocol_start2023_10_10_end2024_10_10" (Берёт информацию не из сети а из ранее собранного отчёта, то есть например можно собрать за год, и после на основе этого файла, собрать по каждому месяцу)
!ФОРМТЫ ОТЧЁТОВ ДОЛЖНЫ СОВПАДАТЬ!

1. После запуска будет предложено ввести формат (Во всех режимах кроме ExelOnly)
Буквы отвечают за информацию в столбцах, например DlV - отчёт будет состоять из следующей последовательности столбцов (Полная дата рилса | Ссылка на него | Кол-во просмотров)
2. После ввода формата будет предложено ввести интересующий диапазон
Формат дат может быть полным или сокращённым, например 10.10.2024 или 10.10.2024_00:00:00 что соответсвует (10 октября 2024 года 0 часов 0 минут), однако реккомендуется для начального времени указывать час ночи, а для конечного час ночи следующего дня,
например нужно собрать 21-23 октября (значит наши даты будут 21.10.2024_01:00:00 по 24.10.2024_01:00:00)
3. Далее будет запущен процесс сбора.

Рекоммендуется использовать VPN, для обхода блокировок отображения рилсов из-за используемой музыки и подобном.

В процессе его выполнения могут быть проблемы с указанными аккаунтами, более подробная информация о причинах будет выведена на экран и дополнительно сохранена в bad.acc (рекоммендуется после решения проблем отдельно их собрать и объеденить используя параметр ExelOnly).

В случае запроса подтверждения от инстаграмма, программа остановиться и напишет об этом. После нужно в браузере или на другом устройстве войти в этот аккаунт и пройти необходимые действия. 

Если работа программы завершилась ошибкой, не будет потерена информация о аккаунтах с первого по последний который писался Record <Имя аккунта>, то есть их можно удалить из parsing_acc.txt и начать сбор занаво, и далее поместить в parsing_acc общий список и объеденить информацию используя ExelOnly.

При многократных ошибках входа (скорее всего вас забанили по IP) реккомендуется сменить VPN (полученный адресс) и продолжить.

После окончания работы будет выведена информация в консоль (Если вы запускали не из консоли - не успеете увидеть, она закроется сразу), формата аккаунт просмотры и снизу суммарное кол-во

Результаты Будут сохранены в папке \result\xlsx_s\ParsingProtocol_start2023_10_10_end2024_10_10 (У вас будут другие даты) и будут представлены в 3-х файлах (Raw, One, True)
- Raw (Отчёт по заданному формату без разделения просмотров междно сооавторами рилсов, то есть в каждом аккаунте будет написано полное кол-во просмотров, так-же в крайнем поле будет список соавторов для этого рилса)
- One (Отчёт одностраничный содержащий только список всех рилсов, их просмотры и авторов)
- True (Наподобии Raw но уже с распределёнными просмотрами и вместо списка соавторов, доля от полных просмотров)

К сожалению мои таблицы для форматирования, и делания таблиц красивыми отдать не могу из-за технических сложностей, Извините:(. В Репозитории есть файл myGoogleScriptForFormat.txt который содержит скрипты в моём документе для форматирования из google.sheets . Если разберётесь то окей, а если нет ещё раз Извините

 
