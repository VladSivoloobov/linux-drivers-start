# 📦 Простой символьный драйвер в Linux

Этот проект реализует **простой символьный драйвер** для ядра Linux. Драйвер создаёт виртуальное устройство `/dev/Hello`, через которое можно:

- Записывать данные из пользовательского пространства.
- Читать эти данные обратно.

Он предназначен для обучения и демонстрации базовых принципов разработки драйверов в ядре Linux.