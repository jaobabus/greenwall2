# greenwall2

Консольный генератор матричных эффектов с наложением изображения/видео.  
**Linux-only**.

## Требования

- C++17+
- CMake
- FFmpeg библиотеки: libavformat-dev, libavcodec-dev, libavutil-dev, libswscale-dev, libswresample-dev
- Boost.Program_options
- minijson (https://github.com/snorrwe/minijson)

## Сборка

```bash
sudo apt update
sudo apt install build-essential cmake libavformat-dev libavcodec-dev libavutil-dev libswscale-dev libswresample-dev libboost-program-options-dev
git clone https://github.com/snorrwe/minijson.git external/minijson
mkdir build
cd build
cmake .. -DCMAKE_CXX_FLAGS="-I../external/minijson/src"
cmake --build .
```

## Запуск

```bash
./greenwall2 --config config.json
```

Если файл конфигурации отсутствует, он будет создан автоматически.

## Параметры

```bash
./greenwall2 --help
```

- `-c, --config <file>` — путь к конфигурации (обязательный параметр)
- `-D, --variable <key=value>` — установить переменную конфигурации
- `-H, --config_list` — показать список переменных
- `-S, --config_schema` — показать схему конфигурации

## Конфигурация

Схема доступна командой:

```bash
./greenwall2 --config_schema
```

## Примечания

- При первом запуске без `config.json` создаётся дефолтный файл.
- Для видео используется только FFmpeg API.
