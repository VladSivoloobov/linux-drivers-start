# 🧰 Сборник простых драйверов Linux

Привет! Это мой личный репозиторий, в котором я собираю **простые примеры драйверов для ядра Linux**, которые я написал или буду писать по мере изучения разработки модулей ядра.

Цель — систематизировать знания, создавая рабочие примеры, которые можно легко понять, протестировать и использовать как основу для более сложных проектов.

---

## 📁 Содержание

1. [Установленные драйверы](##📦-установленные-драйверы)
2. [Планируемые драйверы](##📅-планируемые-драйверы)
3. [Как собрать и запустить](##🛠️-как-собрать-и-запустить)
4. [Цель проекта](##🎯-цель-проекта)
5. [Что я уже изучил](##🧠-что-я-уже-изучил)
6. [Лицензия](##📝-лицензия)

---

## 📦 Установленные драйверы

На данный момент в репозитории находятся следующие драйверы:

### 1. 🧪 "Hello World" модуль

- **Описание:** Простейший модуль ядра, который выводит `"Hello world, loaded!"` при загрузке и `"Goodbye world!"` при выгрузке.
- **Используется для:** Основ работы с `module_init`, `module_exit`, `printk`.

### 2. 💬 Символьный драйвер

- **Описание:** Реализация символьного устройства `/dev/Hello`. Поддерживает чтение и запись данных между пользовательским и ядерным пространством.
- **Используется для:** Изучения `file_operations`, `cdev`, `device_create`, `copy_to_user`, `copy_from_user`.

---

## 📅 Планируемые драйверы

В будущем планируется добавить примеры следующих типов драйверов:

| Тип              | Описание                                                 |
| ---------------- | -------------------------------------------------------- |
| **misc**         | Пример misc-устройства                                   |
| **platform**     | Драйвер на основе platform_device/platform_driver        |
| **input**        | Драйвер устройства ввода (например, клавиатура или мышь) |
| **i2c/spi**      | Примеры драйверов для шин I²C и SPI                      |
| **procfs/sysfs** | Работа с файлами в `/proc` и `/sys`                      |
| **gpio**         | Управление GPIO через драйвер                            |
| **interrupts**   | Обработка аппаратных прерываний                          |

Если у тебя есть идеи — смело открывай issue или pull request 😊

---

## 🛠️ Как собрать и запустить

### 1. Убедитесь, что установлены зависимости:

```bash
sudo apt update
sudo apt install build-essential linux-headers-$(uname -r)
```

### 2. Перейдите в папку нужного драйвера и соберите его:

```bash
cd driver/
make
```

### 3. Загрузите модуль в ядро:

```bash
sudo insmod main.ko
```

### 4. Проверьте логи ядра:

```bash
dmesg | tail -n 20
```

### 5. Выгрузите модуль после тестирования:

```bash
sudo rmmod hello
```

---

## 🎯 Цель проекта

Этот репозиторий создан:

- Для **обучения** работе с ядром Linux.
- Как **набор минимально рабочих примеров (MWE)** для быстрого старта.
- Чтобы помочь другим, кто хочет начать изучать разработку драйверов.

---

## 🧠 Что я уже изучил

- Как создавать базовые модули ядра.
- Работу с `init` и `exit` функциями.
- Создание символьных устройств (`cdev`).
- Использование `file_operations` для реализации `read`, `write`.
- Безопасную передачу данных между ядром и пользовательским пространством (`copy_to_user`, `copy_from_user`).
- Логирование через `printk`.
- Автоматическое создание файла устройства в `/dev`.

---

## 📝 Лицензия

Все исходные коды распространяются под лицензией **GNU General Public License v2.0**.  
Ты можешь свободно использовать, изменять и распространять эти драйверы, при условии указания оригинального авторства и открытости производных работ.

---

## 🤝 Хочешь помочь?

- Открывай **issue**, если нашёл баг или хочешь предложить улучшение.
- Делай **pull request**, если хочешь добавить свой драйвер или улучшить существующие.
- Делись ссылкой, если кому-то будет полезно!

---

Спасибо за интерес к проекту! 🙌  
_— Владислав_
